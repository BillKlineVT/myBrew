"""
data-service: FastAPI CRUD service for all myBrew persistent data.
Backed by PostgreSQL via SQLAlchemy async + asyncpg.
"""
import os
import logging
from contextlib import asynccontextmanager
from datetime import datetime, timezone
from typing import Optional

import xmltodict
from fastapi import FastAPI, Depends, HTTPException, UploadFile, File
from sqlalchemy import select, desc, delete, text
from sqlalchemy.ext.asyncio import AsyncSession, create_async_engine, async_sessionmaker
from sqlalchemy.orm import selectinload

from models import (
    Base,
    SensorReading, GPIOState, PIDSettings, BrewerySetting,
    BrewSession, ChecklistEntry, Recipe, HopAddition, MashStep,
    SensorReadingSchema, GPIOStateSchema, PIDSettingsSchema, BrewerySettingSchema,
    BrewSessionSchema, ChecklistEntrySchema, RecipeSchema,
    HopAdditionSchema, MashStepSchema,
)

logging.basicConfig(level=logging.INFO)
log = logging.getLogger("data-service")

DATABASE_URL = os.getenv(
    "DATABASE_URL",
    "postgresql+asyncpg://mybrew:mybrew@localhost:5432/mybrew"
)

engine = create_async_engine(DATABASE_URL, echo=False, pool_pre_ping=True)
SessionLocal = async_sessionmaker(engine, expire_on_commit=False)


@asynccontextmanager
async def lifespan(app: FastAPI):
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)
        # Ensure columns exist for existing deployments
        await conn.execute(text(
            "ALTER TABLE brew_sessions ADD COLUMN IF NOT EXISTS ended_at TIMESTAMP WITH TIME ZONE"
        ))
        await conn.execute(text(
            "ALTER TABLE brew_sessions ADD COLUMN IF NOT EXISTS name VARCHAR"
        ))
        await conn.execute(text(
            "ALTER TABLE brew_sessions ADD COLUMN IF NOT EXISTS recipe_id INTEGER REFERENCES recipes(id)"
        ))
    log.info("Database tables created/verified")
    yield
    await engine.dispose()


app = FastAPI(title="myBrew Data Service", lifespan=lifespan)


async def get_db():
    async with SessionLocal() as session:
        yield session


# ── Sensor Readings ──────────────────────────────────────────────────────────

@app.get("/sensors", response_model=list[SensorReadingSchema])
async def list_sensors(limit: int = 100, db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(SensorReading).order_by(desc(SensorReading.timestamp)).limit(limit)
    )
    return result.scalars().all()


@app.get("/sensors/latest", response_model=SensorReadingSchema)
async def latest_sensor(db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(SensorReading).order_by(desc(SensorReading.timestamp)).limit(1)
    )
    row = result.scalar_one_or_none()
    if row is None:
        raise HTTPException(status_code=404, detail="No sensor readings yet")
    return row


@app.post("/sensors", response_model=SensorReadingSchema, status_code=201)
async def create_sensor_reading(payload: SensorReadingSchema, db: AsyncSession = Depends(get_db)):
    row = SensorReading(**payload.model_dump(exclude={"id", "timestamp"}))
    row.timestamp = datetime.now(timezone.utc)
    db.add(row)
    await db.commit()
    await db.refresh(row)
    return row


# ── GPIO State ───────────────────────────────────────────────────────────────

@app.get("/gpio", response_model=GPIOStateSchema)
async def get_gpio(db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(GPIOState).order_by(desc(GPIOState.timestamp)).limit(1)
    )
    row = result.scalar_one_or_none()
    if row is None:
        return GPIOStateSchema()
    return row


@app.put("/gpio", response_model=GPIOStateSchema, status_code=201)
async def set_gpio(payload: GPIOStateSchema, db: AsyncSession = Depends(get_db)):
    row = GPIOState(
        gpio_18=payload.gpio_18,
        gpio_22=payload.gpio_22,
        gpio_24=payload.gpio_24,
        gpio_27=payload.gpio_27,
        timestamp=datetime.now(timezone.utc),
    )
    db.add(row)
    await db.commit()
    await db.refresh(row)
    return row


# ── PID Settings ─────────────────────────────────────────────────────────────

@app.get("/pid/{vessel}", response_model=PIDSettingsSchema)
async def get_pid(vessel: str, db: AsyncSession = Depends(get_db)):
    vessel = vessel.upper()
    result = await db.execute(select(PIDSettings).where(PIDSettings.vessel == vessel))
    row = result.scalar_one_or_none()
    if row is None:
        raise HTTPException(status_code=404, detail=f"No PID settings for vessel {vessel}")
    return row


@app.put("/pid/{vessel}", response_model=PIDSettingsSchema)
async def set_pid(vessel: str, payload: PIDSettingsSchema, db: AsyncSession = Depends(get_db)):
    vessel = vessel.upper()
    result = await db.execute(select(PIDSettings).where(PIDSettings.vessel == vessel))
    row = result.scalar_one_or_none()
    if row is None:
        row = PIDSettings(vessel=vessel)
        db.add(row)
    row.set_temp = payload.set_temp
    row.p_val = payload.p_val
    row.i_val = payload.i_val
    row.d_val = payload.d_val
    row.enabled = payload.enabled
    row.updated_at = datetime.now(timezone.utc)
    await db.commit()
    await db.refresh(row)
    return row


# ── Brewery Settings (key-value) ──────────────────────────────────────────────

@app.get("/settings", response_model=list[BrewerySettingSchema])
async def list_settings(db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(BrewerySetting))
    return result.scalars().all()


@app.get("/settings/{key}", response_model=BrewerySettingSchema)
async def get_setting(key: str, db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(BrewerySetting).where(BrewerySetting.key == key))
    row = result.scalar_one_or_none()
    if row is None:
        raise HTTPException(status_code=404, detail=f"Setting '{key}' not found")
    return row


@app.put("/settings/{key}", response_model=BrewerySettingSchema)
async def set_setting(key: str, payload: BrewerySettingSchema, db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(BrewerySetting).where(BrewerySetting.key == key))
    row = result.scalar_one_or_none()
    if row is None:
        row = BrewerySetting(key=key)
        db.add(row)
    row.value = payload.value
    await db.commit()
    await db.refresh(row)
    return row


# ── Brew Sessions ────────────────────────────────────────────────────────────

CHECKLIST_STEPS = [
    "Mill grain",
    "Heat strike water",
    "Mash in",
    "Mash rest",
    "Vorlauf",
    "Sparge",
    "Boil",
    "Add hops",
    "Chill wort",
    "Transfer to fermenter",
    "Pitch yeast",
    "Clean equipment",
]


@app.get("/sessions", response_model=list[BrewSessionSchema])
async def list_sessions(db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(BrewSession).order_by(desc(BrewSession.started_at)))
    return result.scalars().all()


@app.post("/sessions", response_model=BrewSessionSchema, status_code=201)
async def create_session(payload: BrewSessionSchema, db: AsyncSession = Depends(get_db)):
    session = BrewSession(
        started_at=datetime.now(timezone.utc),
        name=payload.name,
        pre_boil_gravity=payload.pre_boil_gravity,
        mash_ph=payload.mash_ph,
        ground_water_temp=payload.ground_water_temp,
    )
    db.add(session)
    await db.flush()
    # Auto-populate checklist
    for step in CHECKLIST_STEPS:
        db.add(ChecklistEntry(session_id=session.id, step_name=step))
    await db.commit()
    await db.refresh(session)
    return session


@app.patch("/sessions/{session_id}/end", response_model=BrewSessionSchema)
async def end_session(session_id: int, db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(BrewSession).where(BrewSession.id == session_id))
    session = result.scalar_one_or_none()
    if session is None:
        raise HTTPException(status_code=404, detail="Session not found")
    session.ended_at = datetime.now(timezone.utc)
    await db.commit()
    await db.refresh(session)
    return session


@app.patch("/sessions/{session_id}", response_model=BrewSessionSchema)
async def update_session(session_id: int, payload: BrewSessionSchema, db: AsyncSession = Depends(get_db)):
    result = await db.execute(select(BrewSession).where(BrewSession.id == session_id))
    session = result.scalar_one_or_none()
    if session is None:
        raise HTTPException(status_code=404, detail="Session not found")
    if payload.name is not None:
        session.name = payload.name
    if payload.recipe_id is not None:
        session.recipe_id = payload.recipe_id
    await db.commit()
    await db.refresh(session)
    return session


@app.get("/sessions/{session_id}/checklist", response_model=list[ChecklistEntrySchema])
async def get_checklist(session_id: int, db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(ChecklistEntry).where(ChecklistEntry.session_id == session_id)
    )
    return result.scalars().all()


@app.post("/sessions/{session_id}/checklist/{step}", response_model=ChecklistEntrySchema)
async def complete_checklist_step(session_id: int, step: str, db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(ChecklistEntry).where(
            ChecklistEntry.session_id == session_id,
            ChecklistEntry.step_name == step,
        )
    )
    entry = result.scalar_one_or_none()
    if entry is None:
        raise HTTPException(status_code=404, detail="Checklist step not found")
    entry.completed_at = datetime.now(timezone.utc)
    await db.commit()
    await db.refresh(entry)
    return entry


# ── Recipes ──────────────────────────────────────────────────────────────────

@app.get("/recipes", response_model=list[RecipeSchema])
async def list_recipes(db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(Recipe)
        .options(selectinload(Recipe.hop_additions), selectinload(Recipe.mash_steps))
        .order_by(desc(Recipe.created_at))
    )
    return result.scalars().all()


@app.get("/recipes/{recipe_id}", response_model=RecipeSchema)
async def get_recipe(recipe_id: int, db: AsyncSession = Depends(get_db)):
    result = await db.execute(
        select(Recipe)
        .options(selectinload(Recipe.hop_additions), selectinload(Recipe.mash_steps))
        .where(Recipe.id == recipe_id)
    )
    row = result.scalar_one_or_none()
    if row is None:
        raise HTTPException(status_code=404, detail="Recipe not found")
    return row


@app.post("/recipes", response_model=RecipeSchema, status_code=201)
async def create_recipe(payload: RecipeSchema, db: AsyncSession = Depends(get_db)):
    recipe = Recipe(**payload.model_dump(exclude={"id", "hop_additions", "mash_steps", "created_at"}))
    recipe.created_at = datetime.now(timezone.utc)
    db.add(recipe)
    await db.flush()
    for hop in payload.hop_additions:
        db.add(HopAddition(**hop.model_dump(exclude={"id"}), recipe_id=recipe.id))
    for step in payload.mash_steps:
        db.add(MashStep(**step.model_dump(exclude={"id"}), recipe_id=recipe.id))
    await db.commit()
    result = await db.execute(
        select(Recipe)
        .options(selectinload(Recipe.hop_additions), selectinload(Recipe.mash_steps))
        .where(Recipe.id == recipe.id)
    )
    return result.scalar_one()


@app.post("/recipes/import", response_model=RecipeSchema, status_code=201)
async def import_beerxml(file: UploadFile = File(...), db: AsyncSession = Depends(get_db)):
    """Parse a BeerXML file and store the recipe."""
    content = await file.read()
    try:
        data = xmltodict.parse(content)
        rec = data["RECIPES"]["RECIPE"]
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"Invalid BeerXML: {e}")

    def _f(val, default=0.0):
        try:
            return float(val) if val is not None else default
        except (TypeError, ValueError):
            return default

    recipe = Recipe(
        name=rec.get("NAME"),
        type=rec.get("TYPE"),
        brewer=rec.get("BREWER"),
        batch_size=_f(rec.get("BATCH_SIZE")),
        boil_size=_f(rec.get("BOIL_SIZE")),
        boil_time=_f(rec.get("BOIL_TIME")),
        efficiency=_f(rec.get("EFFICIENCY")),
        xml_content=content.decode("utf-8", errors="replace"),
        created_at=datetime.now(timezone.utc),
    )

    # Mash steps
    mash = rec.get("MASH", {})
    steps_raw = mash.get("MASH_STEPS", {}).get("MASH_STEP", [])
    if isinstance(steps_raw, dict):
        steps_raw = [steps_raw]
    db.add(recipe)
    await db.flush()

    for i, s in enumerate(steps_raw):
        db.add(MashStep(
            recipe_id=recipe.id,
            name=s.get("NAME"),
            temperature=_f(s.get("STEP_TEMP")),
            duration=_f(s.get("STEP_TIME")),
            step_number=i,
        ))

    # Hop additions
    hops_raw = rec.get("HOPS", {}).get("HOP", [])
    if isinstance(hops_raw, dict):
        hops_raw = [hops_raw]
    for h in hops_raw:
        db.add(HopAddition(
            recipe_id=recipe.id,
            name=h.get("NAME"),
            alpha=_f(h.get("ALPHA")),
            amount=_f(h.get("AMOUNT")),
            use=h.get("USE"),
            time=_f(h.get("TIME")),
        ))

    await db.commit()
    result = await db.execute(
        select(Recipe)
        .options(selectinload(Recipe.hop_additions), selectinload(Recipe.mash_steps))
        .where(Recipe.id == recipe.id)
    )
    return result.scalar_one()


@app.delete("/recipes/{recipe_id}", status_code=204)
async def delete_recipe(recipe_id: int, db: AsyncSession = Depends(get_db)):
    await db.execute(delete(Recipe).where(Recipe.id == recipe_id))
    await db.commit()


# ── Health ───────────────────────────────────────────────────────────────────

@app.get("/health")
async def health():
    return {"status": "ok", "service": "data-service"}

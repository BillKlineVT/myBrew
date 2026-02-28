from datetime import datetime
from typing import Optional
from sqlalchemy import Column, Integer, Float, Boolean, String, Text, ForeignKey, DateTime
from sqlalchemy.orm import DeclarativeBase, relationship
from pydantic import BaseModel


class Base(DeclarativeBase):
    pass


class SensorReading(Base):
    __tablename__ = "sensor_readings"

    id = Column(Integer, primary_key=True, autoincrement=True)
    timestamp = Column(DateTime(timezone=True), default=datetime.utcnow, nullable=False)
    hlt_temp = Column(Float)
    hlt_pwm = Column(Float)
    mash_temp = Column(Float)
    boil_temp = Column(Float)
    chill_temp = Column(Float)
    hlt_level = Column(Float)
    boil_level = Column(Float)


class GPIOState(Base):
    __tablename__ = "gpio_state"

    id = Column(Integer, primary_key=True, autoincrement=True)
    timestamp = Column(DateTime(timezone=True), default=datetime.utcnow, nullable=False)
    gpio_18 = Column(Boolean, default=False)
    gpio_22 = Column(Boolean, default=False)
    gpio_24 = Column(Boolean, default=False)
    gpio_27 = Column(Boolean, default=False)


class PIDSettings(Base):
    __tablename__ = "pid_settings"

    vessel = Column(String, primary_key=True)
    set_temp = Column(Float)
    p_val = Column(Float)
    i_val = Column(Float)
    d_val = Column(Float)
    enabled = Column(Boolean, default=False)
    updated_at = Column(DateTime(timezone=True), default=datetime.utcnow)


class BrewerySetting(Base):
    __tablename__ = "brewery_settings"

    key = Column(String, primary_key=True)
    value = Column(String)


class BrewSession(Base):
    __tablename__ = "brew_sessions"

    id = Column(Integer, primary_key=True, autoincrement=True)
    started_at = Column(DateTime(timezone=True), default=datetime.utcnow)
    ended_at = Column(DateTime(timezone=True), nullable=True)
    name = Column(String, nullable=True)
    recipe_id = Column(Integer, ForeignKey("recipes.id"), nullable=True)
    pre_boil_gravity = Column(String)
    mash_ph = Column(Float)
    ground_water_temp = Column(Float)

    checklist = relationship("ChecklistEntry", back_populates="session", cascade="all, delete-orphan")


class ChecklistEntry(Base):
    __tablename__ = "checklist_entries"

    id = Column(Integer, primary_key=True, autoincrement=True)
    session_id = Column(Integer, ForeignKey("brew_sessions.id", ondelete="CASCADE"))
    step_name = Column(String, nullable=False)
    completed_at = Column(DateTime(timezone=True))

    session = relationship("BrewSession", back_populates="checklist")


class Recipe(Base):
    __tablename__ = "recipes"

    id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String)
    type = Column(String)
    brewer = Column(String)
    batch_size = Column(Float)
    boil_size = Column(Float)
    boil_time = Column(Float)
    efficiency = Column(Float)
    grain_weight = Column(Float)
    strike_temp = Column(Float)
    pitch_temp = Column(Float)
    xml_content = Column(Text)
    created_at = Column(DateTime(timezone=True), default=datetime.utcnow)

    hop_additions = relationship("HopAddition", back_populates="recipe", cascade="all, delete-orphan")
    mash_steps = relationship("MashStep", back_populates="recipe", cascade="all, delete-orphan")


class HopAddition(Base):
    __tablename__ = "hop_additions"

    id = Column(Integer, primary_key=True, autoincrement=True)
    recipe_id = Column(Integer, ForeignKey("recipes.id", ondelete="CASCADE"))
    name = Column(String)
    alpha = Column(Float)
    amount = Column(Float)
    use = Column(String)
    time = Column(Float)

    recipe = relationship("Recipe", back_populates="hop_additions")


class MashStep(Base):
    __tablename__ = "mash_steps"

    id = Column(Integer, primary_key=True, autoincrement=True)
    recipe_id = Column(Integer, ForeignKey("recipes.id", ondelete="CASCADE"))
    name = Column(String)
    temperature = Column(Float)
    duration = Column(Float)
    step_number = Column(Integer)

    recipe = relationship("Recipe", back_populates="mash_steps")


# ── Pydantic schemas ─────────────────────────────────────────────────────────

class SensorReadingSchema(BaseModel):
    id: Optional[int] = None
    timestamp: Optional[datetime] = None
    hlt_temp: Optional[float] = None
    hlt_pwm: Optional[float] = None
    mash_temp: Optional[float] = None
    boil_temp: Optional[float] = None
    chill_temp: Optional[float] = None
    hlt_level: Optional[float] = None
    boil_level: Optional[float] = None

    class Config:
        from_attributes = True


class GPIOStateSchema(BaseModel):
    id: Optional[int] = None
    timestamp: Optional[datetime] = None
    gpio_18: bool = False
    gpio_22: bool = False
    gpio_24: bool = False
    gpio_27: bool = False

    class Config:
        from_attributes = True


class PIDSettingsSchema(BaseModel):
    vessel: str
    set_temp: Optional[float] = 0.0
    p_val: Optional[float] = 2.0
    i_val: Optional[float] = 0.0
    d_val: Optional[float] = 1.0
    enabled: bool = False

    class Config:
        from_attributes = True


class BrewerySettingSchema(BaseModel):
    key: str
    value: str

    class Config:
        from_attributes = True


class BrewSessionSchema(BaseModel):
    id: Optional[int] = None
    started_at: Optional[datetime] = None
    ended_at: Optional[datetime] = None
    name: Optional[str] = None
    recipe_id: Optional[int] = None
    pre_boil_gravity: Optional[str] = None
    mash_ph: Optional[float] = None
    ground_water_temp: Optional[float] = None

    class Config:
        from_attributes = True


class ChecklistEntrySchema(BaseModel):
    id: Optional[int] = None
    session_id: int
    step_name: str
    completed_at: Optional[datetime] = None

    class Config:
        from_attributes = True


class HopAdditionSchema(BaseModel):
    id: Optional[int] = None
    recipe_id: Optional[int] = None
    name: Optional[str] = None
    alpha: Optional[float] = None
    amount: Optional[float] = None
    use: Optional[str] = None
    time: Optional[float] = None

    class Config:
        from_attributes = True


class MashStepSchema(BaseModel):
    id: Optional[int] = None
    recipe_id: Optional[int] = None
    name: Optional[str] = None
    temperature: Optional[float] = None
    duration: Optional[float] = None
    step_number: Optional[int] = None

    class Config:
        from_attributes = True


class RecipeSchema(BaseModel):
    id: Optional[int] = None
    name: Optional[str] = None
    type: Optional[str] = None
    brewer: Optional[str] = None
    batch_size: Optional[float] = None
    boil_size: Optional[float] = None
    boil_time: Optional[float] = None
    efficiency: Optional[float] = None
    grain_weight: Optional[float] = None
    strike_temp: Optional[float] = None
    pitch_temp: Optional[float] = None
    xml_content: Optional[str] = None
    hop_additions: list[HopAdditionSchema] = []
    mash_steps: list[MashStepSchema] = []

    class Config:
        from_attributes = True

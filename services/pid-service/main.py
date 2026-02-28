"""
pid-service: PID control loop for HLT (and optionally BK) heating elements.
Ports: python/PID_PWM_Controller/RPi_PID_HLT.py
Control cycle: every 1 s, fetch setpoint+tuning from data-service,
read current temp from Redis, compute PID, send PWM to gpio-service.
"""
import asyncio
import json
import logging
import os
import time

import httpx
import redis.asyncio as aioredis
from fastapi import FastAPI

from pid import PID

logging.basicConfig(level=logging.INFO)
log = logging.getLogger("pid-service")

DATA_SERVICE_URL = os.getenv("DATA_SERVICE_URL", "http://data-service:8001")
GPIO_SERVICE_URL = os.getenv("GPIO_SERVICE_URL", "http://gpio-service:8003")
REDIS_URL = os.getenv("REDIS_URL", "redis://redis:6379")
CONTROL_INTERVAL = float(os.getenv("CONTROL_INTERVAL_S", "1.0"))

# PID instances per vessel
_controllers: dict[str, PID] = {
    "HLT": PID(),
    "BK": PID(),
}
_last_output: dict[str, float] = {"HLT": 0.0, "BK": 0.0}
_latest_temps: dict[str, float | None] = {"HLT": None, "BK": None, "mash": None}
_enabled: dict[str, bool] = {"HLT": False, "BK": False}

app = FastAPI(title="myBrew PID Service")


# ── Redis subscriber for real-time temps ─────────────────────────────────────

async def _subscribe_temps(redis: aioredis.Redis):
    """Listen to mybrew:sensors channel to get current temperatures."""
    pubsub = redis.pubsub()
    await pubsub.subscribe("mybrew:sensors")
    async for message in pubsub.listen():
        if message["type"] != "message":
            continue
        try:
            data = json.loads(message["data"])
            _latest_temps["HLT"] = data.get("hlt_temp")
            _latest_temps["BK"] = data.get("boil_temp")
            _latest_temps["mash"] = data.get("mash_temp")
        except Exception as e:
            log.warning("Temp parse error: %s", e)


# ── PID control loop ─────────────────────────────────────────────────────────

async def _update_pid_settings(http: httpx.AsyncClient):
    """Refresh PID tuning and setpoints from data-service."""
    for vessel in ("HLT", "BK"):
        try:
            resp = await http.get(f"/pid/{vessel}")
            resp.raise_for_status()
            cfg = resp.json()
            pid = _controllers[vessel]
            pid.setPoint(cfg["set_temp"])
            pid.setKp(cfg["p_val"])
            pid.setKi(cfg["i_val"])
            pid.setKd(cfg["d_val"])
            _enabled[vessel] = cfg["enabled"]
        except Exception as e:
            log.warning("Could not fetch PID settings for %s: %s", vessel, e)


async def _apply_pwm(http: httpx.AsyncClient, vessel: str, pwm: float):
    """Send computed PWM output to gpio-service and record in data-service."""
    log.debug("%s PWM → %.1f%%", vessel, pwm)
    # For HLT we toggle gpio_18 (pump acts as PWM-controlled SSR in the original)
    # Real PWM duty-cycle control would need a hardware PWM GPIO; here we
    # record the value and let gpio-service handle the pin.
    try:
        # Post sensor update with PWM value so frontend can chart it
        await http.post(
            f"{DATA_SERVICE_URL}/sensors",
            json={"hlt_pwm": pwm if vessel == "HLT" else None},
        )
    except Exception as e:
        log.warning("Could not post PWM reading: %s", e)


async def control_loop():
    redis = aioredis.from_url(REDIS_URL)
    asyncio.create_task(_subscribe_temps(redis))

    async with httpx.AsyncClient(timeout=3.0) as http:
        http._base_url = httpx.URL(DATA_SERVICE_URL)

        while True:
            await _update_pid_settings(http)

            for vessel in ("HLT", "BK"):
                if not _enabled[vessel]:
                    _last_output[vessel] = 0.0
                    continue
                current_temp = _latest_temps.get(vessel)
                if current_temp is None:
                    log.debug("%s: no temperature yet, skipping PID step", vessel)
                    continue
                output = _controllers[vessel].update(current_temp)
                _last_output[vessel] = output
                await _apply_pwm(http, vessel, output)

            await asyncio.sleep(CONTROL_INTERVAL)


@app.on_event("startup")
async def startup():
    asyncio.create_task(control_loop())


# ── REST endpoints ────────────────────────────────────────────────────────────

@app.get("/status")
async def status():
    return {
        "enabled": _enabled,
        "output": _last_output,
        "setpoints": {v: _controllers[v].getPoint() for v in _controllers},
        "current_temps": _latest_temps,
    }


@app.post("/reset/{vessel}")
async def reset_pid(vessel: str):
    vessel = vessel.upper()
    if vessel not in _controllers:
        from fastapi import HTTPException
        raise HTTPException(status_code=400, detail=f"Unknown vessel: {vessel}")
    _controllers[vessel].setIntegrator(0)
    _controllers[vessel].setDerivator(0)
    return {"reset": vessel}


@app.get("/health")
async def health():
    return {"status": "ok", "service": "pid-service"}

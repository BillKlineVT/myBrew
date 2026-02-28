"""
sensor-service: Reads 1-wire temperature sensors and capacitive level sensor.
Publishes readings to Redis channel 'mybrew:sensors' and POSTs to data-service.
Ports: python/temp_sensor_poll.py + python/CapSensor_Listener_corrected.py
"""
import asyncio
import json
import logging
import os
import time
from pathlib import Path

import httpx
import redis.asyncio as aioredis
from fastapi import FastAPI

logging.basicConfig(level=logging.INFO)
log = logging.getLogger("sensor-service")

MOCK_HARDWARE = os.getenv("MOCK_HARDWARE", "false").lower() == "true"
DATA_SERVICE_URL = os.getenv("DATA_SERVICE_URL", "http://data-service:8001")
REDIS_URL = os.getenv("REDIS_URL", "redis://redis:6379")
POLL_INTERVAL = float(os.getenv("POLL_INTERVAL_S", "1.0"))
SERIAL_PORT = os.getenv("SERIAL_PORT", "/dev/ttyAMA0")

# 1-wire sensor device IDs (override via env for different hardware)
W1_BASE = Path(os.getenv("W1_BASE_DIR", "/sys/bus/w1/devices"))
HLT_SENSOR_ID  = os.getenv("HLT_SENSOR_ID",  "28-000004cd6f22")
MASH_SENSOR_ID = os.getenv("MASH_SENSOR_ID", "28-000004cd6f23")
BOIL_SENSOR_ID = os.getenv("BOIL_SENSOR_ID", "28-000004ce4807")
CHILL_SENSOR_ID = os.getenv("CHILL_SENSOR_ID", "28-000004cea439")

# Level sensor calibration constants (from CapSensor_Listener_corrected.py)
LEVEL_MIN_EMPTY = 1225.0
LEVEL_MAX_50F = 3190.0
VOLUME_ARRAY = [
    0.0, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0,
    5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0,
    10.5, 11.0, 11.5, 12.0, 12.5, 13.0, 13.5, 14.0,
]
LEVEL_60F = [
    1225, 1310, 1400, 1485, 1550, 1575, 1660, 1750, 1835, 1925,
    2010, 2100, 2175, 2220, 2275, 2360, 2405, 2495, 2580, 2640,
    2710, 2780, 2840, 2930, 3000, 3060, 3105,
]
LEVEL_83F = [
    1225, 1310, 1400, 1485, 1575, 1660, 1750, 1835, 1925, 2010,
    2100, 2185, 2275, 2360, 2445, 2495, 2603, 2680, 2755, 2840,
    2875, 2930, 3015, 3185, 3250, 3280, 3370,
]

# latest readings cache
_latest: dict = {}

app = FastAPI(title="myBrew Sensor Service")


# ── 1-wire temperature reading ───────────────────────────────────────────────

def _read_w1_temp_f(sensor_id: str) -> float | None:
    """Read a DS18B20 sensor; return Fahrenheit or None on error."""
    device_file = W1_BASE / sensor_id / "w1_slave"
    try:
        text = device_file.read_text()
    except OSError:
        return None
    lines = text.splitlines()
    if len(lines) < 2 or "YES" not in lines[0]:
        return None
    pos = lines[1].find("t=")
    if pos == -1:
        return None
    temp_c = float(lines[1][pos + 2:]) / 1000.0
    return temp_c * 9.0 / 5.0 + 32.0


def _mock_temp(base: float) -> float:
    import random
    return base + random.uniform(-0.5, 0.5)


def read_temperatures() -> dict[str, float | None]:
    if MOCK_HARDWARE:
        return {
            "hlt_temp":   _mock_temp(155.0),
            "mash_temp":  _mock_temp(152.0),
            "boil_temp":  _mock_temp(212.0),
            "chill_temp": _mock_temp(65.0),
        }
    return {
        "hlt_temp":   _read_w1_temp_f(HLT_SENSOR_ID),
        "mash_temp":  _read_w1_temp_f(MASH_SENSOR_ID),
        "boil_temp":  _read_w1_temp_f(BOIL_SENSOR_ID),
        "chill_temp": _read_w1_temp_f(CHILL_SENSOR_ID),
    }


# ── Level sensor ─────────────────────────────────────────────────────────────

def _level_from_lookup(raw: float, temp_f: float) -> float:
    """
    Temperature-compensated level lookup.
    Ported from CapSensor_Listener_corrected.py.
    Returns gallons (0–14).
    """
    if temp_f <= 71.5:
        arr = LEVEL_60F
    elif temp_f <= 95.0:
        scale = (temp_f - 60.0) / (83.0 - 60.0)
        arr = [int((1 - scale) * a + scale * b) for a, b in zip(LEVEL_60F, LEVEL_83F)]
    else:
        arr = LEVEL_83F  # simplification; extend with more lookup tables as needed

    for i, threshold in enumerate(arr):
        if raw <= threshold:
            return VOLUME_ARRAY[i] if i < len(VOLUME_ARRAY) else VOLUME_ARRAY[-1]
    return VOLUME_ARRAY[-1]


async def _read_level_sensor() -> float | None:
    """Read capacitive level sensor over serial. Returns gallons or None."""
    if MOCK_HARDWARE:
        import random
        return round(random.uniform(5.0, 12.0), 2)
    try:
        import serial  # pyserial
        port = serial.Serial(SERIAL_PORT, baudrate=9600, timeout=3.0)
        raw_bytes = port.read(5)
        port.close()
        if not raw_bytes:
            return None
        raw_val = float(raw_bytes.decode("ascii", errors="ignore").strip())
        # Use mash temp as reference temperature (if available)
        ref_temp = _latest.get("mash_temp") or 60.0
        return _level_from_lookup(raw_val, ref_temp)
    except Exception as e:
        log.warning("Level sensor read error: %s", e)
        return None


# ── Main polling loop ────────────────────────────────────────────────────────

async def poll_loop():
    redis = aioredis.from_url(REDIS_URL)
    async with httpx.AsyncClient(base_url=DATA_SERVICE_URL, timeout=5.0) as http:
        while True:
            try:
                temps = read_temperatures()
                hlt_level = await _read_level_sensor()

                reading = {
                    **temps,
                    "hlt_level": hlt_level,
                    "hlt_pwm": None,  # filled by pid-service
                }
                _latest.update(reading)

                # POST to data-service
                await http.post("/sensors", json=reading)

                # Publish to Redis
                await redis.publish("mybrew:sensors", json.dumps(reading))
                log.debug("Published sensor reading: %s", reading)

            except Exception as e:
                log.error("Poll error: %s", e)

            await asyncio.sleep(POLL_INTERVAL)


@app.on_event("startup")
async def startup():
    asyncio.create_task(poll_loop())


# ── REST endpoints ───────────────────────────────────────────────────────────

@app.get("/sensors/latest")
async def get_latest():
    return _latest or {"error": "no readings yet"}


@app.get("/health")
async def health():
    return {"status": "ok", "service": "sensor-service", "mock": MOCK_HARDWARE}

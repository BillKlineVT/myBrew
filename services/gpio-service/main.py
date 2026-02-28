"""
gpio-service: Receives GPIO commands via REST and actuates RPi GPIO pins.
Ports: python/RPi_GPIO_DB_Listener.py
GPIO mapping (BCM): 18=pump, 22=stir motor, 24=HLT fill valve, 27=reserved
"""
import logging
import os

from fastapi import FastAPI
from pydantic import BaseModel

logging.basicConfig(level=logging.INFO)
log = logging.getLogger("gpio-service")

MOCK_HARDWARE = os.getenv("MOCK_HARDWARE", "false").lower() == "true"

# GPIO pin assignments (BCM numbering)
GPIO_PINS = {
    "gpio_18": 18,  # pump relay
    "gpio_22": 22,  # stir motor relay
    "gpio_24": 24,  # HLT fill valve relay
    "gpio_27": 27,  # reserved
}

# Track current state in memory
_state: dict[str, bool] = {k: False for k in GPIO_PINS}

app = FastAPI(title="myBrew GPIO Service")


def _setup_gpio():
    if MOCK_HARDWARE:
        log.info("MOCK_HARDWARE=true — GPIO not initialised")
        return
    try:
        import RPi.GPIO as GPIO
        GPIO.setmode(GPIO.BCM)
        for pin in GPIO_PINS.values():
            GPIO.setup(pin, GPIO.OUT)
            GPIO.output(pin, GPIO.LOW)
        log.info("GPIO pins initialised: %s", list(GPIO_PINS.values()))
        app.state.gpio = GPIO
    except Exception as e:
        log.error("GPIO setup failed: %s — falling back to mock", e)
        app.state.gpio = None


def _set_pin(gpio_key: str, value: bool):
    pin = GPIO_PINS[gpio_key]
    if MOCK_HARDWARE or getattr(app.state, "gpio", None) is None:
        log.info("[MOCK] GPIO %d → %s", pin, "HIGH" if value else "LOW")
        return
    app.state.gpio.output(pin, app.state.gpio.HIGH if value else app.state.gpio.LOW)
    log.info("GPIO %d → %s", pin, "HIGH" if value else "LOW")


@app.on_event("startup")
async def startup():
    _setup_gpio()


@app.on_event("shutdown")
async def shutdown():
    gpio = getattr(app.state, "gpio", None)
    if gpio:
        gpio.cleanup()


# ── Pydantic models ───────────────────────────────────────────────────────────

class GPIOCommand(BaseModel):
    gpio_18: bool = False
    gpio_22: bool = False
    gpio_24: bool = False
    gpio_27: bool = False


# ── Endpoints ────────────────────────────────────────────────────────────────

@app.get("/gpio")
async def get_gpio():
    return _state


@app.put("/gpio")
async def set_gpio(cmd: GPIOCommand):
    for key, value in cmd.model_dump().items():
        if _state[key] != value:
            _set_pin(key, value)
            _state[key] = value
    return _state


@app.put("/gpio/{pin_name}")
async def set_single_gpio(pin_name: str, value: bool):
    if pin_name not in GPIO_PINS:
        from fastapi import HTTPException
        raise HTTPException(status_code=400, detail=f"Unknown pin: {pin_name}")
    _set_pin(pin_name, value)
    _state[pin_name] = value
    return _state


@app.get("/health")
async def health():
    return {"status": "ok", "service": "gpio-service", "mock": MOCK_HARDWARE, "state": _state}

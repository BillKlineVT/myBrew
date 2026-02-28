# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

myBrew is a Qt5-based beer brewing controller and monitoring application designed to run on a Raspberry Pi. It controls heating elements via GPIO/SSR, reads temperature and level sensors, implements PID/PWM control, and provides a touchscreen-friendly UI.

## Build Commands

```bash
# Generate Makefiles and build everything (lib + app + tests)
qmake -makefile
make clean
make

# Build only the library
cd myBrewLib && qmake -makefile && make

# Build only the app
cd myBrewApp && qmake -makefile && make

# Run unit tests (requires /DB directory and LD_LIBRARY_PATH set)
mkdir -p /DB
export LD_LIBRARY_PATH=./myBrewLib
./myBrewTests/myBrewTests

# Run unit tests with XML output (as CI does)
xvfb-run ./myBrewTests/myBrewTests -o test_output.xml,xml

# Static analysis
cppcheck --enable=all --inconclusive --xml --xml-version=2 . 2> cppcheck.xml

# GUI tests (SikuliX / Maven-based, requires running app on display :1)
cd myBrewTests/GUITest/GUITest && mvn clean package
```

## Architecture

The project uses a **subdirectory qmake structure** (`myBrewProject.pro`) with three components:

### `myBrewLib/` — Shared Library (core of the application)
All UI logic, business logic, and sensor/PID threading lives here. Built as a shared lib (`libmyBrewLib.so`).

- **`src/myBrew.h/.cpp`** — Main `QMainWindow` subclass. Owns all UI tabs (HLT, MLT, BK, Chill, Clean, Checklist, Recipe, Timers, Logs, Settings). Manages QTimers for brew step timers, displays sensor data received from `SensorWorkerThread` via Qt signals.
- **`src/SensorWorkerThread.h/.cpp`** — `QThread` that polls the SQLite database (`/DB/brewing_db1.db`) for new sensor readings (temperature, PWM, liquid level) and emits `SensorWorker_READ_COMPLETE` signal to update the UI.
- **`src/PIDThread.h/.cpp`** — `QThread` for PID control (currently commented out in mainwindow; PID is delegated to Python scripts).
- **`src/beerxmlrecipe.h/.cpp`** — Parses BeerXML recipe files using pugixml. Populates hop additions, mash steps, fermentables.
- **`src/brewerysettings.h/.cpp`** — Plain data class holding all configurable brewery parameters (PID tuning, temp calibrations, brew constants).
- **`src/*popup.h/.cpp`** — Modal dialogs for settings (brew constants, PID settings, temp calibration, audio alarm, step duration).
- **`include/openbrew/`** — OpenBrew GPL library headers/sources: `Vessel`, `HLT`, `MLT`, `Kettle`, `Step`, `Hop`, `Fermentable`, `defines.h`.
- **`include/pugixml/`** — pugixml XML parser (bundled).
- **`include/qcustomplot/`** — QCustomPlot chart widget (bundled) for temperature/PWM trend plots.

### `myBrewApp/` — Application Shell
Just `main.cpp` — creates `QApplication` and instantiates `myBrew`. Links against `myBrewLib`.

### `myBrewTests/` — Qt Unit Tests
Uses Qt Test framework. Tests are friend-class black-box tests of `myBrew` (accesses private members via `friend class myBrewTests`). Tests cover timer start/stop and checklist initialization.

GUI tests in `myBrewTests/GUITest/GUITest/` use SikuliX image-recognition via Maven/Java.

### `python/` — Raspberry Pi Backend Scripts (Python 2)
These run as separate processes on the RPi and communicate with the Qt app via the shared SQLite database at `/DB/brewing_db1.db`.

- **`RPi_GPIO_DB_Listener.py`** — Polls `gpio_outputs` table and sets RPi GPIO pins (18, 22, 24, 27) for pump relay, stir motor, HLT fill valve.
- **`PID_PWM_Controller/RPi_PID_HLT.py`** — Reads PID setpoint/tuning from DB, runs PID algorithm, outputs PWM to heating element GPIO.
- **`temp_sensor_poll.py`** — Reads 1-wire temperature sensors and writes to DB.
- **`CapSensor_Listener.py`** / **`PressureSensor_Listener/`** — Reads capacitive/pressure liquid level sensors and writes to DB.

## Key Architectural Pattern: SQLite as IPC

The Qt GUI and Python scripts are decoupled via a shared SQLite database at `/DB/brewing_db1.db`. The Qt app writes desired GPIO states; Python scripts poll and actuate. Sensor scripts write readings; `SensorWorkerThread` polls and updates the UI. This enables datalogging and multi-process access.

## GPIO Mapping (RPi BCM numbering)
- GPIO 18 — pump relay
- GPIO 22 — stir motor relay
- GPIO 24 — HLT fill valve relay
- GPIO 27 — (reserved)

## CI/CD

Jenkins pipeline (`Jenkinsfile`) stages: build → GUI test build → package → static-analysis (cppcheck) → unit-tests → deliver to Artifactory → build/push Docker image (`billklinefelter/mybrew_x86_64`) → execute SikuliX GUI tests on remote device.

Docker base image for Jenkins agent: `billklinefelter/jenkins-qt5`.

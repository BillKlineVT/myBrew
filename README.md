# myBrew
Beer Brewing Controller and Monitoring — Qt5 desktop app + Kubernetes microservices web app

## UI
<p align="center"><img src="https://github.com/BillKlineVT/myBrew/blob/master/doc/myBrew_screenshots/myBrew%20UI%20screenshot.png?raw=true" width="800" align="middle"/></p>
- The main user interface is intended to be touchscreen friendly, but needs some more work to evaluate button sizes
- There are tabs for displays relevant to each step of the brewing process.

## File Structure
- Base directory of repo includes source code and project files for the QT IDE-based GUI for myBrew
- Subdirectories include helper-function Python apps for database interactions, Raspberry Pi GPIO interaction, PID & PWM Controller libraries
- The QT GUI and Python-based processing apps are linked via an sqlite database to allow for multi-service data access which also enables datalogging for post-brew session data review.
 
## Hardware
- System is designed to run on a Raspberry Pi, should support any version after Rasbperry Pi 1 B
- GPIO control of heating elements is wired to 40A-capable Solid State Relays which are wired to 120VAC residential power circuits.
- Pulse Width Modulation (PWM) is done using Raspberry Pi PWM library.  This enables gradual heating needed to implement PID algorithm.

<p align="center">
<img src="https://github.com/BillKlineVT/myBrew/blob/master/doc/myBrew%20Toolbox.JPG?raw=true" width="400"/>  <img src="https://github.com/BillKlineVT/myBrew/blob/master/doc/myBrew%20Toolbox%20internal.JPG?raw=true" width="400"/>
<img src="https://github.com/BillKlineVT/myBrew/blob/master/doc/myBrew%20v1%20schematic%20(2x%20120VAC%20circuits).JPG?raw=true" alt="" width="800"/>
</p>

---

## Web App (Microservices / Kubernetes)

The `services/` directory contains a modern rewrite as a microservices web app.

### Architecture

| Service | Port | Description |
|---|---|---|
| `frontend` | 80 | React/TypeScript SPA (Vite + Recharts + React Query) |
| `api-gateway` | 8000 | FastAPI reverse proxy + WebSocket hub (Redis fan-out) |
| `data-service` | 8001 | FastAPI CRUD API backed by PostgreSQL |
| `sensor-service` | 8002 | Polls 1-wire temps + capacitive level sensor → Redis + DB |
| `gpio-service` | 8003 | REST interface to RPi GPIO relay pins |
| `pid-service` | 8004 | PID control loop — reads Redis temps, outputs PWM |
| `postgres` | 5432 | Persistent storage (replaces SQLite) |
| `redis` | 6379 | Pub/sub channel `mybrew:sensors` for real-time data |

### Local Development (Docker Compose)

```bash
# Start all services with mock hardware (no RPi needed)
docker compose up --build

# Frontend: http://localhost:3000
# API:      http://localhost:8000
```

### Kubernetes / Helm

```bash
# Add Bitnami repo for postgresql + redis subcharts
helm repo add bitnami https://charts.bitnami.com/bitnami
helm dependency update ./helm/mybrew

# Install (mock hardware mode — no RPi GPIO required)
helm install mybrew ./helm/mybrew --namespace mybrew --create-namespace

# Install for real RPi hardware
helm install mybrew ./helm/mybrew \
  --namespace mybrew --create-namespace \
  --set sensorService.mockHardware=false \
  --set gpioService.mockHardware=false

# Upgrade / change values
helm upgrade mybrew ./helm/mybrew -f my-values.yaml

# Port-forward if no Ingress controller
kubectl port-forward -n mybrew svc/mybrew-frontend 3000:80
```

The Ingress default hostname is `mybrew.local` (edit `values.yaml` → `ingress.hosts`).
Add `127.0.0.1 mybrew.local` to `/etc/hosts` for local testing with an Ingress controller.

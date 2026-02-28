"""
api-gateway: FastAPI reverse proxy + WebSocket hub.
- Proxies REST calls to backend services.
- WebSocket /ws: subscribes to Redis 'mybrew:sensors', broadcasts to all clients.
"""
import asyncio
import json
import logging
import os
from typing import Any

import httpx
import redis.asyncio as aioredis
from fastapi import FastAPI, WebSocket, WebSocketDisconnect, Request, HTTPException
from fastapi.responses import Response

logging.basicConfig(level=logging.INFO)
log = logging.getLogger("api-gateway")

DATA_SERVICE_URL  = os.getenv("DATA_SERVICE_URL",  "http://data-service:8001")
GPIO_SERVICE_URL  = os.getenv("GPIO_SERVICE_URL",  "http://gpio-service:8003")
PID_SERVICE_URL   = os.getenv("PID_SERVICE_URL",   "http://pid-service:8004")
REDIS_URL         = os.getenv("REDIS_URL",          "redis://redis:6379")

app = FastAPI(title="myBrew API Gateway")

# Connected WebSocket clients
_ws_clients: set[WebSocket] = set()


# ── WebSocket hub ─────────────────────────────────────────────────────────────

async def _redis_subscriber():
    """Background task: subscribe to Redis and fan-out to WS clients."""
    redis = aioredis.from_url(REDIS_URL)
    pubsub = redis.pubsub()
    await pubsub.subscribe("mybrew:sensors")
    log.info("Subscribed to mybrew:sensors")
    async for message in pubsub.listen():
        if message["type"] != "message":
            continue
        payload = message["data"]
        if isinstance(payload, bytes):
            payload = payload.decode()
        dead = set()
        for ws in _ws_clients:
            try:
                await ws.send_text(payload)
            except Exception:
                dead.add(ws)
        _ws_clients -= dead


@app.on_event("startup")
async def startup():
    asyncio.create_task(_redis_subscriber())


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    _ws_clients.add(websocket)
    log.info("WS client connected (%d total)", len(_ws_clients))
    try:
        while True:
            # Keep connection alive; clients send pings or we just wait
            await websocket.receive_text()
    except WebSocketDisconnect:
        pass
    finally:
        _ws_clients.discard(websocket)
        log.info("WS client disconnected (%d total)", len(_ws_clients))


# ── HTTP proxy helper ─────────────────────────────────────────────────────────

async def _proxy(request: Request, base_url: str, path: str) -> Response:
    async with httpx.AsyncClient(base_url=base_url, timeout=10.0) as client:
        body = await request.body()
        headers = {
            k: v for k, v in request.headers.items()
            if k.lower() not in ("host", "content-length")
        }
        try:
            resp = await client.request(
                method=request.method,
                url=path,
                params=dict(request.query_params),
                content=body,
                headers=headers,
            )
            return Response(
                content=resp.content,
                status_code=resp.status_code,
                headers=dict(resp.headers),
                media_type=resp.headers.get("content-type"),
            )
        except httpx.ConnectError as e:
            raise HTTPException(status_code=503, detail=f"Upstream unavailable: {e}")


# ── Route table ───────────────────────────────────────────────────────────────

# data-service routes
DATA_PREFIXES = ("/sensors", "/settings", "/sessions", "/recipes")

@app.api_route("/api{path:path}", methods=["GET", "POST", "PUT", "DELETE", "PATCH"])
async def gateway_route(path: str, request: Request):
    # GPIO routes → gpio-service
    if path.startswith("/gpio"):
        return await _proxy(request, GPIO_SERVICE_URL, path)

    # PID routes → pid-service
    if path.startswith("/pid") or path.startswith("/pid-status"):
        if path == "/pid-status":
            return await _proxy(request, PID_SERVICE_URL, "/status")
        return await _proxy(request, DATA_SERVICE_URL, path)

    # Everything else → data-service
    return await _proxy(request, DATA_SERVICE_URL, path)


# ── Health ────────────────────────────────────────────────────────────────────

@app.get("/health")
async def health():
    return {
        "status": "ok",
        "service": "api-gateway",
        "ws_clients": len(_ws_clients),
    }

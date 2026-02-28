FROM python:3.12-slim

WORKDIR /app

RUN apt-get update && apt-get install -y --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 8002

# Run with host networking so 1-wire and serial are reachable
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8002"]

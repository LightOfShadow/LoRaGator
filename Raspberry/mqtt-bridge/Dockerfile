FROM python:3.10-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY mqtt_bridge.py .
COPY config.ini .

CMD ["python", "mqtt_bridge.py"]

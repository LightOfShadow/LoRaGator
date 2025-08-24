import configparser
import paho.mqtt.client as mqtt
import json

# Load config
config = configparser.ConfigParser()
config.read("config.ini")

print(config)

SOURCE_BROKER = "mosquitto"
SOURCE_PORT = 1883

TARGET_BROKER = config["target_broker"]["host"]
TARGET_PORT = int(config["target_broker"]["port"])
TARGET_TOPIC = config["target_broker"]["topic"]

source_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
target_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)


# Connect target broker
target_client.connect(TARGET_BROKER, TARGET_PORT)
target_client.loop_start()

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Connected successfully to source broker.")
        client.subscribe("#")  # or your specific topic
    else:
        print("Failed to connect, reason:", reason_code)

def on_message(client, userdata, msg):
    try:
        payload_str = msg.payload.decode("utf-8")
        data = json.loads(payload_str)  # Try parsing JSON
        print(f"[FORWARDING] {msg.topic}: {data}")

        # Forward to another broker
        target_client.publish(TARGET_TOPIC, json.dumps(data))

    except (UnicodeDecodeError, json.JSONDecodeError):
        print(f"[SKIPPED - not JSON]: {msg.topic}")
        # Optionally: log or store the raw binary

# Set up callbacks
source_client.on_connect = on_connect
source_client.on_message = on_message

# Connect to source and start loop
source_client.connect(SOURCE_BROKER, SOURCE_PORT)
source_client.loop_forever()

import paho.mqtt.client as mqtt
import json
from datetime import datetime

def on_connect(client, userdata, flags, rc):
    print(f"[{datetime.now().strftime('%H:%M:%S')}] Connected to broker (code {rc})")
    client.subscribe("smarthome/+/alerts")
    print("Subscribed to: smarthome/+/alerts")
    print("Waiting for alerts from all devices...")

def on_message(client, userdata, msg):
    timestamp = datetime.now().strftime('%H:%M:%S')
    try:
        payload = json.loads(msg.payload.decode())
        device  = msg.topic.split("/")[1]
        cls     = payload.get("class", "Unknown")
        expl    = payload.get("explanation", "N/A")
        print(f"[{timestamp}] {device:<12} | {cls:<16} | {expl}")
    except Exception:
        print(f"[{timestamp}] Raw: {msg.payload.decode()}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print("CB011911 TinyML-IDS — Cloud Alert Subscriber")
print("=" * 55)
import os
USERNAME = os.environ.get("MQTT_USER", "idsuser")
PASSWORD = os.environ.get("MQTT_PASS", "ids2026secure")
client.username_pw_set(USERNAME, PASSWORD)
client.connect("localhost", 1883, 60)
client.loop_forever()
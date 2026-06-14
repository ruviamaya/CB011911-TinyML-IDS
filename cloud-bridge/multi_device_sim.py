import paho.mqtt.client as mqtt
import json
import time
import random

# Pre-explained alert payloads matching the firmware output
ALERTS = [
    {
        "class": "DDoS",
        "explanation": "DDoS detected: Number is above normal range, consistent with volumetric flooding."
    },
    {
        "class": "Reconnaissance",
        "explanation": "Probe detected: TCP is above normal range, consistent with network reconnaissance."
    },
    {
        "class": "Benign",
        "explanation": "No threat detected: traffic features are within normal operating ranges."
    },
]

DEVICES = ["device_001", "device_002", "device_003"]
NUM_CYCLES = 5

def run_simulation():
    client = mqtt.Client()
    client.connect("localhost", 1883, 60)
    client.loop_start()

    print("CB011911 TinyML-IDS — Multi-Device Fleet Simulation")
    print("=" * 55)
    print(f"Simulating {len(DEVICES)} edge devices, {NUM_CYCLES} cycles each")

    try:
        for cycle in range(1, NUM_CYCLES + 1):
            print(f"--- Cycle {cycle}/{NUM_CYCLES} ---")
            for device in DEVICES:
                alert   = random.choice(ALERTS)
                topic   = f"smarthome/{device}/alerts"
                payload = json.dumps(alert)
                client.publish(topic, payload)
                print(f"  Published: {device} -> {alert['class']}")
                time.sleep(0.5)
            print()
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()
        client.disconnect()
        print("Simulation complete.")

if __name__ == "__main__":
    run_simulation()
"""
This script sends JSON data via MQTT using the paho-mqtt client. 
It connects to the broker at "192.168.1.11" on port 1883.

Functions:
- on_connect(client, userdata, flags, rc): Connects to the MQTT broker
"""
import sys
import os
import time
import json
import glob
import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, rc):
    """
    The callback for when the client receives a CONNACK response from the server.

    Parameters:
    client (mqtt.Client): The client instance for this callback
    userdata: The private user data as set in Client() or userdata_set()
    flags (dict): Response flags sent by the broker
    rc (int): The connection result

    Returns:
    None
    """
    if rc == 0:
        print("Connected successfully.")
    else:
        print(f"Connection failed with code {rc}.")
        sys.exit()


client = mqtt.Client()
client.on_connect = on_connect

# Connect to the MQTT broker and error handling
try:
    client.connect("docker.local", 1883, 60)

except ConnectionRefusedError:
    print("Connection refused. Please check your broker settings.")
    sys.exit(1)
except ConnectionError:
    print("Connection failed. Please check your network connection.")
    sys.exit(1)
except TimeoutError:
    print("Connection timed out. Please check your broker settings or network connection.")
    sys.exit(1)
except OSError as e:
    print(f"OS error occurred: {e}")
    sys.exit(1)
except Exception as e:
    print(f"Unexpected error occurred: {e}")
    sys.exit(1)

# Start the MQTT loop
client.loop_start()

# Check if a filename was passed as a command-line argument
if len(sys.argv) > 1:
    # A filename was provided, send only that file
    filenames = sys.argv[1:]
else:
    # No filename was provided, send all .json files in the file where this script is located

    script_dir = os.path.dirname(os.path.realpath(__file__))
    filenames = glob.glob(os.path.join(script_dir, '*.json'))

# Loop over the list of filenames
for filename in filenames:
    # Load the JSON data from the file
    with open(filename, encoding='utf-8') as f:
        data = json.load(f)

    # Extract the topic and payload from the data
    topic = data['topic']
    payload = data['payload']

    # Publish the payload to the topic
    client.publish(topic, json.dumps(payload), qos=1, retain=True)
    print(f"Published {filename} to {topic}")
    time.sleep(1)  # add a small delay to ensure the message gets sent

client.loop_stop()  # stop the loop

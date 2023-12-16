"""
This module provides functionality for sending JSON data using MQTT.
"""
import json
import time
import sys  # import the sys module
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
    print("Connected with result code "+str(rc))


client = mqtt.Client()
client.on_connect = on_connect

client.connect("192.168.1.11", 1883, 60)

# Check if a filename was passed as a command-line argument
if len(sys.argv) != 2:
    print("Usage: python send_json.py <filename>")
    sys.exit(1)

# Load the JSON data from the file passed as a command-line argument
with open(sys.argv[1], encoding='utf-8') as f:
    data = json.load(f)

# Extract the topic and payload from the data
topic = data['topic']
payload = data['payload']

client.loop_start()  # start the loop
client.publish(topic, json.dumps(payload), qos=1)
time.sleep(1)  # add a small delay to ensure the message gets sent
client.loop_stop()  # stop the loop

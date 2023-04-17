import paho.mqtt.client as mqtt

# Define constants for topic names
SYSTEM_STATE_TOPIC = "SystemState"
LED_STATE1_TOPIC = "LedState1"
LED_STATE0_TOPIC = "LedState0"
BUTTON_PRESS_COUNT_TOPIC = "ButtonPressCount"

# Define callback functions
def on_connect(client, userdata, flags, rc):
    """
    Callback function called when the client connects to the broker.
    """
    print("Connected")
    client.subscribe(SYSTEM_STATE_TOPIC)
    client.subscribe(LED_STATE1_TOPIC)
    client.subscribe(LED_STATE0_TOPIC)
    client.subscribe(BUTTON_PRESS_COUNT_TOPIC)

def on_message(client, userdata, msg):
    """
    Callback function called when a message is received.
    """
    payload_str = msg.payload.decode('utf-8')  # decode the byte string to regular string
    print("Topic: " + msg.topic + " Message: " + payload_str)

# Set up MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to broker
host = "localhost"
port = 1883
timeout = 60

try:
    client.connect(host, port, timeout)
except ConnectionRefusedError:
    print(f"Failed to connect to broker at {host}:{port}")
    exit(1)

# Start the loop
try:
    client.loop_forever()
except KeyboardInterrupt:
    print("Interrupted")
finally:
    client.disconnect()


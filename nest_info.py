import time
import smbus
import paho.mqtt.client as mqtt
import automationhat

broker = "192.168.1.140"
broker_port = 1883
topic = "/nest/heat" # For home assistant looking
direct_control_topic = "/stove/cmd"
client = mqtt.Client()
#client.on_connect = on_connect
#client.on_message = on_message

def get_heat():
    # The way the nest works 1 is don't heat and 0 is heat
    val = automationhat.input.one.read()
    print(val)
    return(val)


try:
    client.connect(broker, broker_port, 10)
except:
    pass

while(True):
    client.loop(timeout=1.0, max_packets=1)
    heat = get_heat()
    client.publish(topic, payload=heat)
    client.publish(direct_control_topic, payload=heat) # Use this so that we don't have to use automations in HA, just communicates directly
    print(heat)
    time.sleep(5.0)

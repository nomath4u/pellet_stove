import time
import smbus
import Adafruit_MCP9808.MCP9808 as MCP9808
import paho.mqtt.client as mqtt

sensor = MCP9808.MCP9808()
sensor.begin()

broker = "192.168.1.140"
broker_port = 1883
topic = "/faux_nest/temp"
client = mqtt.Client()
#client.on_connect = on_connect
#client.on_message = on_message
try:
    client.connect(broker, broker_port, 10)
except:
    pass

while(True):
    client.loop(timeout=1.0, max_packets=1)
    temp = sensor.readTempC()
    client.publish(topic, payload=round(temp,1))
    print temp
    time.sleep(5.0)

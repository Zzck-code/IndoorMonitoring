from sense_hat import SenseHat
import time
import MySQLdb
import os
import sys
import paho.mqtt.client as mqtt
import json

THINGSBOARD_HOST = '10.207.161.39'
ACCESS_TOKEN = "RaspberryPiSenseHat"

INTERVAL=15

sensor_data = {'temperature': 0, 'pressure': 0, 'humidity': 0 }

next_reading = time.time()

client = mqtt.Client()

client.username_pw_set(ACCESS_TOKEN)

client.connect(THINGSBOARD_HOST, 1883, 60)

client.loop_start()

sense = SenseHat()

while True:
    temperature = sense.get_temperature()
    pressure = sense.get_pressure()
    humidity = sense.get_humidity()

    # formula see belowhy
    temperature = round( 0.0071*temperature*temperature+0.86*temperature-10.0,1)
    humidity = round( humidity*(2.5-0.029*temperature),1)
    pressure = round(pressure, 1)
    sensor_data['temperature'] = temperature
    sensor_data['pressure'] = pressure
    sensor_data['humidity'] = humidity
    
    client.publish('v1/devices/me/telemetry', json.dumps(sensor_data), 1)
    
    next_reading += INTERVAL
    sleep_time = next_reading-time.time()
    if sleep_time > 0:
        time.sleep(sleep_time)


client.loop_stop()
client.disconnect()

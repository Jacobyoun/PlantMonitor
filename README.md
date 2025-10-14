# Plant Monitor Project

An IoT system that uses a soil moisture sensor, a humidity sensor, a temperature sensor, and a light sensor to collect house plant environment data. This data is being used to train
a TinyML model to predict when the plant (a peacelily) will next need to be watered.

Check out the data graph here: https://www.jacobyoungerman.com/plantmonitor

# Tech Stack:

The main technology for the project is a Particle Argon Kit. This system has a Nordic nRF52840 and Espressif ESP32 running Particle's Device OS which is build on FreeRTOS.
The Argon Kit is integrated with a Photodiode light sensor, a Gwoop Digital Humidity and Temperature Sensor, and Gowoops 2pcs Temp/Humidity sensor, and a Stemedu Capacitive Analog Soil Moisture Sensor Module.

The data collected by the hardware is sent to the particle cloud as a JSON string. This data is sent from the cloud to a google sheet via a webhook. This data is then converted to a csv and parsed by the code from my 
personal website project. It is then displayed at https://www.jacobyoungerman.com/plantmonitor.

# Project Hardware:
Particle Argon Kit: https://docs.particle.io/argon/
Temp Sensor: https://www.amazon.com/dp/B073F472JL?ref=ppx_yo2ov_dt_b_fed_asin_title
Soil Moisture Sensor: https://www.amazon.com/dp/B0BTHL6M19?ref=ppx_yo2ov_dt_b_fed_asin_title


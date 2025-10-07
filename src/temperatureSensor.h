/**
 * Header file for temperatureSensor.cpp
 */
#pragma once;
#include "Particle.h" 

class TemperatureSensor {
    private:

        uint8_t PIN = D3;               // Contains the pin the sensor is connected to
        byte dataArray[5] = {0x00};          // Array to hold the 5 bytes of data from the sensor
        float humidity = 0.0f;          // Variable to hold the humidity value
        float temp = 0.0f;              // Variable to hold the temperature value
        float prevTemp;                 // Variable to hold the previous temperature value
        float prevHumidity;             // Variable to hold the previous humidity value
        bool TemperatureSensorRead();   // Function to read the sensor and update temp and humidity

    public:
        TemperatureSensor();     // Function to initialize the sensor (constructor)
        std::vector<float>  GetTemperatureAndHumidity(); // Function to get temperature and humidity
};

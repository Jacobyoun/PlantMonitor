/**
 * Header file for moistureSensor.cpp
 */


#pragma once
#include "Particle.h"

/**
 * Class for interfacing with a soil moisture sensor
 */
class MoistureSensor {
private:
    uint8_t PIN = A1;      // Pin the sensor is connected to
    const int baud = 9600; // Baud rate for serial communication
    int moistureLevel = 0; // Variable to hold the moisture level

public:
    MoistureSensor();      // Constructor to initialize the sensor
    float  GetMoisture();    // Function to get moisture level and print to the serial log
};
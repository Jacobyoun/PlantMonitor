/**
 * Source file for moistureSensor.cpp
 * 
 * @author Jacob Youngerman (github.com/jacobyoun1)
 */

#include "Particle.h"
#include "moistureSensor.h"

/**
 * Let the serial log know the sensor is online
 */
MoistureSensor::MoistureSensor() {
    // Wait for serial to initialize (initialize in my_first_project.cpp)
    Serial.println("DHT22 Temperature and Humidity Sensor Online");
}

/**
 * Function to get moisture level and print to the serial log
 */
float MoistureSensor::GetMoisture() {
    const int dry = 2200;   // Dry value when put in very dry soil, adjust based on your calibration
    const int wet = 600;    // Wet value when put in highly saturated soil, adjust based on your calibration
    const int dif = dry - wet;

    // read raw value
    int raw = analogRead(mPIN);

    // map raw to a percentage
    float percent = 100.0f * (dry - raw) / dif;
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 100.0f) percent = 100.0f;

    // Return the percentage
    return percent;
}
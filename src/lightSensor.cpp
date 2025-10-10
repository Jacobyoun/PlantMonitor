
#include "lightSensor.h"

LightSensor::LightSensor() {
    // Wait for serial to initialize (initialize in my_first_project.cpp)
    Serial.println("DHT22 Temperature and Humidity Sensor Online");
}

/**
 * Function to get brightness level and print to the serial log
 */
int LightSensor::GetBrightness() {
    // Raw analog value from 0-4095 (lower means higher light level)
    mLightLevel = analogRead(mPIN);

    // Convert to percentage (0-100)
    // Values below minVal are considered 100% light, above maxVal are 0% light
    int minVal = 30;
    int maxVal = 400;
    int lightPercent = 100 - ((mLightLevel - minVal) * 100 / (maxVal - minVal));

    // clamp to 0-100
    if (lightPercent < 0) lightPercent = 0;
    if (lightPercent > 100) lightPercent = 100;

    return lightPercent;
}

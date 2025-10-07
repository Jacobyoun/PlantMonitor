
#include "Particle.h"
#include "temperatureSensor.h"
#include "moistureSensor.h"
#include "lightSensor.h"

TemperatureSensor tempSensor; // Initialize the temperature sensor
LightSensor lightSensor; // Initialize the light sensor
MoistureSensor moistureSensor; // Initialize the moisture sensor
std::vector<int> light_levels = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Vector to hold light levels

/**
 * The setup function runs once when you press reset or power the board
 * 
 * Starts all of the sensors and initializes the serial log
 */
void setup() {
    Serial.begin(9600);
    delay(2000); // Give time for sensors to initialize
}

/**
 * Main control loop of the program
 */
void loop() {
    // Get light level every minute for 15 minutes and upload the average to the cloud
    std::vector<int> light_levels = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < 15; i++) { 
        light_levels[i] = lightSensor.GetBrightness();
        delay(60000); // Wait for 1 minute
    }
    int averageLightLevel = 0;
    for (int level : light_levels) {
        averageLightLevel += level;
    }
    averageLightLevel /= 15;

    // Get temperature and humidity
    std::vector<float> temp_humidity = tempSensor.GetTemperatureAndHumidity();
    float fahrenheit = temp_humidity.at(0) * 9 / 5 + 32;

    // Get moisture level
    float moisture = moistureSensor.GetMoisture();
    
    // Format data as JSON
    char jsonData[128];
    snprintf(jsonData, sizeof(jsonData), 
             "{\"tempC\":%.1f,\"tempF\":%.1f,\"humidity\":%.1f,\"light\":%d,\"moisture\":%.1f}", 
             temp_humidity.at(0), fahrenheit, temp_humidity.at(1), averageLightLevel, moisture);

    // Publish to Particle Cloud
    Particle.publish("sensorData", jsonData, PRIVATE);

    // Also print locally for debugging
    Serial.println(jsonData);
}


#include "Particle.h"
#include "temperatureSensor.h"
#include "moistureSensor.h"
#include "lightSensor.h"

TemperatureSensor tempSensor; // Initialize the temperature sensor
LightSensor lightSensor; // Initialize the light sensor
MoistureSensor moistureSensor; // Initialize the moisture sensor
std::vector<int> light_levels = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Vector to hold light levels
std::vector<float> soil_moisture_levels = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Vector to hold soil moisture levels

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
    // Get light level  and moisture level every minute for 15 minutes and upload the average to the cloud
    for (int i = 0; i < 15; i++) { 
        soil_moisture_levels[i] = moistureSensor.GetMoisture();
        light_levels[i] = lightSensor.GetBrightness();
        delay(59000); // Wait slightly less than a minute to account for execution time
    }
    int averageLightLevel = 0;
    float averageMoisture = 0;
    for (int level : light_levels) {
        averageLightLevel += level;
    }
    for (float moisture : soil_moisture_levels) {
        averageMoisture += moisture;
    }
    averageLightLevel /= 15;
    averageMoisture /= 15.0f;

    // Get temperature and humidity
    std::vector<float> temp_humidity = tempSensor.GetTemperatureAndHumidity();
    float fahrenheit = temp_humidity.at(0) * 9 / 5 + 32;
    
    // Format data as JSON
    char jsonData[128];
    snprintf(jsonData, sizeof(jsonData), 
             "{\"tempC\":%.1f,\"tempF\":%.1f,\"humidity\":%.1f,\"light\":%d,\"moisture\":%.1f}", 
             temp_humidity.at(0), fahrenheit, temp_humidity.at(1), averageLightLevel, averageMoisture   );
    // After collecting light readings and other sensor data

    // Get current minute and second
    int currentMinute = Time.minute();
    int currentSecond = Time.second();

    // Compute how many seconds to wait until next quarter-hour
    int minutesPastQuarter = currentMinute % 15;
    int secondsToWait = (15 - minutesPastQuarter) * 60 - currentSecond;

    // If we're already exactly on the quarter-hour, no need to wait
    if (secondsToWait < 0) {
        secondsToWait = 0;
    }
    if (secondsToWait > 0) {
        delay(secondsToWait * 1000); // Wait in milliseconds
    }

    // Now publish
    Particle.publish("sensorData", jsonData, PRIVATE);
    Serial.println(jsonData);
}



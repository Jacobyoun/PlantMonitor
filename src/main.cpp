
#include "Particle.h"
#include "temperatureSensor.h"
#include "moistureSensor.h"
#include "lightSensor.h"

TemperatureSensor tempSensor; // Initialize the temperature sensor
LightSensor lightSensor; // Initialize the light sensor
MoistureSensor moistureSensor; // Initialize the moisture sensor
std::vector<int> light_levels = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Vector to hold light levels
std::vector<float> soil_moisture_levels = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Vector to hold soil moisture levels

unsigned long lastSampleTime = 0;
unsigned long nextPublishTime = 0;
int sampleIndex = 0;
int cycles = 0;

const unsigned long SAMPLE_INTERVAL = 59000;     // 59 seconds minute

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
    unsigned long now = millis();

    // Take a reading every minute
    if (sampleIndex < 15 && (now - lastSampleTime >= SAMPLE_INTERVAL)) {
        lastSampleTime = now;

        soil_moisture_levels[sampleIndex] = moistureSensor.GetMoisture();
        light_levels[sampleIndex] = lightSensor.GetBrightness();

        Serial.printf("Minute %d - Light: %d, Moisture: %.2f\n", 
                      sampleIndex + 1, light_levels[sampleIndex], soil_moisture_levels[sampleIndex]);

        sampleIndex = (sampleIndex + 1);
    }

    // Set the time for the next quarter-hour publish
    if (sampleIndex > 14 && nextPublishTime == 0) {
        // Compute time until the next quarter-hour boundary
        int currentMinute = Time.minute();
        int currentSecond = Time.second();
        int minutesPastQuarter = currentMinute % 15;
        int secondsToNextQuarter = (15 - minutesPastQuarter) * 60 - currentSecond;
        
        // Get how long until the next publish time
        nextPublishTime = millis() + secondsToNextQuarter * 1000UL;
        Serial.printf("Next publish scheduled in %d seconds\n", secondsToNextQuarter);
    }

    // If we've reached or passed the scheduled time, publish
    if (sampleIndex > 14 && now >= nextPublishTime) {
        // Reset for the next cycle
        nextPublishTime = 0;
        sampleIndex = 0;     

        // Compute averages
        int sumLight = 0;
        float sumMoisture = 0;
        for (int i = 0; i < 15; i++) {
            sumLight += light_levels[i];
            sumMoisture += soil_moisture_levels[i];
        }

        int avgLight = sumLight / 15;
        float avgMoist = sumMoisture / 15.0f;

        // Temperature + humidity
        std::vector<float> temp_humidity = tempSensor.GetTemperatureAndHumidity();
        float tempC = temp_humidity.at(0);
        float humidity = temp_humidity.at(1);
        float tempF = tempC * 9 / 5 + 32;

        // Format the data
        char jsonData[128];
        snprintf(jsonData, sizeof(jsonData),
                 "{\"tempC\":%.1f,\"tempF\":%.1f,\"humidity\":%.1f,\"light\":%d,\"moisture\":%.1f}",
                 tempC, tempF, humidity, avgLight, avgMoist);

        // Publish the data
        Particle.publish("sensorData", jsonData, PRIVATE);
        Serial.printlnf("Published data at quarter-hour: %s", jsonData);

        cycles += 1;
    }

    // Reset sensors every 24 hours to prevent drift
    if (cycles >= 96) {
        moistureSensor.resetSensor();
        tempSensor.resetSensor();
        cycles = 0;
    }
}


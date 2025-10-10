/**
 * Code for reading DHT22 temperature and humidity sensor
 * and publishing the readings to the Particle Cloud.
 * Based on the DHT22 timing protocol.
 * 
 * Wiring is done manually without using external libraries. (could not get any libraries to play nicely with Particle)
 * @author Jacob Youngerman (github.com/jacobyoun1)
 */
#include "temperatureSensor.h"


/**
 * The setup function runs once when you press reset or power the board
 */
TemperatureSensor::TemperatureSensor() {
    pinMode(mPIN, INPUT_PULLUP);
    // Wait for serial to initialize (initialize in my_first_project.cpp)
    Serial.println("DHT22 Temperature and Humidity Sensor Online");
}

/**
 * Function to get temperature and humidity and print to the serial log
 */
std::vector<float> TemperatureSensor::GetTemperatureAndHumidity(){
    for (int i = 0; i < 3; i++) { // Try up to 3 times to get a reading
        }
        if (TemperatureSensorRead()) {
            //Serial.printf("Temp: %.1fC/%.1fF, Humidity: %.1f%%\n", temp, humidity);
            mPrevTemp = mTemp;
            mPrevHumidity = mHumidity;
        } 
        delay(500);
    return {mPrevTemp, mPrevHumidity};
};

/**
 * Reads the DHT22 sensor and updates the global temp and humidity variables
 * @return true if the reading was successful, false otherwise
 */
bool TemperatureSensor::TemperatureSensorRead() {
    unsigned long lastRead = 0;
    int BITS = 40;
    
    // Reset mDataArray array
    for (int i = 0; i < 5; i++) {
        this->mDataArray[i] = 0;
    }
    
    // Tell the sensor to start sending mDataArray
    pinMode(mPIN, OUTPUT);
    digitalWrite(mPIN, LOW);
    delay(2);
    
    // Stop interrupts and prepare to read mDataArray
    noInterrupts();
    digitalWrite(mPIN, HIGH);
    delayMicroseconds(40);
    pinMode(mPIN, INPUT_PULLUP);
    
    //
    // The next three loops make sure that the sensor is responding to us asking it to read
    // If it is responding, it should pull the line low for ~80us, then high for ~80us
    // If not, return false.
    //

    // Wait for sensor response (low for ~80us)
    lastRead = micros(); // micros() tells us what time it is in microseconds
    while (digitalRead(mPIN)) {
        if ((micros() - lastRead) > 500) { // So we can do micros() - lastRead to see how long we've been waiting
            interrupts();
            return false;
        }
    }
    // Wait through low period (~80us)
    lastRead = micros();
    while (!digitalRead(mPIN)) {
        if ((micros() - lastRead) > 500) {
            interrupts();
            return false;
        }
    }
    // Wait through high period (~80us)
    lastRead = micros();
    while (digitalRead(mPIN)) {
        if ((micros() - lastRead) > 500) {
            interrupts();
            return false;
        }
    }
    

    //
    // Read 40 bits of mDataArray
    // Each bit starts with a ~50us low period, followed by a high period
    // The 50us low period is to tell us that the sensor is about to send a bit, it can be ignored.
    // The length of the high period determines whether it's a 0 or 1
    //
    unsigned long times[BITS * 2] = {0};
    for (uint8_t i = 0; i < BITS * 2; i += 2) {
        // Measure low period. This is just telling us that the sensor is about to send a bit, so don't have do anything
        // But if it takes too long, the sensor is not responding, so return false
        lastRead = micros();
        while (!digitalRead(mPIN)) {
            if ((micros() - lastRead) > 500) {
                interrupts();
                return false;
            }
        }
        // Only store the low time for debugging purposes
        times[i] = micros() - lastRead;
        
        // Measure high period (determines bit value)
        // Again, if it takes too long, return false
        lastRead = micros();
        while (digitalRead(mPIN)) {
            if ((micros() - lastRead) > 500) {
                interrupts();
                return false;
            }
        }
        times[i + 1] = micros() - lastRead;
    }
    interrupts();
    
    //
    // Convert timings to bits
    //
    for (uint8_t i = 0; i < BITS * 2; i += 2) {
        // Divide by 16 to get the byte index (0-4). 16 since we also stored the low time. Each bit has 2 entries in times[].
        uint8_t byteIndex = i / 16;
        // Left shift by one bit to make room for the new bit
        mDataArray[byteIndex] = mDataArray[byteIndex] << 1;
        
        // If high time > 40us, it's a 1
        if (times[i + 1] > 40) {
            // So then we OR that 1 with the current byte and the LSB (the rightmost bit) becomes a 1
            mDataArray[byteIndex] = mDataArray[byteIndex] | 1;
        }
    }
    
    // Verify checksum
    uint8_t checksum = (mDataArray[0] + mDataArray[1] + mDataArray[2] + mDataArray[3]) & 0xFF;

    // The checksum should match the last byte we received (The sensor sends mDataArray[4] as a way to verify the mDataArray. 
    // It should be equal to the sum of the first 4 bytes anded with 0xFF)
    if (mDataArray[4] != checksum) {
        //Serial.print("Checksum failed - Expected: ");
        //Serial.print(checksum);
        //Serial.print(", Got: ");
        //Serial.println(mDataArray[4]);
        return false;
    }
    
    // Humidity is stored in teh first 2 bytes: mDataArray[0] (high byte) + mDataArray[1] (low byte) / 10
    mHumidity = ((mDataArray[0] << 8) | mDataArray[1]) / 10.0f;
    
    // Temperature: mDataArray[2] (high byte) + mDataArray[3] (low byte) / 10
    // Check for negative temperature (MSB of mDataArray[2] since it is a signed value)
    if (mDataArray[2] & 0x80) {
        mTemp = -((((mDataArray[2] & 0x7F) << 8) | mDataArray[3]) / 10.0f);
    } else {
        mTemp = (((mDataArray[2] << 8) | mDataArray[3]) / 10.0f);
    }
    
    return true;
}
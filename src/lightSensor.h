#pragma once
#include "Particle.h"

class LightSensor {
private:
    uint8_t PIN = A5;
    const int baud = 9600;
    int lightLevel = 0;

public:
    LightSensor();
    int GetBrightness();
};
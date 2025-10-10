#pragma once
#include "Particle.h"

class LightSensor {
private:
    uint8_t mPIN = A5;
    const int Baud = 9600;
    int mLightLevel = 0;

public:
    LightSensor();
    int GetBrightness();
};
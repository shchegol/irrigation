#pragma once

class Sensor {
public:
    void setPin(int p);
    int getResistance();
    int getHumidity();

private:
    int pin;
    int resistance;
    int minValue = 600;
    int maxValue = 900;

    void checkResistance();
    void calibrateResistanceRange();
};
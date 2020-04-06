#pragma once

class Sensor {
public:
    void setPin(int pin);
    int getResistance();
    int getHumidity();
private:
    int _pin;
    int _resistance;
    int _humidity;
    int _minResistance = 900;
    int _maxResistance = 400;

    void convertToHumidity();
    void checkResistance();
    void calibrateResistance();
};
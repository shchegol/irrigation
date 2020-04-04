#pragma once

class Sensor {
public:
    void setPin(int pin);
    int read();
    int readPercent();
private:
    int _pin;
    int _data;
};
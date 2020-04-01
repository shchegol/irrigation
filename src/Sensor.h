#pragma once

class Sensor {
public:
    void setPin(int pin);
    int read();
    int readPercent();
private:
    int pin;
    int data;
};
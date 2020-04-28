#pragma once

class Sensor {
public:
    void setPin(int p);
//    void setDate(int d, int h);
    int getResistance();
    int getHumidity();

private:
    int pin;
    int resistance;
    int humidity;
    int minResistance = 900;
    int maxResistance = 600;
    // int currentDate[2] = {0, 0}; // todo проверка по дате
    // int keyDate[2] = {0, 0}; // todo проверка по дате

    void convertToHumidity();
    void checkResistance();
    void calibrateResistanceRange();
};
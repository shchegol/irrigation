#pragma once

class Display {
public:
    Display(int p);

    void setPin(int pin);
    void setData(int values[4], int sensorsNum);
    void draw(int hour, int minute);

private:
    int pin;
    int sensorsNum;
    int values[4];

    void prepare(void);
    void drawSensor(int sensorName, int value, int fromLeft);
};
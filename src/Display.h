#pragma once

class Display {
public:
    void setPin(int pin);
    void setSensor(int position, int value);
    void setTime(int hour, int minute);
    void draw();

private:
    int _pin;
    int _sensors[4] = {-1, -1, -1, -1}; // sensors data
    int _time[2]; // hours and minutes

    void drawSensor(int sensorName, int value, int fromLeft);
    void drawTime();
    void prepare();
};

#pragma once

class Display {
public:
    void setPin(int p);
    void setSensor(int position, int value);
    void setTime(int hour, int minute);
    void draw();

private:
    int pin;
    int sensors[4] = {-1, -1, -1, -1}; // sensors data
    int time[2]; // hours and minutes

    void drawSensor(int sensorName, int value, int fromLeft);
    void drawTime();
    void prepare();
};

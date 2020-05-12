#pragma once

class Display {
public:
    Display(int p);

    void drawStartWindow();
    void drawSensorsWindow(int sensors[4], int time[2]);

private:
    int pin;
    void prepare();
};

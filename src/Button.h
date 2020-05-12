#pragma once

class Button {
public:
    Button(int p);

    bool isClick = false; // the button is clicked
    bool isLongPress = false; // the button is clamped
    void scanState(); // check the signal
    void setLongPressTime(int t); // set time for state when button is pressed

private:
    int counter = 0;
    int timeClick = 5; // before click
    int timeLongPress = 1000; // before long press
    int pin;
};
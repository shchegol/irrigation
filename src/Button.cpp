#include <Arduino.h>
#include <Button.h>

Button::Button(int p) {
    pin = p;
    pinMode(pin, INPUT);
}

/**
 * Check button state
 * isClick = true - button clicked
 * isClick = true - button clumped
 */
void Button::scanState() {
    if (digitalRead(pin)) {
        counter++;
    } else {
        counter = 0;
        isClick = false;
        isLongPress = false;

        return;
    }

    isClick = counter == timeClick;
    isLongPress = counter == timeLongPress;
}

/**
 * Set number of iterations before long click
 * @param t - new number of iterations
 */
void Button::setLongPressTime(int t) {
    if (t > 1000 && t < 10000) {
        timeLongPress = t;
    }
}

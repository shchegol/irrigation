#include <Arduino.h>
#include <Button.h>

/**
 * Check button state
 * flagPress = true - button pressed
 * flagPress = false - button not pressed
 * flagClick = true - clicked
 */
void Button::scanState() {
    if (flagPress == (!digitalRead(pin))) {
        buttonCount = 0;
    } else {
        buttonCount++;

        if (buttonCount >= timeButton) {
            flagPress = !flagPress;
            buttonCount = 0;

            if (flagPress) flagClick = true;
        }
    }
}

/**
 * Set pin and confirmation time
 */
void Button::setPinTime(int p, int tButton) {
    pin = p;
    timeButton = tButton;
    pinMode(pin, INPUT_PULLUP);
}

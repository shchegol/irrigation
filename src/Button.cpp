#include <Arduino.h>
#include <Button.h>

/**
 * Check button state
 * flagPress = true - button pressed
 * flagPress = false - button not pressed
 * flagClick = true - clicked
 */
void Button::scanState() {
    if (flagPress == (!digitalRead(_pin))) {
        _buttonCount = 0;
    } else {
        _buttonCount++;

        if (_buttonCount >= _timeButton) {
            flagPress = !flagPress;
            _buttonCount = 0;

            if (flagPress) flagClick = true;
        }
    }
}

/**
 * Set pin and confirmation time
 */
void Button::setPinTime(int pin, int timeButton) {
    _pin = pin;
    _timeButton = timeButton;
    pinMode(_pin, INPUT_PULLUP);
}

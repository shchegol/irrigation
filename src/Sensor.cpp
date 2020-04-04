#include "Arduino.h"
#include "Sensor.h"

void Sensor::setPin(int pin) {
    if (pin >= 0 && pin < 4) {
        _pin = pin;
        pinMode(_pin, INPUT);
    }
}

int Sensor::read() {
    _data = analogRead(_pin);
    return _data;
}

int Sensor::readPercent() {
    Sensor::read();
    int percent = _data;
    percent = map(percent, 1000, 300, 0, 100); // 1023 300 0 100
    percent = constrain(percent, 0, 100);
    Serial.println(_data);
    return percent;
}

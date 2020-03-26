#include "Arduino.h"
#include "Sensor.h"

// todo переделать в конструктор
void Sensor::setPin(int pin) {
    if (pin >= 0 && pin < 4) {
        this->pin = pin;
        pinMode(pin, INPUT);
    }
}

int Sensor::read() {
    data = analogRead(pin);
    return data;
}

int Sensor::readPercent() {
    Sensor::read();
    int percent = data;
    percent = map(percent, 1000, 300, 0, 100); // 1023 300 0 100
    percent = constrain(percent, 0, 100);
    Serial.println(data);
    return percent;
}

#include <Arduino.h>
#include <Sensor.h>

/**
 * Set IO pin
 * @param {int} pin - io pin number.
 */
void Sensor::setPin(int pin) {
    if (pin >= 0 && pin < 4) {
        _pin = pin;
        pinMode(_pin, INPUT);
    }
}

/**
 * Gets the current resistance at the sensor
 * @return {int} _resistance - actual resistance
 */
int Sensor::getResistance() {
    Sensor::checkResistance();
    return _resistance;
}

/**
 * Gets the current humidity
 * @return {int} _humidity - humidity
 */
int Sensor::getHumidity() {
    Sensor::convertToHumidity();
    return _humidity;
}

/**
 * Converts resistance from sensors to a percentage of humidity
 */
void Sensor::convertToHumidity() {
    Sensor::checkResistance();
    int humidity = _resistance;

    humidity = map(humidity, _minResistance, _maxResistance, 0, 100); // 1023 300 0 100
    humidity = constrain(humidity, 0, 100);

    _humidity = humidity;
}

/**
 * Checks resistance on sensor
 */
void Sensor::checkResistance() {
    Sensor::calibrateResistance();
    _resistance = analogRead(_pin);
}

/**
 * Upper and lower resistance calibration
 */
void Sensor::calibrateResistance() {
    if (_resistance > _minResistance) {
        _minResistance = _resistance;
    }

    if (_resistance < _maxResistance) {
        _maxResistance = _resistance;
    }
}

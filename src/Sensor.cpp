#include <Arduino.h>
#include <Sensor.h>

/**
 * Set IO pin
 * @param {int} pin - io pin number.
 */
void Sensor::setPin(int p) {
    if (p >= 0 && p < 4) {
        pin = p;
        pinMode(pin, INPUT);
    }
}

//void Sensor::setDate(int d, int h) {
//    currentDate[0] = d;
//    currentDate[1] = h;
//}

/**
 * Gets the current resistance at the sensor
 * @return {int} _resistance - actual resistance
 */
int Sensor::getResistance() {
    Sensor::checkResistance();
    return resistance;
}

/**
 * Gets the current humidity
 * @return {int} _humidity - humidity
 */
int Sensor::getHumidity() {
    Sensor::convertToHumidity();
    return humidity;
}

/**
 * Converts resistance from sensors to a percentage of humidity
 */
void Sensor::convertToHumidity() {
    Sensor::checkResistance();
    humidity = constrain(map(resistance, minResistance, maxResistance, 0, 100), 0, 100);
}

/**
 * Checks resistance on sensor
 */
void Sensor::checkResistance() {
    resistance = analogRead(pin);
    Sensor::calibrateResistanceRange();
}

/**
 * Upper and lower resistance calibration
 */
void Sensor::calibrateResistanceRange() {
    Serial.println("");
    Serial.println("*** calibration info ***");
    Serial.print("resistance: ");
    Serial.println(resistance);

    // set initial value
//    if (minResistance == -1 || maxResistance == -1) {
//        minResistance = maxResistance = resistance;
//    }

//    if (resistance > minResistance) {
//        minResistance = resistance;
//    }

    if (resistance < maxResistance) {
        maxResistance = resistance;
    }

    Serial.print("minResistance: ");
    Serial.println(minResistance);
    Serial.print("maxResistance: ");
    Serial.println(maxResistance);
    Serial.println("************************");
}

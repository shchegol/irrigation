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

// todo для колибровки датчика с помощью
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
    Sensor::checkResistance();
    return constrain(map(resistance, maxValue, minValue, 0, 100), 0, 100);;
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
//    Serial.println("");
//    Serial.println("*** calibration info ***");
//    Serial.print("resistance: ");
//    Serial.println(resistance);

    // set initial value
//    if (minValue == -1 || maxValue == -1) {
//        minValue = maxValue = resistance;
//    }

//    if (resistance < minValue) {
//        minValue = resistance;
//    }

// todo по сути не нужно т.к. на данный момен наличие датчика сравнивается со значением 1000
//    if (resistance > maxValue) {
//        maxValue = resistance;
//    }

//    Serial.print("minValue: ");
//    Serial.println(minValue);
//    Serial.print("maxValue: ");
//    Serial.println(maxValue);
//    Serial.println("************************");
}

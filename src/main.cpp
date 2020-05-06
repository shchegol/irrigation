#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <Button.h>
#include <Sensor.h>
#include <Display.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))
const int MESUREMENTS_TICK = 60 * 60 * 2; // measurement update step in seconds

const int SENSOR_PINS[4] = {0, 1, 2, 3};
const int SENSORS_COUNT = 1; // number of sensors (1-3), number is also pin for sensor
const int DISPLAY_PIN = 4; // A4

const int BTN_PIN = 2; // D2
const int RTC_PINS[3] = {3, 4, 5}; // RST, DAT, CLK
const int RELAY_PINS[4] = {7, 8, 9, 10}; // D7, D8, D9, D10
const int PUMP_PIN = 11; // D11

ThreeWire myWire(RTC_PINS[1], RTC_PINS[2], RTC_PINS[0]); // DAT/IO, CLK/SCLK RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime nextTick; // init RTC
Sensor waterSensor[SENSORS_COUNT]; // init array of water sensors
Display infoDisplay; // init display
Button checkSensorsBtn; // init btn

void printDateTime(const RtcDateTime &dt);

void doMesurments();

//////////////////////////////////////////////////////

void setup() {
    // init
    Serial.begin(9600);
    Rtc.Begin();

    checkSensorsBtn.setPinTime(BTN_PIN, 15);

    // display init
    infoDisplay.setPin(DISPLAY_PIN);
    for (int i = 0; i < SENSORS_COUNT; i++) {
        waterSensor[i].setPin(i);
    }

    // pump init
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);

    // relay init
    for (int i = 0, max = sizeof(RELAY_PINS) / sizeof(int); i < max; i++) {
        pinMode(RELAY_PINS[i], OUTPUT);
        digitalWrite(RELAY_PINS[i], HIGH);
    }

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    RtcDateTime now = Rtc.GetDateTime();
    nextTick = now + MESUREMENTS_TICK;

    // rtc test
    if (now < compiled) {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    } else if (now > compiled) {
        Serial.println("RTC is newer than compile time. (this is expected)");
    } else if (now == compiled) {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    Serial.print("Compiled: ");
    printDateTime(now);
    Serial.println();
    Serial.println("########################################");

    doMesurments();
}

void loop() {
    // get current time
    RtcDateTime now = Rtc.GetDateTime();

    if (nextTick <= now) {
        // Serial.println("mesurments tick");
        doMesurments();

        nextTick += MESUREMENTS_TICK;
    }

    // btn click
    checkSensorsBtn.scanState();

    if (checkSensorsBtn.flagClick) {
        // clicked
        checkSensorsBtn.flagClick = false;
        Serial.println("click btn");
        doMesurments();

//        if (digitalRead(PUMP_PIN)) {
//            digitalWrite(PUMP_PIN, LOW);
//            digitalWrite(RELAY_PINS[0], LOW);
//            Serial.println("LOW");
//        } else {
//            digitalWrite(PUMP_PIN, HIGH);
//            digitalWrite(RELAY_PINS[0], HIGH);
//            Serial.println("HIGH");
//        }

    }

    delay(1);
}

void printDateTime(const RtcDateTime &dt) {
    char datestring[20];

    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u.%02u.%04u %02u:%02u:%02u"),
               dt.Day(),
               dt.Month(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
    Serial.print(datestring);
}

/**
 * Сделать замеры со всех сенсоров
 */
void doMesurments() {
    RtcDateTime now = Rtc.GetDateTime();

    Serial.println();
    Serial.println();
    Serial.println();
    Serial.print("do mesurments: ");
    printDateTime(now);
    Serial.println();

    for (int i = 0; i < SENSORS_COUNT; i++) {
        // waterSensor[i].setDate(now.Day(), now.Hour());
        int humidity = waterSensor[i].getHumidity();
        bool relay = digitalRead(RELAY_PINS[i]);

        infoDisplay.setSensor(i, humidity);

        Serial.println("");
        Serial.println("----");
        Serial.print("Relay ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(relay);
        Serial.print("Pump: ");
        Serial.println(digitalRead(PUMP_PIN));
        Serial.print("Humidity: ");
        Serial.println(humidity);

        if (humidity == 0 && relay) {
            digitalWrite(RELAY_PINS[i], LOW); // turn on
            digitalWrite(PUMP_PIN, LOW);

            delay(1000);
            doMesurments();

            Serial.println("Way 1: pump turned on");
            Serial.println("----");
        }

        if (humidity < 10 && !relay) {
            Serial.println("Way 2: pump is working");
            Serial.println("----");

            delay(1000);
            doMesurments();
        }

        if (humidity > 10) {
            Serial.println("Way 3: pump turned off");
            Serial.println("----");

            digitalWrite(RELAY_PINS[i], HIGH); // turn off
            digitalWrite(PUMP_PIN, HIGH);
        }
    }

    infoDisplay.setTime(now.Hour(), now.Minute());
    infoDisplay.draw();
}

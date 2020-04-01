#include "Arduino.h"
#include "ThreeWire.h"
#include "RtcDS1302.h"
#include "Sensor.h"
#include "Display.h"

ThreeWire myWire(4, 5, 3); // DAT/IO, CLK/SCLK RST/CE
RtcDS1302 <ThreeWire> Rtc(myWire);

/////////////////////////////////////////////////////
const int MESUREMENTS_TICK = 60 * 60 * 2; // measurement update step in seconds
const int SENSORS_COUNT = 1; // number of sensors (1-3), number is also pin for sensor
const int DISPLAY_PIN = 4;
const int RELAY_PIN[4] = {9, 8, 7, 6};
// button todo кнопки перенести в класс
const int BTN_PIN = 2;
boolean lastButton = HIGH;
boolean currentButton = LOW;

boolean debounce(boolean last) {
    boolean current = digitalRead(BTN_PIN);
    if (last != current) {
        delay(5);
        current = digitalRead(BTN_PIN);
        return current;
    }

    return last;
}

RtcDateTime nextTick; // init RTC
Sensor waterSensor[SENSORS_COUNT]; // init array of water sensors
Display displaySensor; // init display

/**
 * Перевод в удобочитаемую строку показаний с rtc
 */
#define countof(a) (sizeof(a) / sizeof(a[0]))

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

    Serial.print("do mesurments: ");
    printDateTime(now);
    Serial.println();

    for (int i = 0; i < SENSORS_COUNT; i++) {
        displaySensor.setSensor(i, waterSensor[i].readPercent());
    }

    displaySensor.setTime(now.Hour(), now.Minute());
    displaySensor.draw();
}
//////////////////////////////////////////////////////

void setup() {
    Serial.begin(9600);
    Rtc.Begin();


    // set pins in Sensors
    for (int i = 0; i < SENSORS_COUNT; i++) {
        waterSensor[i].setPin(i);
    }

    // set pin for display
    displaySensor.setPin(DISPLAY_PIN);

    // btns pin init todo вывести в отдельный класс
    pinMode(BTN_PIN, INPUT);

    // relay pin init todo при подключении реле
//  for (int i = 0, max = sizeof (RELAY_PIN) / sizeof (int); i < max; i++) {
//    pinMode(RELAY_PIN[i], OUTPUT);
//    digitalWrite(RELAY_PIN[i], HIGH);
//  }

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

    // btn click todo перевести в отдельный класс
    currentButton = debounce(lastButton);
    if (currentButton != lastButton && currentButton != LOW) {
        Serial.println("click btn");
        doMesurments();
    }
    lastButton = currentButton;

    delay(1);
}

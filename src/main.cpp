#include "Arduino.h"
#include "ThreeWire.h"
#include "RtcDS1302.h"
#include "Button.h"
#include "Sensor.h"
#include "Display.h"

/**
 * Перевод в удобочитаемую строку показаний с rtc
 */
#define countof(a) (sizeof(a) / sizeof(a[0]))
#define MESUREMENTS_TICK 60 * 60 * 2 // measurement update step in seconds
#define SENSORS_COUNT 1 // number of sensors (1-3), number is also pin for sensor
#define DISPLAY_PIN 4 // A4
#define BTN_PIN 2
#define RTC_PIN_RST 3 // D3
#define RTC_PIN_DAT 4 // D4
#define RTC_PIN_CLK 5 // D5
// #define RELAY_PIN[4] {9, 8, 7, 6} // D9, D8, D7, D6

ThreeWire myWire(RTC_PIN_DAT, RTC_PIN_CLK, RTC_PIN_RST); // DAT/IO, CLK/SCLK RST/CE
RtcDS1302 <ThreeWire> Rtc(myWire);
RtcDateTime nextTick; // init RTC
Sensor waterSensor[SENSORS_COUNT]; // init array of water sensors
Display infoDisplay; // init display
Button checkSensorsBtn; // init btn

void printDateTime(const RtcDateTime &dt);
void doMesurments();

//////////////////////////////////////////////////////

void setup() {
    Serial.begin(9600);
    Rtc.Begin();

    // set pin for btn
    checkSensorsBtn.setPinTime(BTN_PIN, 15);

    // set pin for display
    infoDisplay.setPin(DISPLAY_PIN);

    // set pins in Sensors
    for (int i = 0; i < SENSORS_COUNT; i++) {
        waterSensor[i].setPin(i);
    }

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

    // btn click
    checkSensorsBtn.scanState();
    if ( checkSensorsBtn.flagClick ) {
        // clicked
        checkSensorsBtn.flagClick = false; //
        Serial.println("click btn");
        doMesurments();
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

    Serial.print("do mesurments: ");
    printDateTime(now);
    Serial.println();

    for (int i = 0; i < SENSORS_COUNT; i++) {
        infoDisplay.setSensor(i, waterSensor[i].readPercent());
    }

    infoDisplay.setTime(now.Hour(), now.Minute());
    infoDisplay.draw();
}

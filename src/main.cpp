#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <Button.h>
#include <Sensor.h>
#include <Display.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))

template<typename T>
Print &operator<<(Print &s, T n) {
    s.print(n);
    return s;
}

const int SENSOR_PINS[4] = {0, 1, 2, 3}; // A0, A1, A2, A3
const int SENSORS_COUNT = 4; // number of sensors
const int DISPLAY_PIN = 4; // A4
const int BTN_PIN = 2; // D2
const int RTC_PINS[3] = {3, 4, 5}; // RST, DAT, CLK
const int VALVE_PINS[4] = {8, 9, 10, 11}; // D7, D8, D9, D10
const int PUMP_PIN = 12; // D11
const int MESUREMENTS_TICK = 60 * 60 * 2; // measurement update step in seconds

bool isRunning = false; //

ThreeWire myWire(RTC_PINS[1], RTC_PINS[2], RTC_PINS[0]); // DAT/IO, CLK/SCLK RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);
RtcDateTime nextTick; // init RTC
Sensor waterSensor[SENSORS_COUNT]; // init array of water sensors
Display infoDisplay(DISPLAY_PIN); // init display
Button startBtn(BTN_PIN); // turn on/off measurements button

void printDateTime(const RtcDateTime &dt);

void doMeasurements();

//////////////////////////////////////////////////////

void setup() {
    Serial.begin(9600);

    // display
//    infoDisplay.setPin(DISPLAY_PIN);

    // sensors
    for (int i = 0; i < SENSORS_COUNT; i++) {
        waterSensor[i].setPin(SENSOR_PINS[i]);
    }

    // pump
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, HIGH);

    // relays
    for (int i = 0, max = sizeof(VALVE_PINS) / sizeof(int); i < max; i++) {
        pinMode(VALVE_PINS[i], OUTPUT);
        digitalWrite(VALVE_PINS[i], HIGH);
    }

    // RTC
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    RtcDateTime now = Rtc.GetDateTime();
    nextTick = now + MESUREMENTS_TICK;

    // RTC test
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

//    doMeasurements();
    infoDisplay.drawStartWindow();
}

void loop() {
    // btns click
    startBtn.scanState();

    if (startBtn.isClick) {
        // the button is clicked

        Serial.println();
        Serial.println("On/off button clicked");

        isRunning = true;

        doMeasurements();
    }

    if (startBtn.isLongPress) {
        Serial.println();
        Serial.println("Reset button clicked");

        isRunning = false;
        infoDisplay.drawStartWindow();
    }

    if (isRunning) {
        // get current time
        RtcDateTime now = Rtc.GetDateTime();

        if (nextTick <= now) {
            Serial.println();
            Serial.print("Next measurements time");

            doMeasurements();

            nextTick += MESUREMENTS_TICK;
        }
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
void doMeasurements() {
    RtcDateTime now = Rtc.GetDateTime();

    Serial.println();
    Serial.print("!!! Do measurements: ");
    printDateTime(now);

    int displaySensors[4] = {-1, -1, -1, -1};
    int displayTime[2] = {now.Hour(), now.Minute()};

    for (int i = 0; i < SENSORS_COUNT; i++) {
        int humidity = waterSensor[i].getHumidity();
        int resistance = waterSensor[i].getResistance();
        bool relay = digitalRead(VALVE_PINS[i]);

        // типа нет датчика
        if (resistance > 950) {
            displaySensors[i] = -1;
            digitalWrite(VALVE_PINS[i], HIGH);
            digitalWrite(PUMP_PIN, HIGH);

            continue;
        }

        displaySensors[i] = humidity;

        Serial.println(".......................");
        Serial << "Valve " << i << ": " << relay;
        Serial.println();
        Serial << "Pump: " << digitalRead(PUMP_PIN);
        Serial.println();
        Serial << "Resistance: " << resistance;
        Serial.println();

        if (humidity == 0 && relay) {
            // turn on the pump and valve
            digitalWrite(VALVE_PINS[i], LOW);
            digitalWrite(PUMP_PIN, LOW);

            // delay(1000);
            // doMeasurements();

            Serial.println("Way 1: turned on");
        }

        if (humidity < 10 && !relay) {
            Serial.println("Way 2: is working");

            // delay(1000);
            // doMeasurements();
        }

        if (humidity > 10) {
            Serial.println("Way 3: turned off");

            // turn off the pump and valve
            digitalWrite(VALVE_PINS[i], HIGH);
            digitalWrite(PUMP_PIN, HIGH);
        }
    }

    infoDisplay.drawSensorsWindow(displaySensors, displayTime);
}

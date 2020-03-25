#include "Arduino.h"
#include "ThreeWire.h"
#include "RtcDS1302.h"
#include "Sensor.h"
#include "Display.h"

ThreeWire myWire(4, 5, 3); // DAT/IO, CLK/SCLK RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);

/////////////////////////////////////////////////////
const int MESUREMENTS_TICK = 60 * 60 * 2; // measurement update step in seconds
RtcDateTime nextTick;

// sensors
const int SENSORS_NUM = 1; // number of sensors (1-3)
int sensorsData[SENSORS_NUM]; // sensors data  ray
Sensor waterSensor[SENSORS_NUM]; // init array of water sensors

// display
Display displaySensor(4); // init display (pin 4)

// relay
const int PIN_RELAY[4] = {9,8,7,6};

// button todo кнопки перенести в класс
const int PIN_BUTTON = 2;
boolean lastButton = HIGH;
boolean currentButton = LOW;
boolean debounce(boolean last) {
  boolean current = digitalRead(PIN_BUTTON);
  if (last != current) {
    delay(5);
    current = digitalRead(PIN_BUTTON);
    return current;
  }
}

/**
 * Сделать замеры со всех сенсоров
 */
void doMesurments() {
  RtcDateTime now = Rtc.GetDateTime();

  // Serial.print("do mesurments: ");
  // printDateTime(now);
  // Serial.println();

  for (int i = 0; i < SENSORS_NUM; i++) {
    sensorsData[i] = waterSensor[i].readPercent();

    // TODO применение реле
    // if (sensorsData[i] > 10) {
    //   digitalWrite(PIN_RELAY[i], HIGH);
    // } else {
    //   digitalWrite(PIN_RELAY[i], LOW);
    // }
  }

  displaySensor.setData(sensorsData, SENSORS_NUM);
  displaySensor.draw(now.Hour(), now.Minute());
}

/**
 * Перевод в удобочитаемую строку показаний с rtc
 */
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u.%02u.%04u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}

//////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Rtc.Begin();

  // btns pin init
  pinMode(PIN_BUTTON, INPUT);
  
  // relay pin init todo при подключении реле
//  for (int i = 0, max = sizeof (PIN_RELAY) / sizeof (int); i < max; i++) {
//    pinMode(PIN_RELAY[i], OUTPUT);
//    digitalWrite(PIN_RELAY[i], HIGH);
//  }
  
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  RtcDateTime now = Rtc.GetDateTime();
  nextTick = now + MESUREMENTS_TICK;

  // rtc test
  if (now < compiled)   {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // set pins in Sensors
  for (int i = 0; i < SENSORS_NUM; i++) {
    waterSensor[i].setPin(i);
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

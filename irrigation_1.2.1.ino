#include <ThreeWire.h>
#include <RtcDS1302.h>
#include "U8glib.h"

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);
ThreeWire myWire(4, 5, 3); // DAT/IO, CLK/SCLK RST/CE
RtcDS1302<ThreeWire> Rtc(myWire);

template <typename T> Print &operator << (Print &s, T n) {
  s.print(n);
  return s;
};

// blob state icons
static unsigned char blobs[][32] = {
  {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x04, 0x01, 0x04, 0x01, 0x04, 0x02, 0x02, 0x02, 0x02, 0x8C, 0x01, 0x70, 0x00}, // 0
  {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x04, 0x01, 0x04, 0x01, 0x04, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}, // 25
  {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0xFF, 0x07, 0xFF, 0x07, 0xFF, 0x07, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}, // 50
  {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0xFC, 0x01, 0xFE, 0x03, 0xFE, 0x03, 0xFF, 0x07, 0xFF, 0x07, 0xFF, 0x07, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}, // 75
  {0x20, 0x00, 0x70, 0x00, 0x70, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xFC, 0x01, 0xFC, 0x01, 0xFE, 0x03, 0xFE, 0x03, 0xFF, 0x07, 0xFF, 0x07, 0xFF, 0x07, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}  // 100
};

class Display {
  public:
    Display(byte p) {
      Display::setPin(p);
      pinMode(pin, OUTPUT);
    }

    void setPin(byte pin) {
      if (pin >= 0 && pin <= 5) {
        this->pin = pin;
      }
    }

    void setData(int values[4], byte sensorsNum) {
      this->sensorsNum = sensorsNum;

      for (int i = 0; i < sensorsNum; i++) {
        this->values[i] = values[i];
      }
    }

    void draw(const RtcDateTime& dt) {
      int fromLeft = 4;

      Display::prepare();

      for (byte i = 0; i < sensorsNum; i++) {
        Display::drawSensor(i, values[i], fromLeft);
        fromLeft += 35;
      }

      u8g.setPrintPos(0, 54);
      u8g << "last time: " << dt.Hour() << ":" << dt.Minute();
    }

  private:
    byte pin;
    byte sensorsNum;
    int values[4];

    void prepare(void) {
      u8g.setFont(u8g_font_6x10);
      u8g.setFontRefHeightExtendedText();
      u8g.setDefaultForegroundColor();
      u8g.setFontPosTop();
    }

    void drawSensor(byte sensorName, byte value, byte fromLeft) {
      byte blobNum = map(value, 0, 100, 0, 4);

      u8g.setPrintPos(fromLeft, 0);
      u8g << "A" << sensorName;
      u8g.drawXBM(fromLeft, 16, 11, 16, blobs[blobNum]);
      u8g.setPrintPos(fromLeft - 1, 36);
      u8g << value << "%";
    }
};

class Sensor {
  public:
    void setPin(byte pin) {
      if (pin >= 0 && pin < 4) {
        this->pin = pin;
        pinMode(pin, INPUT);
      }
    }

    int read() {
      data = analogRead(pin);
      return data;
    }

    int readPercent() {
      Sensor::read();
      int percent = data;
      percent = map(percent, 1000, 600, 0, 100); // 1023 300 0 100
      percent = constrain(percent, 0, 100);
      Serial.println(data);
      return percent;
    }

  private:
    byte pin;
    int data;
};

/////////////////////////////////////////////////////
const int MESUREMENTS_TICK = 60 * 60 * 2; // measurement update step in seconds
// const int MESUREMENTS_TICK = 2;
RtcDateTime nextTick;

// sensors
const byte SENSORS_NUM = 1; // number of sensors (1-3)
int sensorsData[SENSORS_NUM]; // sensors data  ray
Sensor waterSensor[SENSORS_NUM]; // init array of water sensors

// display
Display displaySensor(4); // init display (pin 4)

// relay
const int PIN_RELAY[4] = {9,8,7,6};

// button
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

//////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Rtc.Begin();

  // btns pin init
  pinMode(PIN_BUTTON, INPUT);
  
  // relay pin init
  for (byte i = 0, max = sizeof (PIN_RELAY) / sizeof (int); i < max; i++) {
    pinMode(PIN_RELAY[i], OUTPUT);
    digitalWrite(PIN_RELAY[i], HIGH);
  }
  
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  RtcDateTime now = Rtc.GetDateTime();
  nextTick = now + MESUREMENTS_TICK;

  if (now < compiled)   {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  for (byte i = 0; i < SENSORS_NUM; i++) {
    waterSensor[i].setPin(i);
  }

  Serial.print("Compiled: ");
  printDateTime(now);
  Serial.println();
  Serial.println("########################################");
  doMesurments(); 
}

void loop() {
  // RTC
  RtcDateTime now = Rtc.GetDateTime();

  if (nextTick <= now) {
    // Serial.println("mesurments tick");
    doMesurments();

    nextTick += MESUREMENTS_TICK;
  }

  // btn click
  currentButton = debounce(lastButton);
  if (currentButton != lastButton && currentButton != LOW) {
    Serial.println("click btn");
    doMesurments();
  }
  lastButton = currentButton;

  delay(1);
}

void doMesurments() {
  RtcDateTime now = Rtc.GetDateTime();

  // Serial.print("do mesurments: ");
  // printDateTime(now);
  // Serial.println();

  for (byte i = 0; i < SENSORS_NUM; i++) {
    sensorsData[i] = waterSensor[i].readPercent();

    // TODO применение реле
    // if (sensorsData[i] > 10) {
    //   digitalWrite(PIN_RELAY[i], HIGH);
    // } else {
    //   digitalWrite(PIN_RELAY[i], LOW);
    // }
  }

  displaySensor.setData(sensorsData, SENSORS_NUM);

  // picture loop
  u8g.firstPage();
  do {
    displaySensor.draw(now);
  } while ( u8g.nextPage() );
}

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

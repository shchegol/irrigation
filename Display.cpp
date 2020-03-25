#include "Arduino.h"
#include "U8glib.h"
#include "Display.h"

template <typename T> Print &operator << (Print &s, T n) {
    s.print(n);
    return s;
};

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);

// blob state icons
static unsigned char blobs[][32] = {
        {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x04, 0x01, 0x04, 0x01, 0x04, 0x02, 0x02, 0x02, 0x02, 0x8C, 0x01, 0x70, 0x00}, // 0
        {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x04, 0x01, 0x04, 0x01, 0x04, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}, // 25
        {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0xFF, 0x07, 0xFF, 0x07, 0xFF, 0x07, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}, // 50
        {0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x88, 0x00, 0x88, 0x00, 0x04, 0x01, 0xFC, 0x01, 0xFE, 0x03, 0xFE, 0x03, 0xFF, 0x07, 0xFF, 0x07, 0xFF, 0x07, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}, // 75
        {0x20, 0x00, 0x70, 0x00, 0x70, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xFC, 0x01, 0xFC, 0x01, 0xFE, 0x03, 0xFE, 0x03, 0xFF, 0x07, 0xFF, 0x07, 0xFF, 0x07, 0xFE, 0x03, 0xFE, 0x03, 0xFC, 0x01, 0x70, 0x00}  // 100
};

Display::Display(int p)
{
    Display::setPin(p);
    pinMode(pin, OUTPUT);
}

void Display::setPin(int pin)
{
    if (pin >= 0 && pin <= 5) {
        this->pin = pin;
    }
}

void Display::setData(int values[4], int sensorsNum)
{
    this->sensorsNum = sensorsNum;

    for (int i = 0; i < sensorsNum; i++) {
        this->values[i] = values[i];
    }
}

void Display::draw(int hour, int minute)
{
    int fromLeft = 4;

    Display::prepare();

    for (int i = 0; i < sensorsNum; i++) {
        Display::drawSensor(i, values[i], fromLeft);
        fromLeft += 35;
    }

    u8g.setPrintPos(0, 54);
    u8g << "last time: " << hour << ":" << minute;
}

void Display::prepare(void)
{
    u8g.setFont(u8g_font_6x10);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
}

void Display::drawSensor(int sensorName, int value, int fromLeft)
{
    int blobNum = map(value, 0, 100, 0, 4);

    u8g.setPrintPos(fromLeft, 0);
    u8g << "A" << sensorName;
    u8g.drawXBM(fromLeft, 16, 11, 16, blobs[blobNum]);
    u8g.setPrintPos(fromLeft - 1, 36);
    u8g << value << "%";
}


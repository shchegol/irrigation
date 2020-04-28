#pragma once

class Button {

public:
    bool flagPress;    // признак кнопка сейчас нажата
    bool flagClick;    // признак кнопка была нажата (клик)

    void scanState();   // метод проверки состояние сигнала
    void setPinTime(int p, int tButton); // метод установки номера вывода и времени подтверждения
private:
    int buttonCount;    // счетчик подтверждений стабильного состояния
    int timeButton;     // время подтверждения состояния кнопки
    int pin;            // номер вывода
};
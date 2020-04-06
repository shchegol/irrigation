#pragma once

class Button {

public:
    bool flagPress;    // признак кнопка сейчас нажата
    bool flagClick;    // признак кнопка была нажата (клик)

    void scanState();   // метод проверки состояние сигнала
    void setPinTime(int pin, int timeButton); // метод установки номера вывода и времени подтверждения
private:
    int _buttonCount;    // счетчик подтверждений стабильного состояния
    int _timeButton;     // время подтверждения состояния кнопки
    int _pin;            // номер вывода
};
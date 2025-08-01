/*#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>
#include <string>
#include <iostream>
class GPIO {
private:
    int PinSensor;
    int PinStrobo;
    gpiod_chip *chip;
    gpiod_line *sensorLine;
    gpiod_line *stroboLine;

public:
    GPIO(int pinSensor, int pinStrobo, const std::string &chipname = "gpiochip4");
    ~GPIO();

    bool ReadSensor();   // Retorna valor booleano do pino sensor
    void OutStrobo();    // Ativa e desativa o pino strobo
};
#endif
/*

sudo apt update
sudo apt install libgpiod-dev gpiod
*/
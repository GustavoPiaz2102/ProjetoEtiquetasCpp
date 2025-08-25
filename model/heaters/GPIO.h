/*#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
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
    void BlinkStrobo(int Delay); // Ativa o pino strobo por um tempo definido em milissegundos
};
#endif
*/
/*

sudo apt update
sudo apt install libgpiod-dev gpiod
*/
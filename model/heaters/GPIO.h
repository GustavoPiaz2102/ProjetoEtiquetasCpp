#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>


#define DebounceValue 2

class GPIO
{
private:
    int PinSensor;
    int PinStrobo;
    gpiod_chip *chip;
    gpiod_line *sensorLine;
    gpiod_line *stroboLine;
    bool LastSensorValue = false;
    bool firstRead = true;
    int ActualCounter = 0;
public:
    GPIO(int pinSensor, int pinStrobo, const std::string &chipname = "gpiochip4");
    ~GPIO();

    bool ReadSensor();           // Retorna valor booleano do pino sensor
    void OutStrobo();            // Ativa e desativa o pino strobo
    void BlinkStrobo(int Delay); // Ativa o pino strobo por um tempo definido em milissegundos
    void SetStroboHigh();        // Define o pino strobo como HIGH
    void SetStroboLow();         // Define o pino strobo como LOW
    void ReturnToFirst(){firstRead = true; ActualCounter = 0; LastSensorValue = false;} // Reseta a leitura do sensor
};
#endif

/*

sudo apt update
sudo apt install libgpiod-dev gpiod
*/

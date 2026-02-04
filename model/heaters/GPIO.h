#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

#define ADC_FILE_PATH "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define SENSOR_THRESHOLD 2000 
#define DebounceValue 1

class GPIO
{
private:
    int PinStrobo;
    gpiod_chip *chip;
    gpiod_line *stroboLine;
    
    bool LastSensorValue = false;
    bool firstRead = true;
    int ActualCounter = 0;

public:
    GPIO(int pinStrobo, const std::string &chipname = "gpiochip4");
    ~GPIO();

    bool ReadSensor();           
    int ReadADC_Raw();           

    void OutStrobo();            
    void BlinkStrobo(int Delay); 
    void SetStroboHigh();        
    void SetStroboLow();         
    
    void ReturnToFirst() { firstRead = true; ActualCounter = 0; LastSensorValue = false; }
};

#endif  
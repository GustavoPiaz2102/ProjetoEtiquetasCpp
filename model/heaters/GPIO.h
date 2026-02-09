#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

const std::string DEVICE_DIR = "/sys/bus/iio/devices/iio:device0/";
const std::string FILE_RAW   = DEVICE_DIR + "in_voltage0_raw";   
const std::string FILE_SCALE = DEVICE_DIR + "in_voltage0_scale"; 

#define SENSOR_THRESHOLD 190
#define DebounceValue 1

class GPIO
{
private:
	int PinStrobo;
	gpiod_chip *chip;
	gpiod_line *stroboLine;
	
	bool firstRead = true;
	int ActualCounter = 0;
	double scale;
	int lastValidatedRaw = 0;
	bool LastSensorState = false;
public:
	GPIO(int pinStrobo, const std::string &chipname = "gpiochip4");
	~GPIO();

	bool ReadSensor();           
	int ReadRaw();           

	void OutStrobo();            
	void BlinkStrobo(int Delay); 
	void SetStroboHigh();        
	void SetStroboLow();         
	void ReturnToFirst() { firstRead = true; ActualCounter = 0; LastSensorState = false; }
};

#endif  
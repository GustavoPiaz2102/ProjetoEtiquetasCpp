#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <fstream>

const std::string DEVICE_DIR = "/sys/bus/iio/devices/iio:device0/";
const std::string FILE_RAW   = DEVICE_DIR + "in_voltage0_raw";   
const std::string FILE_SCALE = DEVICE_DIR + "in_voltage0_scale"; 

#define SENSOR_THRESHOLD 16200
#define DebounceValue 5

class GPIO{
	private:
		int PinStrobo;
		gpiod_chip *chip;
		gpiod_line *stroboLine;
		std::ifstream fsRaw;
		
		double scale;
	public:
		// Flags
		bool firstRead = true;
		bool LastSensorState = false;
		int ActualCounter = 0;

		//
		GPIO(int pinStrobo, const std::string &chipname = "gpiochip4");
		~GPIO();

		bool ReadSensor();           
		int ReadRaw();           

		void OutStrobo();            
		void SetStroboHigh(int sleep);        
		void SetStroboLow(int sleep);         
		void ReturnToFirst() { firstRead = true; ActualCounter = 0; LastSensorState = false; }
};

#endif  // GPIO_H
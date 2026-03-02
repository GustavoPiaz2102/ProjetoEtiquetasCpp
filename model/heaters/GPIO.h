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

#define SENSOR_THRESHOLD 16000
#define SENSOR_HYSTERESIS 100 //(16k desliga e 16400 liga)
#define DEBOUNCE_MS 100 //periodo entre leituras
#define FILTER_ALPHA 0.25
// cada iteração contribui em 25% do valor final (aumentar se ficar muito suave)
class GPIO{
	private:
		int PinStrobo;
		gpiod_chip *chip;
		gpiod_line *stroboLine;
		std::ifstream fsRaw;
		
		double scale;

		// Encolder
		/*
		std::atomic<int> encoderPulses{0};
		std::thread encoderThread;
		gpiod_line *encoderLine;
		gpiod_line *stepLine;
		*/

		double smoothedValue = 0.0;
		bool stableState = false;
		bool lastLogicalState = false;
		std::chrono::steady_clock::time_point lastStateChange;

	public:
		// Flags
		bool firstRead = true;
		
		GPIO(int pinStrobo, const std::string &chipname = "gpiochip4");
		~GPIO();

		bool ReadSensor();
		int ReadRaw();

		void OutStrobo();
		void SetStroboHigh(int sleep = 0);
		void SetStroboLow(int sleep = 0);
		void ReturnToFirst() { 
			firstRead = true; 
			stableState = false; 
			lastLogicalState = false;
			smoothedValue = 0.0;
		}

		// Encoder

		// int GetAndResetEncoderPulses();
		// void MonitorEncoder();
};

#endif  // GPIO_H
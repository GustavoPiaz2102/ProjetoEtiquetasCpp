#ifndef HARDWAREPWM_H
#define HARDWAREPWM_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

class HardwarePWM {
	private:
		std::string pwmPath;
		int period_ns;

		void writeToFile(const std::string &path, const std::string &value);

	public:
		HardwarePWM(int chip, int channel); // chip (ex: 0), channel (ex: 0 para PWM0)

		void setFrequency(uint32_t freq_hz); // Define a frequência em Hz (mantendo o Duty Cycle sempre em 50%)

		void enable(bool state);
};

#endif // HARDWAREPWM_H
#ifndef GPIO_H
#define GPIO_H

#include <chrono>
#include <fstream>
#include <gpiod.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

const std::string DEVICE_DIR = "/sys/bus/iio/devices/iio:device0/";
const std::string FILE_RAW = DEVICE_DIR + "in_voltage0_raw";
const std::string FILE_SCALE = DEVICE_DIR + "in_voltage0_scale";

#define SENSOR_THRESHOLD 8
#define DEBOUNCE_MS 10 //periodo entre leituras

class GPIO {
	private:
		gpiod_chip *chip;

		// --------- Strobo ---------

		gpiod_line *stroboLine;
		uint8_t PinStrobo;

		// --------- Encoder ---------
		/*
		std::atomic<int> encoderPulses{0};
		std::thread encoderThread;
		gpiod_line *encoderLine;
		gpiod_line *stepLine;
		*/

		// --------- Sensor ---------

		std::ifstream fsRaw;

		double scale;
		bool stableState = false;
		bool reported = false;
		bool lastLogicalState = false;
		std::chrono::steady_clock::time_point lastStateChange;

		// --------- Buzzer ---------

		gpiod_line *buzzerLine;
		uint8_t PinBuzzer;
		bool runningBuzzerThread = false;
		std::thread buzzerThread;

	public:
		// Flags
		bool firstRead = true;

		GPIO(uint8_t pinStrobo, uint8_t pinBuzzer, const std::string &chipname = "gpiochip4");
		~GPIO();

		// --------- Sensor LDR ---------

		/**
		 * @brief Lê o estado do sensor LDR, aplicando filtragem e debounce.
		 * 
		 * @return true se o sensor estiver ativado (condição de detecção), false caso contrário.
		 */
		bool ReadSensor();

		/**
		 * @brief Lê o valor bruto do sensor LDR.
		 */
		int ReadRaw();

		// --------- Strobo ---------

		/**
		 * @brief Emite um pulso no pino do strobo.
		 */
		void OutStrobo();

		/**
		 * @brief Define o pino do strobo como HIGH por um período específico.
		 * 
		 * @param sleep Tempo em milissegundos para manter o pino em HIGH. O padrão é 0, o que significa que o pino será definido como HIGH e permanecerá assim até que SetStroboLow() seja chamado.
		 * @important Certifique-se de chamar SetStroboLow() após o período desejado para evitar que o pino fique permanentemente HIGH.
		 */
		void SetStroboHigh(int sleep = 0);

		/**
		 * @brief Define o pino do strobo como LOW por um período específico.
		 * 
		 * @param sleep Tempo em milissegundos para manter o pino em LOW. O padrão é 0, o que significa que o pino será definido como LOW e permanecerá assim até que SetStroboHigh() seja chamado.
		 * @important Certifique-se de chamar SetStroboHigh() antes de chamar este método para evitar que o pino fique permanentemente LOW.
		 */
		void SetStroboLow(int sleep = 0);

		/**
		 * @brief Reseta o estado do sensor para a condição inicial, forçando uma nova leitura completa.
		 */
		void ReturnToFirst() {
			firstRead = true;
			stableState = false;
			lastLogicalState = false;
		}

		// --------- Encoder ---------

		// int GetAndResetEncoderPulses();
		// void MonitorEncoder();

		// --------- Buzzer ---------

		/**
		 * @brief Emite um beep do buzzer por uma duração específica.
		 * 
		 * @param duration_ms Duração do beep em milissegundos.
		 */
		void beep(uint16_t duration_ms);

		/**
		 * @brief Função que roda em uma thread para controlar o buzzer.
		 * 
		 * @param duration_ms Duração do beep em milissegundos.
		 */
		void buzzerRun(uint16_t duration_ms);

		/**
		 * @brief Inicia a thread do buzzer.
		 * 
		 * @param duration_ms Duração do beep em milissegundos.
		 */
		void startBuzzerThread(uint16_t duration_ms);

		/**
		 * @brief Para a thread do buzzer.
		 */
		void stopBuzzerThread();
};

#endif // GPIO_H
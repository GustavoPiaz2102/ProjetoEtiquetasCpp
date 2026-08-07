#include "../heaters/GPIO.h"
#include <cstring> // Para strerror
#include <errno.h> // Para errno

// --------- GPIO ---------

GPIO::GPIO(const std::string &chipname) : chipName(chipname), chip(nullptr) {
	chip = gpiod_chip_open_by_name(chipname.c_str());
	if (!chip)
		throw std::runtime_error("Erro ao abrir o chip GPIO: " + chipname + " (" + strerror(errno) + ")");

	std::cout << "[INIT] GPIO chip aberto: " << chipname << std::endl;
}

GPIO::~GPIO() {
	if (chip)
		gpiod_chip_close(chip);
}

// --------- Sensor ---------

Sensor::Sensor(const std::string &chipname) : GPIO(chipname) {
	std::ifstream fs(FILE_SCALE);
	if (fs.is_open()) {
		fs >> scale;
		fs.close();
		std::cout << "[INIT] Escala detectada: " << scale << " mV/unidade\n";
	} else {
		scale = 0.1875;
		std::cerr << "[AVISO] Nao foi possivel ler escala. Usando padrao: " << scale << "\n";
	}

	fsRaw.open(FILE_RAW);
	if (!fsRaw.is_open())
		std::cerr << "[ERRO] Nao foi possivel abrir o arquivo do sensor: " << FILE_RAW << "\n";
}

Sensor::~Sensor() {
	if (fsRaw.is_open())
		fsRaw.close();
}

int Sensor::ReadRaw() {
	if (!fsRaw.is_open())
		return -1;

	fsRaw.seekg(0);
	int value = 0;
	fsRaw >> value;

	if (fsRaw.fail()) {
		fsRaw.clear();
		return -1;
	}
	//std::cout << "raw: " << value << "\n";
	return value;
}

bool Sensor::ReadSensor() {
	int rawValue = ReadRaw();
	if (rawValue < 0)
		return stableState;

	bool currentLogicalState = (rawValue > SENSOR_THRESHOLD);
	auto now = std::chrono::steady_clock::now();

	if (firstRead) {
		lastLogicalState = currentLogicalState;
		lastStateChange = now;
		stableState = lastLogicalState;
		reported = false;
		firstRead = false;
		return stableState;
	}

	if (currentLogicalState != lastLogicalState) {
		lastStateChange = now;
		lastLogicalState = currentLogicalState;
	}

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStateChange).count();
	if (elapsed >= DEBOUNCE_MS) {
		stableState = lastLogicalState;

		if (stableState && !reported) {
			reported = true;
			return true;
		}
		if (!stableState)
			reported = false;
	}

	return false;
}

// --------- Strobo ---------

Strobo::Strobo(uint8_t pinStrobo, const std::string &chipname) : GPIO(chipname), pinStrobo(pinStrobo) {
	stroboLine = gpiod_chip_get_line(chip, pinStrobo);
	if (!stroboLine) {
		throw std::runtime_error("Erro: Nao foi possivel obter a linha do strobo " + std::to_string(pinStrobo));
	}

	if (gpiod_line_request_output(stroboLine, "projeto_etiquetas", 0) < 0) {
		std::string erro_msg = strerror(errno);
		throw std::runtime_error("Erro ao configurar output do strobo (GPIO " + std::to_string(pinStrobo) + "): " + erro_msg);
	}
}

Strobo::~Strobo() {
	if (stroboLine)
		gpiod_line_release(stroboLine);
}

void Strobo::OutStrobo() {
	if (!stroboLine)
		return;

	gpiod_line_set_value(stroboLine, 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	gpiod_line_set_value(stroboLine, 0);
}

void Strobo::SetStroboHigh(int sleep) {
	if (stroboLine)
		gpiod_line_set_value(stroboLine, 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}

void Strobo::SetStroboLow(int sleep) {
	if (stroboLine)
		gpiod_line_set_value(stroboLine, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}

// ---------- Buzzer ----------

Buzzer::Buzzer(uint8_t pinBuzzer, const std::string &chipname) : GPIO(chipname), pinBuzzer(pinBuzzer) {
	buzzerLine = gpiod_chip_get_line(chip, pinBuzzer);
	if (!buzzerLine)
		throw std::runtime_error("Erro: Nao foi possivel obter a linha do buzzer " + std::to_string(pinBuzzer));

	if (gpiod_line_request_output(buzzerLine, "projeto_etiquetas", 0) < 0) {
		std::string erro_msg = strerror(errno);
		throw std::runtime_error("Erro ao configurar output do buzzer (GPIO " + std::to_string(pinBuzzer) + "): " + erro_msg);
	}

	this->startBuzzerThread(500);
}

Buzzer::~Buzzer() {
	stopBuzzerThread();
	if (buzzerLine)
		gpiod_line_release(buzzerLine);
}

void Buzzer::beep(uint16_t duration_ms) {
	if (buzzerLine)
		gpiod_line_set_value(buzzerLine, 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
	if (buzzerLine)
		gpiod_line_set_value(buzzerLine, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
}

void Buzzer::buzzerRun(uint16_t duration_ms) {
	if (!buzzerLine)
		return;

	while (runningBuzzerThread && this->isBuzzerActive()) {
		this->beep(duration_ms);
	}
}

void Buzzer::setBuzzerActive(bool active) {
	std::lock_guard<std::mutex> lock(buzzerMutex);
	buzzerActive.store(active);
}

bool Buzzer::isBuzzerActive() {
	std::lock_guard<std::mutex> lock(buzzerMutex);
	return buzzerActive.load();
}

void Buzzer::startBuzzerThread(uint16_t duration_ms) {
	if (runningBuzzerThread)
		return;

	runningBuzzerThread = true;
	buzzerThread = std::thread(&Buzzer::buzzerRun, this, duration_ms);
}

void Buzzer::stopBuzzerThread() {
	if (!runningBuzzerThread)
		return;

	runningBuzzerThread = false;
	if (buzzerThread.joinable())
		buzzerThread.join();
}

// --------- Encoder ---------
/*
Encoder::Encoder(const std::string &chipname) : GPIO(chipname) {
	encoderLine = gpiod_chip_get_line(chip, 17); // GPIO 17
	stepLine = gpiod_chip_get_line(chip, 27);    // GPIO 27

	if (!encoderLine || !stepLine)
		throw std::runtime_error("Erro: Nao foi possivel obter as linhas do encoder (GPIO 17 e 27)");

	if (gpiod_line_request_both_edges_events(encoderLine, "projeto_etiquetas") < 0)
		throw std::runtime_error("Erro ao configurar eventos do encoder (GPIO 17)");

	if (gpiod_line_request_output(stepLine, "projeto_etiquetas", 0) < 0)
		throw std::runtime_error("Erro ao configurar output do step (GPIO 27)");

	encoderThread = std::thread(&Encoder::MonitorEncoder, this);
}

~Encoder::~Encoder() {
	if (encoderThread.joinable())
		encoderThread.detach();

	if (encoderLine)
		gpiod_line_release(encoderLine);
	if (stepLine)
		gpiod_line_release(stepLine);
}

int Encoder::GetAndResetEncoderPulses() {
	int current = encoderPulses.load();
	encoderPulses.store(0);
	return current;
}

void Encoder::MonitorEncoder() {
	while(true){
		if(gpiod_line_event_wait(encoderLine, NULL) == 1){
			gpiod_line_event event;
			if(gpiod_line_event_read(encoderLine, &event) == 0) if(event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) encoderPulses++;
		}
	}
}
*/
#include "../heaters/GPIO.h"

GPIO::GPIO(int pinStrobo, const std::string &chipname): PinStrobo(pinStrobo), chip(nullptr), stroboLine(nullptr)
{
	chip = gpiod_chip_open_by_name(chipname.c_str());
	if (!chip) {
		throw std::runtime_error("Erro ao abrir o chip GPIO: " + chipname);
	}
	std::cout << "[INIT] GPIO chip aberto: " << chipname << ", pino strobo: " << PinStrobo << "";
	stroboLine = gpiod_chip_get_line(chip, PinStrobo);
	if (!stroboLine || gpiod_line_request_output(stroboLine, "gpio_strobo", 0) < 0) {
		gpiod_chip_close(chip);
		throw std::runtime_error("Erro ao configurar linha do strobo (GPIO " + std::to_string(PinStrobo) + ")");
	}

	std::ifstream fs(FILE_SCALE);
	if (fs.is_open()) {
		fs >> scale;
		fs.close();
		std::cout << "[INIT] Escala detectada: " << scale << " mV/unidade\n";
	} else {
		scale = 0.1875; // Valor padrão comum para ganho +/- 6.144V
		std::cerr << "[AVISO] Nao foi possivel ler escala. Usando padrao: " << scale << "\n";
	}
}
int GPIO::ReadRaw() {
	std::ifstream fs(FILE_RAW);
	int value = 0;

	if (fs.is_open()) {
		fs >> value;
		fs.close();
		if(value < 0) return -1;
		return value;
	}
	return -1; // Erro
}

GPIO::~GPIO() {
	if (chip) {
		gpiod_chip_close(chip);
	}
}

bool GPIO::ReadSensor() {
	int rawValue = ReadRaw();

	if (rawValue < 0) {
		std::cout << "Sensor com erro!!"<<std::endl;
	return false
	}

	bool currentLogicalState = (abs(rawValue - lastValidatedRaw) > SENSOR_THRESHOLD);

	if (firstRead) {
		std::cerr << "Primeira leitura analógica: " << rawValue 
				  << " (Estado: " << currentLogicalState << ")\n";
		LastSensorState = currentLogicalState;
		firstRead = false;
	lastValidatedRaw = rawValue;
		return currentLogicalState;
	}

	if (currentLogicalState != LastSensorState) {
		ActualCounter++;
	} else {
		ActualCounter = 0;
	}
	
	if (ActualCounter >= DebounceValue) {
		std::cout << "Leitura Válida do Sensor (Raw: " << rawValue << ")\n";
		ActualCounter = 0;
		LastSensorState = currentLogicalState;
	lastValidatedRaw = rawValue;
		return currentLogicalState;
	}

	return false; 
}


void GPIO::OutStrobo() {
	if (gpiod_line_set_value(stroboLine, 1) < 0) return;
	if (gpiod_line_set_value(stroboLine, 0) < 0) return;
}

void GPIO::BlinkStrobo(int Delay) {
	gpiod_line_set_value(stroboLine, 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(Delay));
	gpiod_line_set_value(stroboLine, 0);
}

void GPIO::SetStroboHigh() {
	gpiod_line_set_value(stroboLine, 1);
}

void GPIO::SetStroboLow() {
	gpiod_line_set_value(stroboLine, 0);
}

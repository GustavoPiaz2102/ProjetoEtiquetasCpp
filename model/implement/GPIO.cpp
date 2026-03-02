#include "../heaters/GPIO.h"
#include <cstring>   // Para strerror
#include <errno.h>   // Para errno

GPIO::GPIO(int pinStrobo, const std::string &chipname) : PinStrobo(pinStrobo), chip(nullptr), stroboLine(nullptr){
	// 1. Abre o chip
	chip = gpiod_chip_open_by_name(chipname.c_str());
	if (!chip) throw std::runtime_error("Erro ao abrir o chip GPIO: " + chipname + " (" + strerror(errno) + ")");

	std::cout << "[INIT] GPIO chip aberto: " << chipname << ", pino strobo: " << PinStrobo << std::endl;

	// 2. Obtém a linha
	stroboLine = gpiod_chip_get_line(chip, PinStrobo);
	if (!stroboLine) {
		gpiod_chip_close(chip);
		throw std::runtime_error("Erro: Nao foi possivel obter a linha " + std::to_string(PinStrobo));
	}

	// 3. Requisita a linha como SAÍDA
	if (gpiod_line_request_output(stroboLine, "projeto_etiquetas", 0) < 0){
		std::string erro_msg = strerror(errno);
		gpiod_chip_close(chip);
		throw std::runtime_error("Erro ao configurar output do strobo (GPIO " + std::to_string(PinStrobo) + "): " + erro_msg);
	}

	// 4. Carrega escala (Leitura de arquivos do sistema)
	std::ifstream fs(FILE_SCALE);
	if (fs.is_open()){
		fs >> scale;
		fs.close();
		std::cout << "[INIT] Escala detectada: " << scale << " mV/unidade\n";
	} else{
		scale = 0.1875; 
		std::cerr << "[AVISO] Nao foi possivel ler escala. Usando padrao: " << scale << "\n";
	}

	fsRaw.open(FILE_RAW);
	if(!fsRaw.is_open()) std::cerr << "[ERRO] Nao foi possivel abrir o arquivo do sensor: " << FILE_RAW << "\n";

	// Encoder

	// EncoderThread = std::thread(&GPIO::MonitorEncoder, this);
}

GPIO::~GPIO(){
	if(fsRaw.is_open()) fsRaw.close();
	if(stroboLine) gpiod_line_release(stroboLine);
	if(chip) gpiod_chip_close(chip);
	// if(encoderThread.joinable()) encoderThread.detach();
}

int GPIO::ReadRaw(){
	if(!fsRaw.is_open()) return -1;

	fsRaw.seekg(0);
	int value = 0;
	fsRaw >> value;	

	if(fsRaw.fail()){
		fsRaw.clear();
		return -1;
	}

	return value;
}

bool GPIO::ReadSensor() {
	int rawValue = ReadRaw();
	if (rawValue < 0) return stableState;

	if (firstRead) {
		smoothedValue = rawValue;
		stableState = (rawValue > SENSOR_THRESHOLD);
		lastLogicalState = stableState;
		lastStateChange = std::chrono::steady_clock::now();
		firstRead = false;
		return true;
	}
	smoothedValue = (FILTER_ALPHA * rawValue) + (1.0 - FILTER_ALPHA) * smoothedValue; //média móvel exponencial

	bool currentLogicalState = lastLogicalState;
	if (smoothedValue > (SENSOR_THRESHOLD + SENSOR_HYSTERESIS)) {
		currentLogicalState = true;
	} else if (smoothedValue < (SENSOR_THRESHOLD - SENSOR_HYSTERESIS)) {
		currentLogicalState = false;
	}

	auto now = std::chrono::steady_clock::now();

	if (currentLogicalState != lastLogicalState) {
		lastStateChange = now;
		lastLogicalState = currentLogicalState;
	} else {
		if (currentLogicalState != stableState) {
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStateChange).count();
			
			if (elapsed >= DEBOUNCE_MS) {
				stableState = currentLogicalState;
			}
		}
	}
	std::cout<< "Estado Detectado: " << stableState << "\n";
	if(stableState!=lastSensorState){
		lastSensorState=stableState;
		return true;
	}
	else return false;
}

/*
// Leitura do encoder
int GPIO::GetAndResetEncoderPulses() {
	int current = encoderPulses.load();
	encoderPulses.store(0);
	return current;
}

void GPIO::MonitorEncoder() {
	while(true){
		if(gpiod_line_event_wait(encoderLine, NULL) == 1){
			gpiod_line_event event;
			if(gpiod_line_event_read(encoderLine, &event) == 0) if(event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) encoderPulses++;
		}
	}
}
*/

// Comandos de controle do Strobo
void GPIO::OutStrobo(){
	if(!stroboLine) return;
	gpiod_line_set_value(stroboLine, 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	gpiod_line_set_value(stroboLine, 0);
}

void GPIO::SetStroboHigh(int sleep){
	if(stroboLine) gpiod_line_set_value(stroboLine, 1);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}

void GPIO::SetStroboLow(int sleep) {
	if (stroboLine) gpiod_line_set_value(stroboLine, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}
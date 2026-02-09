#include "../heaters/GPIO.h"
#include <cstring>   // Para strerror
#include <errno.h>   // Para errno

GPIO::GPIO(int pinStrobo, const std::string &chipname) 
    : PinStrobo(pinStrobo), chip(nullptr), stroboLine(nullptr) 
{
    // 1. Abre o chip
    chip = gpiod_chip_open_by_name(chipname.c_str());
    if (!chip) {
        throw std::runtime_error("Erro ao abrir o chip GPIO: " + chipname + 
                                 " (" + strerror(errno) + ")");
    }

    std::cout << "[INIT] GPIO chip aberto: " << chipname << ", pino strobo: " << PinStrobo << std::endl;

    // 2. Obtém a linha
    stroboLine = gpiod_chip_get_line(chip, PinStrobo);
    if (!stroboLine) {
        gpiod_chip_close(chip);
        throw std::runtime_error("Erro: Nao foi possivel obter a linha " + std::to_string(PinStrobo));
    }

    // 3. Requisita a linha como SAÍDA
    // Nota: REMOVIDO o gpiod_line_release daqui. A linha deve ficar "presa" ao objeto.
    if (gpiod_line_request_output(stroboLine, "projeto_etiquetas", 0) < 0) {
        std::string erro_msg = strerror(errno);
        gpiod_chip_close(chip);
        throw std::runtime_error("Erro ao configurar output do strobo (GPIO " + 
                                 std::to_string(PinStrobo) + "): " + erro_msg);
    }

    // 4. Carrega escala (Leitura de arquivos do sistema)
    std::ifstream fs(FILE_SCALE);
    if (fs.is_open()) {
        fs >> scale;
        fs.close();
        std::cout << "[INIT] Escala detectada: " << scale << " mV/unidade\n";
    } else {
        scale = 0.1875; 
        std::cerr << "[AVISO] Nao foi possivel ler escala. Usando padrao: " << scale << "\n";
    }
}

GPIO::~GPIO() {
    // No destrutor, liberamos a linha antes de fechar o chip para evitar 'Device busy' depois
    if (stroboLine) {
        gpiod_line_release(stroboLine);
    }
    if (chip) {
        gpiod_chip_close(chip);
    }
}

int GPIO::ReadRaw() {
    std::ifstream fs(FILE_RAW);
    int value = 0;
    if (fs.is_open()) {
        fs >> value;
        fs.close();
        return (value < 0) ? -1 : value;
    }
    return -1;
}

bool GPIO::ReadSensor() {
    int rawValue = ReadRaw();
    if (rawValue < 0) {
        std::cout << "Sensor com erro!!" << std::endl;
        return false;
    }

    bool currentLogicalState = (rawValue > SENSOR_THRESHOLD);

    if (firstRead) {
        std::cerr << "Primeira leitura analógica: " << rawValue 
                  << " (Estado: " << currentLogicalState << ")\n";
        LastSensorState = currentLogicalState;
		LastDetectedTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
        firstRead = false;
        return true;
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
		std::cout << "Tempo desde a última detecção: " << (std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count() - LastDetectedTime) << " segundos\n";
		LastDetectedTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
        return currentLogicalState;
    }
    return false; 
}

// Comandos de controle do Strobo
void GPIO::OutStrobo() {
    if (!stroboLine) return;
    gpiod_line_set_value(stroboLine, 1);
    gpiod_line_set_value(stroboLine, 0);
}

void GPIO::BlinkStrobo(int Delay) {
    if (!stroboLine) return;
    gpiod_line_set_value(stroboLine, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(Delay));
    gpiod_line_set_value(stroboLine, 0);
}

void GPIO::SetStroboHigh() {
    if (stroboLine) gpiod_line_set_value(stroboLine, 1);
}

void GPIO::SetStroboLow() {
    if (stroboLine) gpiod_line_set_value(stroboLine, 0);
}
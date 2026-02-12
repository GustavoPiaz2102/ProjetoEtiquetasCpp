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
}

GPIO::~GPIO(){
    if(fsRaw.is_open()) fsRaw.close();
    if(stroboLine) gpiod_line_release(stroboLine);
    if(chip) gpiod_chip_close(chip);
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

bool GPIO::ReadSensor(){
    int rawValue = ReadRaw();
    if (rawValue < 0) return false;

    bool currentLogicalState = (rawValue > SENSOR_THRESHOLD);

    if(firstRead){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        LastSensorState = currentLogicalState;
        firstRead = false;
        return true;
    }

    if(currentLogicalState != LastSensorState) {
        ActualCounter++;

        if (ActualCounter >= DebounceValue){
            LastSensorState = currentLogicalState;
            ActualCounter = 0;
            return currentLogicalState;
        }
    } else ActualCounter = 0;
    
    return false; 
}

// Comandos de controle do Strobo
void GPIO::OutStrobo(){
    if(!stroboLine) return;
    gpiod_line_set_value(stroboLine, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    gpiod_line_set_value(stroboLine, 0);
}

void GPIO::SetStroboHigh(int sleep = 0){
    if(stroboLine) gpiod_line_set_value(stroboLine, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}

void GPIO::SetStroboLow(int sleep = 0) {
    if (stroboLine) gpiod_line_set_value(stroboLine, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
}
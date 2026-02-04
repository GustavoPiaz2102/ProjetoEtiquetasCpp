#include "../heaters/GPIO.h"

GPIO::GPIO(int pinStrobo, const std::string &chipname)
    : PinStrobo(pinStrobo), chip(nullptr), stroboLine(nullptr)
{
    chip = gpiod_chip_open_by_name(chipname.c_str());
    if (!chip) {
        throw std::runtime_error("Erro ao abrir o chip GPIO: " + chipname);
    }


    stroboLine = gpiod_chip_get_line(chip, PinStrobo);
    if (!stroboLine || gpiod_line_request_output(stroboLine, "gpio_strobo", 0) < 0) {
        gpiod_chip_close(chip);
        throw std::runtime_error("Erro ao configurar linha do strobo (GPIO " + std::to_string(PinStrobo) + ")");
    }
}

GPIO::~GPIO() {
    if (chip) {
        gpiod_chip_close(chip);
    }
}

int GPIO::ReadADC_Raw() {
    std::ifstream fs(ADC_FILE_PATH);
    int value = -1;
    if (fs.is_open()) {
        fs >> value;
        fs.close();
    } else {
    }
    return value;
}

bool GPIO::ReadSensor() {
    int rawValue = ReadADC_Raw();

    if (rawValue < 0) {
        return false; 
    }

    bool currentLogicalState = (rawValue > SENSOR_THRESHOLD);

    if (firstRead) {
        std::cerr << "Primeira leitura analógica: " << rawValue 
                  << " (Estado: " << currentLogicalState << ")\n";
        LastSensorState = currentLogicalState;
        firstRead = false;
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
#include "../heaters/GPIO.h"

GPIO::GPIO(int pinSensor, int pinStrobo, const std::string &chipname)
    : PinSensor(pinSensor), PinStrobo(pinStrobo), chip(nullptr), sensorLine(nullptr), stroboLine(nullptr)
{
    chip = gpiod_chip_open_by_name(chipname.c_str());
    if (!chip) {
        throw std::runtime_error("Erro ao abrir o chip GPIO: " + chipname);
    }

    sensorLine = gpiod_chip_get_line(chip, PinSensor);
    if (!sensorLine || gpiod_line_request_input(sensorLine, "gpio_sensor") < 0) {
        gpiod_chip_close(chip);
        throw std::runtime_error("Erro ao configurar linha do sensor (GPIO " + std::to_string(PinSensor) + ")");
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

bool GPIO::ReadSensor() {
    int value = gpiod_line_get_value(sensorLine);
    if (value < 0) {
        std::cerr << "Erro ao ler valor do sensor (GPIO " << PinSensor << ")\n";
        return false;
    }
    return value;
}

void GPIO::OutStrobo() {
    if (gpiod_line_set_value(stroboLine, 1) < 0) {
        std::cerr << "Erro ao ativar strobo (GPIO " << PinStrobo << ")\n";
        return;
    }

    if (gpiod_line_set_value(stroboLine, 0) < 0) {
        std::cerr << "Erro ao desativar strobo (GPIO " << PinStrobo << ")\n";
    }
void GPIO::BlinkStrobo(int Delay) {
    if (gpiod_line_set_value(stroboLine, 1) < 0) {
        std::cerr << "Erro ao ativar strobo (GPIO " << PinStrobo << ")\n";
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(Delay));
    if (gpiod_line_set_value(stroboLine, 0) < 0) {
        std::cerr << "Erro ao desativar strobo (GPIO " << PinStrobo << ")\n";
    }
}

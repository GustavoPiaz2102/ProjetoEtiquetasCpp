#include "../heaters/hardwarePWM.h"

void HardwarePWM::writeToFile(const std::string& path, const std::string& value) {
    std::ofstream fs(path);
    if (!fs.is_open()) throw std::runtime_error("Erro ao abrir: " + path);
    fs << value;
    fs.close();
}

HardwarePWM::HardwarePWM(int chip, int channel) {
    std::string chipPath = "/sys/class/pwm/pwmchip" + std::to_string(chip);
    pwmPath = chipPath + "/pwm" + std::to_string(channel);

    // Exporta o canal se ainda não estiver exportado
    std::ifstream f(pwmPath);
    if (!f.good()) {
        writeToFile(chipPath + "/export", std::to_string(channel));
    }
}

void HardwarePWM::setFrequency(uint32_t freq_hz) {
    if (freq_hz == 0) {
        enable(false);
        return;
    }

    // Converte Frequência (Hz) para Período em Nanosegundos
    period_ns = 1000000000 / freq_hz;
    int duty_ns = period_ns / 2; // 50% de duty cycle é perfeito para o pino STEP

    // O kernel exige que o duty cycle seja atualizado dependendo se o período aumenta ou diminui
    writeToFile(pwmPath + "/duty_cycle", "0"); 
    writeToFile(pwmPath + "/period", std::to_string(period_ns));
    writeToFile(pwmPath + "/duty_cycle", std::to_string(duty_ns));
}

void HardwarePWM::enable(bool state) {
    writeToFile(pwmPath + "/enable", state ? "1" : "0");
}
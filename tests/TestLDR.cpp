#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>
#include <cmath>

// Caminhos baseados no seu "ls"
const std::string DEVICE_DIR = "/sys/bus/iio/devices/iio:device0/";
const std::string FILE_RAW   = DEVICE_DIR + "in_voltage0_raw";   
const std::string FILE_SCALE = DEVICE_DIR + "in_voltage0_scale"; 

class ADS1115Sensor {
private:
double scale; // Armazena o fator de conversão (mV por unidade)

public:
ADS1115Sensor() {
// 1. Ler a escala (Scale) uma única vez na inicialização
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

int ReadRaw() {
std::ifstream fs(FILE_RAW);
int value = 0;
if (fs.is_open()) {
fs >> value;
fs.close();
return value;
}
return -1; // Erro
}

double GetVoltage(int rawValue) {
// O valor da escala geralmente vem em milivolts (mV)
return (rawValue * scale) / 1000.0; // Converte para Volts
}
};

int main() {
std::cout << "--- Iniciando Monitor ADS1115 (Pino A0) ---\n";

ADS1115Sensor sensor;

std::cout << "Lendo continuamente... (Ctrl+C para sair)\n\n";

while (true) {
int raw = sensor.ReadRaw();

if (raw != -1) {
double voltage = sensor.GetVoltage(raw);

// --- Formatação da Saída ---
// Limpa a linha atual (\r) e imprime os novos dados
std::cout << "\r"
<< "Raw: " << std::setw(6) << raw
<< " | Volts: " << std::fixed << std::setprecision(3) << voltage << "V ";

// --- Barra Gráfica ---
std::cout << "|";

// Define o tamanho da barra.
// O ADS1115 em modo single-ended vai até ~26000 (dependendo do ganho)
// Vamos dividir por 500 para ter uma barra de ~50 caracteres
int bars = raw / 500;

if (bars < 0) bars = 0;   // Evita crash se leitura for negativa
if (bars > 60) bars = 60; // Limite visual da tela

for (int i = 0; i < bars; ++i) std::cout << "#";

// Preenche o resto com espaço vazio para limpar caracteres antigos
for (int i = bars; i < 60; ++i) std::cout << " ";

std::cout << "|" << std::flush;
} else {
std::cout << "\r[ERRO] Falha na leitura do arquivo raw." << std::flush;
}

// Taxa de atualização (100ms = 10 leituras por segundo)
std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

return 0;
}
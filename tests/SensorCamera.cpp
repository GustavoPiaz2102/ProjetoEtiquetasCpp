#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#define DebounceValue 2

int main(){
    // --- Configuração GPIO ---
    gpiod_chip *chip;
    gpiod_line *sensorLine;
    int PinSensor = 17;

    chip = gpiod_chip_open_by_name("gpiochip4");
    if (!chip) {
        std::cerr << "Erro GPIO Chip" << "\n";
        return -1;
    }
    sensorLine = gpiod_chip_get_line(chip, PinSensor);
    if (!sensorLine || gpiod_line_request_input(sensorLine, "gpio_sensor") < 0) {
        gpiod_chip_close(chip);
        std::cerr << "Erro GPIO Line" << "\n";
        return -1;
    }

    bool LastSensorValue = false;
    int ActualCounter = 0;
    
    cv::Mat frame;      
    cv::Mat grayFrame;  

    // Pipeline estável 640x480
    std::string pipeline = "libcamerasrc ! videoconvert ! videoscale ! video/x-raw, width=640, height=480, format=BGR ! appsink drop=1";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera." << std::endl;
        return -1;
    }

    std::cout << "--- SISTEMA PRONTO (AGUARDANDO SENSOR) ---" << "\n";
    std::cout << "A imagem so aparecera quando o sensor for ativado." << "\n";

    while (true) {
        // 1. LEITURA SILENCIOSA (OBRIGATÓRIO)
        // Precisamos ler sempre para jogar fora os frames velhos e manter a câmera "quente".
        // Se tirarmos isso daqui, a foto sairá com atraso.
        if (!cap.read(frame) || frame.empty()) {
            // Se der erro de leitura, tentamos continuar
            continue; 
        }

        // 2. LÓGICA DO SENSOR
        int value = gpiod_line_get_value(sensorLine);
        
        if (value != LastSensorValue) ActualCounter++;
        else ActualCounter = 0;

        if (ActualCounter >= DebounceValue) {
            if (value != LastSensorValue) { 
                LastSensorValue = value;
                
                // Se o sensor ATIVOU (1)
                if (value == 1) { 
                    std::cout << "[SENSOR] Captura!" << std::endl;
                    
                    // Converte para cinza para OCR (opcional)
                    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

                    // 3. EXIBIÇÃO (SÓ AQUI)
                    // Mostra a janela apenas neste momento
                    cv::imshow("Ultima Leitura do Sensor", frame);
                    
                    // O waitKey aqui serve para atualizar a janela gráfica e mantê-la desenhada
                    cv::waitKey(1); 
                    
                    // Se quiser que a janela feche sozinha depois de um tempo, 
                    // ou fique congelada na última foto, a lógica está feita.
                    // Atualmente ela fica mostrando a última foto até vir a próxima.
                }
            }
        }
        
        // Pequena pausa para não fritar a CPU, mas curta o suficiente para não encher o buffer da câmera
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    gpiod_chip_close(chip);
    cv::destroyAllWindows();
    return 0;
}
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
    bool firstRead = true;
    
    cv::Mat frame;      // Imagem colorida (vinda da câmera)
    cv::Mat grayFrame;  // Imagem em tons de cinza (para processamento)

    // PIPELINE BLINDADO (Testado e Aprovado)
    std::string pipeline = "libcamerasrc ! videoconvert ! videoscale ! video/x-raw, width=640, height=480, format=BGR ! appsink drop=1";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera com o pipeline GStreamer." << std::endl;
        return -1;
    }

    std::cout << "--- INICIANDO SISTEMA DE ETIQUETAS ---" << "\n";

    while (true) {
        // --- BLOC 1: DIAGNÓSTICO INICIAL ---
        if (firstRead) {
            firstRead = false;
            // Lê alguns frames para limpar o buffer e estabilizar a luz
            for(int i=0; i<5; i++) cap.read(frame);

            if(cap.read(frame)) {
                std::cout << "Primeiro Frame Capturado!" << "\n";
                std::cout << "Resolução: " << frame.cols << "x" << frame.rows << "\n";
                std::cout << "Canais: " << frame.channels() << "\n";
                
                // Agora verificamos se é 3 canais (BGR), pois é isso que o pipeline entrega
                if (frame.cols == 640 && frame.channels() == 3) {
                    std::cout << "STATUS: SUCESSO! Câmera operante. Pronto para o sensor." << "\n";
                } else {
                    std::cout << "STATUS: ALERTA! Formato inesperado (" << frame.channels() << " canais)." << "\n";
                }
            } else {
                std::cerr << "Erro: Não foi possível ler o primeiro frame." << "\n";
            }
        } 
        // --- BLOCO 2: LEITURA DO SENSOR ---
        else {
            int value = gpiod_line_get_value(sensorLine);
            
            // Lógica de borda (detecta mudança)
            if (value != LastSensorValue) ActualCounter++;
            else ActualCounter = 0;

            if (ActualCounter >= DebounceValue) {
                // Atualiza o estado apenas após debounce
                if (value != LastSensorValue) { 
                    LastSensorValue = value;

                    // Se o sensor foi ATIVADO (assumindo 1 como ativo, ajuste se for 0)
                    if (value == 1) { 
                        if (cap.read(frame) && !frame.empty()) {
                            
                            // CONVERSÃO PARA CINZA (Ideal para OCR/Etiquetas)
                            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

                            std::cout << "[SENSOR] Captura realizada! Processando imagem..." << "\n";

                            // --- AQUI ENTRA SEU CÓDIGO DE OCR ---
                            // Exemplo: processarEtiqueta(grayFrame);
                            
                            // Se quiser salvar para testar:
                            // cv::imwrite("teste_etiqueta.jpg", grayFrame);
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    gpiod_chip_close(chip);
    return 0;
}
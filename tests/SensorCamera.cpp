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
    
    cv::Mat frame;      // Imagem original (640x480)
    cv::Mat grayFrame;  // Imagem final para OCR (640x480)

    // PIPELINE BLINDADO (640x480)
    std::string pipeline = "libcamerasrc ! videoconvert ! videoscale ! video/x-raw, width=640, height=480, format=BGR ! appsink drop=1";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera." << std::endl;
        return -1;
    }

    // Janela com tamanho original
    cv::namedWindow("Live View (640x480)", cv::WINDOW_AUTOSIZE);

    std::cout << "--- SISTEMA PADRAO (640x480) ---" << "\n";

    while (true) {
        // 1. LEITURA
        if (!cap.read(frame) || frame.empty()) {
            std::cerr << "Falha na leitura." << std::endl;
            break;
        }

        // 2. VISUALIZAÇÃO (Mostra a imagem inteira)
        cv::imshow("Live View (640x480)", frame);

        char key = (char)cv::waitKey(1); 
        if (key == 27) break; 

        // 3. LÓGICA DO SENSOR
        int value = gpiod_line_get_value(sensorLine);
        
        if (value != LastSensorValue) ActualCounter++;
        else ActualCounter = 0;

        if (ActualCounter >= DebounceValue) {
            if (value != LastSensorValue) { 
                LastSensorValue = value;

                // Se o sensor ATIVOU (1)
                if (value == 1) { 
                    
                    // PROCESSAMENTO (Usa a imagem inteira de 640x480)
                    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

                    std::cout << "[SENSOR] Captura realizada!" << "\n";
                    std::cout << "Processando imagem completa: " << grayFrame.cols << "x" << grayFrame.rows << "\n";

                    // Mostra o que será enviado para o OCR
                    cv::imshow("Snapshot OCR", grayFrame);
                    cv::waitKey(1); // Atualiza a janela do snapshot
                    
                    // processarEtiqueta(grayFrame);
                }
            }
        }
    }

    gpiod_chip_close(chip);
    cv::destroyAllWindows();
    return 0;
}
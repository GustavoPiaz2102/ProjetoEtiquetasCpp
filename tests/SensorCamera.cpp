#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#define DebounceValue 2

int main(){
    // --- Configuração GPIO (Mantida) ---
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
    cv::Mat frame;

    // --- PIPELINE DE ALTA EFICIÊNCIA PARA OCR ---
    // 1. Limita a 10 FPS na entrada (economia de CPU)
    // 2. Converte para Cinza (GRAY8)
    // 3. Redimensiona para 640x480 usando Nearest Neighbor (method=0) que é ultra rápido
    std::string pipeline = 
        "libcamerasrc ! "
        "video/x-raw,framerate=10/1 ! "
        "videoconvert ! "
        "videoscale method=0 ! "
        "video/x-raw,width=640,height=480,format=GRAY8 ! "
        "appsink drop=true max-buffers=1";

    cv::VideoCapture cap;
    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro ao abrir câmera" << "\n";
        return -1;
    } else {
        std::cout << "Pipeline OCR Iniciado: 640x480 GRAY8 @ 10FPS" << "\n";
    }

    while (true) {
        if (firstRead) {
            firstRead = false;
            cap.read(frame);
        } else {
            int value = gpiod_line_get_value(sensorLine);
            
            if (value != LastSensorValue) ActualCounter++;
            else ActualCounter = 0;

            if (ActualCounter >= DebounceValue) {
                LastSensorValue = value;
                
                // A imagem lida aqui já será 640x480 e em tons de cinza (CV_8UC1)
                // Pronta para ser jogada no Tesseract ou outro motor OCR
                if (cap.read(frame) && !frame.empty()) {
                    
                    // Exemplo: Se for usar Tesseract aqui, o frame já está no formato ideal.
                    
                    // Para DEBUG apenas (Desative em produção para ganhar performance)
                    cv::imshow("OCR Feed (Gray)", frame);
                }
            }
            
            // Se for rodar "headless" (sem monitor), remova o waitKey e o imshow
            if (cv::waitKey(1) == 27) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Folga para a CPU
    }

    gpiod_chip_close(chip);
    return 0;
}
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
    
    cv::Mat fullFrame;  // Imagem original (640x480)
    cv::Mat cropFrame;  // Imagem recortada (320x240)
    cv::Mat grayFrame;  // Imagem final para OCR

    // MANTEMOS O PIPELINE ESTÁVEL (640x480)
    // Se tentarmos mudar o GStreamer agora, pode dar erro de negociação novamente.
    // Faremos o crop via software (OpenCV) que é mais seguro e mantém a qualidade do pixel.
    std::string pipeline = "libcamerasrc ! videoconvert ! videoscale ! video/x-raw, width=640, height=480, format=BGR ! appsink drop=1";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera." << std::endl;
        return -1;
    }

    // Define a área de corte (CENTRO 320x240)
    // Matemática: (640 - 320) / 2 = 160 (X inicial)
    //             (480 - 240) / 2 = 120 (Y inicial)
    cv::Rect roi(160, 120, 320, 240);

    cv::namedWindow("Foco (320x240)", cv::WINDOW_AUTOSIZE);

    std::cout << "--- SISTEMA COM ZOOM DIGITAL (CROP) ---" << "\n";

    while (true) {
        // 1. LEITURA
        if (!cap.read(fullFrame) || fullFrame.empty()) {
            std::cerr << "Falha na leitura." << std::endl;
            break;
        }

        // 2. CROP (Recorte sem perda de qualidade)
        // Isso pega apenas o miolo da imagem, criando efeito de zoom
        cropFrame = fullFrame(roi);

        // 3. VISUALIZAÇÃO
        // Mostramos apenas a parte recortada, que é o que importa
        cv::imshow("Foco (320x240)", cropFrame);

        char key = (char)cv::waitKey(1); 
        if (key == 27) break; 

        // 4. LÓGICA DO SENSOR
        int value = gpiod_line_get_value(sensorLine);
        
        if (value != LastSensorValue) ActualCounter++;
        else ActualCounter = 0;

        if (ActualCounter >= DebounceValue) {
            if (value != LastSensorValue) { 
                LastSensorValue = value;

                // Se o sensor ATIVOU (1)
                if (value == 1) { 
                    
                    // PROCESSAMENTO (Usamos o cropFrame, pois queremos ler a etiqueta centralizada)
                    cv::cvtColor(cropFrame, grayFrame, cv::COLOR_BGR2GRAY);

                    std::cout << "[SENSOR] Captura realizada na area de foco!" << "\n";
                    std::cout << "Processando imagem de dimensoes: " << grayFrame.cols << "x" << grayFrame.rows << "\n";

                    // Se quiser ver a foto exata que foi para o OCR:
                    cv::imshow("Snapshot OCR", grayFrame);
                    cv::waitKey(1);
                    // processarEtiqueta(grayFrame);
                }
            }
        }
    }

    gpiod_chip_close(chip);
    cv::destroyAllWindows();
    return 0;
}
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
    cv::Mat frame;

    // --- PIPELINE CORRIGIDO ---
    // 1. libcamerasrc: O driver vai cuspir 1280x1080 (não temos controle sobre isso aqui).
    // 2. video/x-raw,width=1280,height=1080: Dizemos ao GStreamer "Ok, aceito esse tamanho".
    // 3. videoconvert: Converte o formato de cor (de RGB/YUV para o que o próximo passo precisar).
    // 4. videoscale: AQUI acontece a redução.
    // 5. video/x-raw,width=640,height=480,format=GRAY8: Forçamos a saída final.
    // 6. appsink: Entrega para o C++.
    std::string pipeline = 
        "libcamerasrc ! "
        "video/x-raw,width=1280,height=1080 ! " 
        "videoconvert ! "
        "videoscale ! "
        "video/x-raw,width=640,height=480,format=GRAY8 ! "
        "appsink drop=true max-buffers=1";

    cv::VideoCapture cap;
    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro Fatal: Pipeline falhou!" << "\n";
        return -1;
    } 

    std::cout << "--- INICIANDO DIAGNÓSTICO ---" << "\n";

    while (true) {
        if (firstRead) {
            firstRead = false;
            if(cap.read(frame)) {
                // IMPRIME A PROVA REAL NO TERMINAL
                std::cout << "Primeiro Frame Capturado!" << "\n";
                std::cout << "Resolução: " << frame.cols << "x" << frame.rows << "\n";
                std::cout << "Canais de cor: " << frame.channels() << " (1 = Gray, 3 = Colorido)" << "\n";
                
                if (frame.cols == 640 && frame.channels() == 1) {
                    std::cout << "STATUS: SUCESSO! Imagem está correta para OCR." << "\n";
                } else {
                    std::cout << "STATUS: FALHA! Formato incorreto." << "\n";
                }
            } else {
                std::cerr << "Erro: Não foi possível ler o primeiro frame." << "\n";
            }
        } else {
            int value = gpiod_line_get_value(sensorLine);
            if (value != LastSensorValue) ActualCounter++;
            else ActualCounter = 0;

            if (ActualCounter >= DebounceValue) {
                LastSensorValue = value;
                
                if (cap.read(frame) && !frame.empty()) {
                    // Processamento aqui...
                    
                    // Como você está sem interface gráfica (SSH), comentei o imshow
                    // cv::imshow("Camera", frame);
                }
            }
            // waitKey removido pois não há janela gráfica para gerenciar
            // if (cv::waitKey(1) == 27) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    gpiod_chip_close(chip);
    return 0;
}
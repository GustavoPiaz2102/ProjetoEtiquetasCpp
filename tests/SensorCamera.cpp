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
    
    cv::Mat frame;      // Imagem colorida (Live View)
    cv::Mat grayFrame;  // Imagem processada (Snapshot)

    // PIPELINE BLINDADO RPI 5
    std::string pipeline = "libcamerasrc ! videoconvert ! videoscale ! video/x-raw, width=640, height=480, format=BGR ! appsink drop=1";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera com o pipeline GStreamer." << std::endl;
        return -1;
    }

    // Cria a janela antecipadamente
    cv::namedWindow("Live View", cv::WINDOW_AUTOSIZE);

    std::cout << "--- INICIANDO SISTEMA COM VIDEO ---" << "\n";
    std::cout << "Pressione ESC na janela do vídeo para sair." << "\n";

    while (true) {
        // 1. LEITURA CONTÍNUA (Para ter vídeo fluido)
        if (!cap.read(frame) || frame.empty()) {
            std::cerr << "Falha ao ler frame da câmera." << std::endl;
            break;
        }

        // 2. VISUALIZAÇÃO (SHOW)
        cv::imshow("Live View", frame);

        // O waitKey é OBRIGATÓRIO para o imshow funcionar (desenhar a janela)
        // Se pressionar ESC (27), sai do programa.
        char key = (char)cv::waitKey(1); 
        if (key == 27) break; 

        // 3. LÓGICA DO SENSOR
        int value = gpiod_line_get_value(sensorLine);
        
        if (value != LastSensorValue) ActualCounter++;
        else ActualCounter = 0;

        if (ActualCounter >= DebounceValue) {
            if (value != LastSensorValue) { 
                LastSensorValue = value;

                // Se o sensor ATIVOU (Assumindo 1. Se seu sensor for invertido, mude para 0)
                if (value == 1) { 
                    
                    // PROCESSAMENTO (Tira a "foto" do momento atual)
                    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

                    std::cout << "[SENSOR] Captura! Processando etiqueta..." << "\n";
                    
                    // Exemplo: Mostra o que foi capturado em outra janela, se quiser
                    cv::imshow("Ultima Captura", grayFrame);
                    cv::waitKey(1);
                    
                    // AQUI ENTRA SEU OCR
                    // processarEtiqueta(grayFrame);
                }
            }
        }
        
        // Removemos o sleep longo para o vídeo não ficar "travando"
        // O waitKey(1) já faz um pequeno delay.
    }

    gpiod_chip_close(chip);
    cv::destroyAllWindows(); // Fecha janelas ao sair
    return 0;
}
#include <gpiod.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#define DebounceValue 2
int main(){
    gpiod_chip *chip;
    gpiod_line *sensorLine;
    int PinSensor = 17;

    chip = gpiod_chip_open_by_name("gpiochip4");
    if (!chip) {
        std::cerr << "Erro ao abrir o chip GPIO" << "\n";
        return -1;
    }

    sensorLine = gpiod_chip_get_line(chip, PinSensor);
    if (!sensorLine || gpiod_line_request_input(sensorLine, "gpio_sensor") < 0) {
        gpiod_chip_close(chip);
        std::cerr << "Erro ao configurar linha do sensor (GPIO " << PinSensor << ")" << "\n";
        return -1;
    }

    bool LastSensorValue = false;
    int ActualCounter = 0;
    bool firstRead = true;
    cv::Mat frame;
    std::string pipeline =
    "libcamerasrc ! videoconvert ! video/x-raw,format=BGR,width=640,height=480 ! queue max-size-buffers=1 leaky=downstream ! appsink max-buffers=1 drop=true";
    cv::VideoCapture cap;
    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo pipeline!" << "\n";
        return -1;
    } else {
        std::cout << "Câmera aberta com sucesso pelo pipeline!" << "\n";
    }
    while (true) {
        if (firstRead) {
            firstRead = false;
            cap.read(frame);
        } else {
            int value = gpiod_line_get_value(sensorLine);
            if (value < 0) {
                std::cerr << "Erro ao ler valor do sensor (GPIO " << PinSensor << ")" << "\n";
                break;
            }
            if (value != LastSensorValue) {
                ActualCounter++;
            } else {
                ActualCounter = 0;
            }
            if (ActualCounter >= DebounceValue) {
                LastSensorValue = value;
                cap.read(frame);
            }
            cv::imshow("Frame", frame);
            if (cv::waitKey(1) == 27) { // Sai se a tecla 'Esc' for pressionada
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    gpiod_chip_close(chip);
    return 0;
}
#include "capture.h"
#include <iostream>

Capture::Capture(int cameraIndex) {
    cap.open(cameraIndex);
    if (!cap.isOpened()) {
        std::cerr << "Erro: Não foi possível abrir a câmera!" << std::endl;
    }
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat Capture::captureImage() {
    cv::Mat frame;
    if (cap.isOpened()) {
        cap.read(frame);
        if (frame.empty()) {
            std::cerr << "Erro: Frame capturado está vazio!" << std::endl;
        }
    } else {
        std::cerr << "Erro: Câmera não está aberta!" << std::endl;
    }
    return frame;
}

#include "../heaters/capture.h"
#include <iostream>
#include <opencv2/core/utils/logger.hpp>

Capture::Capture(int cameraIndex)
    : shutter_us(1000) 
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef __linux__
    // Pipeline otimizado para 640x480 e controle de exposição manual
    // exposure-mode=1 é o modo manual no libcamerasrc
    std::string pipeline = "libcamerasrc exposure-mode=1 exposure-time=" + std::to_string(shutter_us) + 
                           " ! video/x-raw, width=640, height=480, framerate=30/1 "
                           " ! videoconvert ! appsink";
    
    std::cout << "Abrindo Pipeline: " << pipeline << std::endl;
    
    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro: Não foi possível abrir o pipeline GStreamer!\n";
    }
#else
    if (!cap.open(cameraIndex)) {
        std::cerr << "Erro: Não foi possível abrir a câmera!\n";
    }
#endif
}

Capture::~Capture() {
    if (cap.isOpened())
        cap.release();
}

void Capture::captureImage() {
    // Agora o grab() apenas limpa o buffer para pegar o frame mais recente
    if (!cap.grab()) {
        std::cerr << "Erro ao capturar frame!\n";
    }
}

cv::Mat Capture::retrieveImage() {
    if (!cap.retrieve(frame)) {
        std::cerr << "Erro ao decodificar frame!\n";
        return cv::Mat(); // Retorna matriz vazia (corrigido o erro anterior)
    }
    return frame;
}
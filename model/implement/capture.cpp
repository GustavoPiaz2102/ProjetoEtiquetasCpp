#include "../heaters/capture.h"
#include <opencv2/core/utils/logger.hpp> // Para desativar logs desnecessários

Capture::Capture(int cameraIndex) : cap(), frame() {
    // Desativa logs verbosos do OpenCV
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    
    // Usa pipeline do libcamera via GStreamer
    std::string pipeline = 
        "libcamerasrc ! video/x-raw,width=" + std::to_string(IMG_SZE) + 
        ",height=" + std::to_string(IMG_SZE2) +
        ",framerate=30/1 ! videoconvert ! appsink";

    cap.open(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Erro: Não foi possível abrir a câmera via libcamera + GStreamer!" << std::endl;
    } else {
        std::cout << "Câmera inicializada com sucesso usando libcamera + GStreamer!" << std::endl;
    }
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat Capture::captureImage() {
    if (!cap.isOpened()) {
        std::cerr << "Erro: Câmera não está aberta!" << std::endl;
        return cv::Mat();
    }

    frame.release();

    // Tenta capturar até 3 vezes antes de desistir
    for (int attempt = 0; attempt < 3; ++attempt) {
        if (cap.grab()) {
            if (cap.retrieve(frame)) {
                if (!frame.empty()) {
                    return frame;
                }
            }
        }
    }

    std::cerr << "Erro: Falha ao capturar frame após 3 tentativas!" << std::endl;
    return cv::Mat();
}

#include "../heaters/capture.h"
#include <opencv2/core/utils/logger.hpp> // Para desativar logs desnecessários

Capture::Capture(int cameraIndex) : cap(), frame() {
    // Desativa logs verbosos do OpenCV
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    
    // Configurações para máxima performance
    cap.open(cameraIndex, cv::CAP_V4L2); // Especifica backend V4L2 no Linux
    if (!cap.isOpened()) {
        std::cerr << "Erro: Não foi possível abrir a câmera!" << std::endl;
    } else {
        // Configurações otimizadas
        cap.set(cv::CAP_PROP_FRAME_WIDTH, IMG_SZE);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMG_SZE2);
        cap.set(cv::CAP_PROP_FPS, 30);            // Ajuste conforme sua câmera
        cap.set(cv::CAP_PROP_BUFFERSIZE, 2);      // Buffer menor = menor latência
        cap.set(cv::CAP_PROP_AUTOFOCUS, 0);       // Desativa autofoco se não necessário
        cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);   // Ajuste de exposição conforme necessidade
        
        // Tenta usar codec MJPG para maior performance
        if (cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'))) {
            std::cout << "Codec MJPG ativado para melhor performance" << std::endl;
        }
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

    // Libera frame anterior para evitar alocação de memória
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
/*#include "../heaters/raspcam.h"
#include <iostream>

Capture::Capture(int cameraIndex) {
    cap.set(cv::CAP_PROP_FRAME_WIDTH, IMG_SZE);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMG_SZE2);
    cap.set(cv::CAP_PROP_FORMAT, CV_8UC3);  // formato BGR, igual ao OpenCV padrão

    if (!cap.open()) {
        std::cerr << "Erro: Não foi possível abrir a câmera Raspberry Pi!" << std::endl;
    }
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat Capture::captureImage() {
    if (cap.isOpened()) {
        cap.grab();
        cap.retrieve(frame);
        if (frame.empty()) {
            std::cerr << "Erro: Frame capturado está vazio!" << std::endl;
        }
    } else {
        std::cerr << "Erro: Câmera não está aberta!" << std::endl;
    }
    return frame;
}
*/
#include "../heaters/capture.h"
#include <opencv2/core/utils/logger.hpp> // Para desativar logs desnecessários
#include <iostream>

Capture::Capture(int cameraIndex) : cap() {
    // Desativa logs verbosos do OpenCV
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

/*#ifdef __linux__
    // Pipeline GStreamer com resolução 640x480 configurada
    std::string pipeline =
    "libcamerasrc ! videoconvert ! video/x-raw,format=BGR,width=640,height=480 ! queue max-size-buffers=1 leaky=downstream ! appsink max-buffers=1 drop=true";
    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo pipeline!" << std::endl;
    } else {
        std::cout << "Câmera aberta com sucesso pelo pipeline!" << std::endl;
    }
#else
*/
    if (!cap.open(cameraIndex)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo índice!" << std::endl;
    } else {
        // Define resolução 640x480 para outros sistemas
        cap.set(cv::CAP_PROP_FRAME_WIDTH, IMG_SZE);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMG_SZE2);
        std::cout << "Câmera aberta com sucesso pelo índice!" << std::endl;
    }
//#endif
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
    cv::Mat frame;
    if (!cap.read(frame)) {
        std::cerr << "Erro ao capturar o frame!" << std::endl;
        return cv::Mat();
    }
    return frame;
}

#include "../heaters/capture.h"
#include <opencv2/core/utils/logger.hpp> // Para desativar logs desnecessários
#include <iostream>
#include <chrono>

Capture::Capture(int cameraIndex) : cap() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef __linux__
    std::string pipeline =
        "libcamerasrc ! videoconvert ! video/x-raw,format=BGR,width=640,height=480 ! queue max-size-buffers=1 leaky=downstream ! appsink max-buffers=1 drop=true";
    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo pipeline!" << std::endl;
    } else {
        std::cout << "Câmera aberta com sucesso pelo pipeline!" << std::endl;
    }
#else
    if (!cap.open(cameraIndex)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo índice!" << std::endl;
    } else {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, IMG_SZE);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMG_SZE2);
        std::cout << "Câmera aberta com sucesso pelo índice!" << std::endl;
    }
#endif

    // Inicializa variáveis para FPS
    frameCount = 0;
    totalTime = 0.0;
    lastTime = std::chrono::high_resolution_clock::now();
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }
}

// Variáveis para medir FPS (como membros privados da classe Capture)
int Capture::frameCount = 0;
double Capture::totalTime = 0.0;
std::chrono::high_resolution_clock::time_point Capture::lastTime;

cv::Mat Capture::captureImage() {
    if (!cap.isOpened()) {
        std::cerr << "Erro: Câmera não está aberta!" << std::endl;
        return cv::Mat();
    }

    auto start = std::chrono::high_resolution_clock::now();

    cv::Mat frame;
    if (!cap.read(frame)) {
        std::cerr << "Erro ao capturar o frame!" << std::endl;
        return cv::Mat();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - lastTime;
    totalTime += diff.count();
    frameCount++;
    lastTime = end;

    // A cada 30 frames imprime FPS médio
    if (frameCount >= 30) {
        double fps = frameCount / totalTime;
        std::cout << "FPS médio: " << fps << std::endl;
        frameCount = 0;
        totalTime = 0.0;
    }

    return frame;
}

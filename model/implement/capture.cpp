#include "../heaters/capture.h"
#include <iostream>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cstdlib>

Capture::Capture(int cameraIndex)
    : cap(), shutter_us(1000) // 1000 µs = 1ms
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef __linux__
    tempFile = "/tmp/capture.jpg";
    std::cout << "Modo libcamera (sem GStreamer) iniciado\n";
#else
    if (!cap.open(cameraIndex)) {
        std::cerr << "Erro: Não foi possível abrir a câmera!\n";
    }
#endif
}

Capture::~Capture() {
#ifndef __linux__
    if (cap.isOpened())
        cap.release();
#endif
}

void Capture::captureImage() {
#ifdef __linux__

    // Comando usando shutter manual e desativando auto exposure
    std::string command =
        "libcamera-still "
        "--shutter " + std::to_string(shutter_us) + " "
        "--gain 1.0 "
        "--awbgains 1,1 "
        "--timeout 1 "
        "--nopreview "
        "-o " + tempFile;

    int ret = system(command.c_str());

    if (ret != 0) {
        std::cerr << "Erro ao executar libcamera-still\n";
    }

#else
    cap.grab();
#endif
}

cv::Mat Capture::retrieveImage() {
#ifdef __linux__

    frame = cv::imread(tempFile, cv::IMREAD_COLOR);

    if (frame.empty()) {
        std::cerr << "Erro ao carregar imagem capturada!\n";
    }

    return frame;

#else
    if (!cap.retrieve(frame)) {
        std::cerr << "Erro ao decodificar frame!\n";
        return cv::Mat();
    }
    return frame;
#endif
}

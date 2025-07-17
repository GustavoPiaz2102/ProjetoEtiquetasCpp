#include "detector.h"
#include <iostream>

Detector::Detector(Interface &inter)
    : camera(0), ocr("eng"), interface(inter), running(false) {}

Detector::~Detector() {
    stop();
}

void Detector::start() {
    if (running.load()) return;

    running = true;
    cameraThread = std::thread(&Detector::captureLoop, this);
}

void Detector::stop() {
    running = false;
    if (cameraThread.joinable()) {
        cameraThread.join();
    }
}

void Detector::captureLoop() {
    while (running) {
        cv::Mat frame = camera.captureImage();
        if (!frame.empty()) {
            std::lock_guard<std::mutex> lock(frameMutex);
            latestFrame = frame.clone();  // Clona para evitar acesso a memória que pode ser alterada
        } else {
            std::cerr << "Erro ao capturar frame." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // 30~ fps
    }
}

std::string Detector::run() {
    cv::Mat frameCopy;
    {
        std::lock_guard<std::mutex> lock(frameMutex);
        if (latestFrame.empty()) return "";
        frameCopy = latestFrame.clone();
    }

    if (interface.atualizar_frame(frameCopy)) return "return";

    cv::Mat processedFrame = preprocessor.preprocess(frameCopy);
    std::string text = ocr.extractText(processedFrame);
    return text;
}

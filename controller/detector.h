#ifndef DETECTOR_H
#define DETECTOR_H

#include "../model/heaters/capture.h"
#include "../model/heaters/preprocessor.h"
#include "../model/heaters/OCR.h"
#include "../view/interface.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class Detector {
private:
    Capture camera;
    Preprocessor preprocessor;
    OCR ocr;
    Interface& interface;

    std::thread cameraThread;
    std::atomic<bool> running;
    std::mutex frameMutex;
    cv::Mat latestFrame;

    void captureLoop();

public:
    Detector(Interface& inter);
    ~Detector();

    void start();  // Inicia thread da câmera
    void stop();   // Para a thread da câmera
    std::string run();  // Processa o frame mais recente
};

#endif

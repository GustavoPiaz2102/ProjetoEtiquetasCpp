#ifndef DETECTOR_H
#define DETECTOR_H

#include "../model/heaters/capture.h"
#include "../model/heaters/preprocessor.h"
#include "../model/heaters/OCR.h"
#include "../view/interface.h"
#include <opencv2/opencv.hpp>


class Detector {
private:
    Capture camera;
    Preprocessor preprocessor;
    OCR ocr;
    Interface& interface;
    cv::Mat frame;  // Frame capturado da câmera
public:
    Detector(Interface& inter);
    ~Detector();
    std::string run();  // Processa o frame mais recente
};

#endif

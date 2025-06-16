#ifndef DETECTOR_H
#define DETECTOR_H

#include "../model/capture.h"
#include "../model/preprocessor.h"
#include "../model/OCR.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "../view/interface.h"
#include <string>

class Detector{
    private:
        Capture camera;
        Preprocessor preprocessor;
        OCR ocr;
        Interface interface;
    public:
        Detector(Interface &inter);
        ~Detector();
        std::string run();
};
#endif
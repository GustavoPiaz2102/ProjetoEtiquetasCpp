#ifndef DETECTOR_H
#define DETECTOR_H

#include "../model/heaters/capture.h"
#include "../model/heaters/preprocessor.h"
#include "../model/heaters/OCR.h"
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
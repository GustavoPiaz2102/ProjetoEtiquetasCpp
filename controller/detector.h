#ifndef DETECTOR_H
#define DETECTOR_H

#include "../model/capture.h"
#include "../model/preprocessor.h"
#include "../model/OCR.h"

class Detector{
    private:
        Capture camera;
        Preprocessor preprocessor;
        OCR ocr;
    public:
        Detector();
        ~Detector();
        void run();
};
#endif
#include "detector.h"

Detector::Detector(Interface &inter)
    : camera(0), ocr("eng"), interface(inter) {}

Detector::~Detector() {
}

std::string Detector::run() {
    frame = camera.captureImage();
    if (interface.atualizar_frame(frame)) return "return";
    frame = preprocessor.preprocess(frame);
    std::string text = ocr.extractText(frame);
    return text;
}

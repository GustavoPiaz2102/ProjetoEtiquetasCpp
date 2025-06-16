#include "detector.h"

Detector::Detector(Interface &inter)
    : camera(0), ocr("eng"),interface(inter){}
    // Inicializa a câmera (ID 0) e OCR em inglês

Detector::~Detector() {
    // Nada específico pra destruir manualmente, pois as classes internas já se cuidam
}

std::string Detector::run() {
        cv::Mat frame = camera.captureImage();
        if (frame.empty()) {
            std::cerr << "Erro ao capturar frame." << std::endl;
        }

        cv::Mat processedFrame = preprocessor.preprocess(frame);
        std::string text = ocr.extractText(processedFrame);
        if(interface.atualizar_frame(frame)) text = "return";
        return text;
    }
#include "detector.h"

Detector::Detector(Interface inter)
    : camera(0), ocr("eng") {
    // Inicializa a câmera (ID 0) e OCR em inglês
    interface = inter;
}

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
        interface.atualizar_texto(text);
        if(interface.atulizar_frame(frame)) text = "return";
        return text;
    }
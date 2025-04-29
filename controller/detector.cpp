#include "detector.h"
#include <opencv2/opencv.hpp>
#include <iostream>

Detector::Detector()
    : camera(0), ocr("eng") {
    // Inicializa a câmera (ID 0) e OCR em inglês
}

Detector::~Detector() {
    // Nada específico pra destruir manualmente, pois as classes internas já se cuidam
}

void Detector::run() {
    while (true) {
        cv::Mat frame = camera.captureImage();
        if (frame.empty()) {
            std::cerr << "Erro ao capturar frame." << std::endl;
            break;
        }

        cv::Mat processedFrame = preprocessor.preprocess(frame);
        std::string text = ocr.extractText(processedFrame);

        std::cout << "Texto detectado: " << text << std::endl;

        // Mostrar o frame original e o frame pré-processado
        //cv::imshow("Frame Original", frame);
        //cv::imshow("Frame Pré-processado", processedFrame);

        // Se quiser, pode desenhar o texto na tela
        cv::putText(frame, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Frame com Texto", frame);

        // Sai do loop se apertar a tecla 'q'
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    cv::destroyAllWindows();
};

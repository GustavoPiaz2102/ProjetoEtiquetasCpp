#ifndef CAMERA_OCR_H
#define CAMERA_OCR_H

#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <memory>
#include <string>
#include <stdexcept>

class OCR {
private:
    static constexpr int OCR_INTERVAL_MS = 1;
    static constexpr const char* OCR_LANGUAGES = "eng+por";
    tesseract::TessBaseAPI* ocr;

public:
    /**
     * @brief Construtor que inicializa o Tesseract OCR
     * @param languages Idiomas para OCR (padrão: "eng+por")
     * @param tessdataPath Caminho opcional para a pasta tessdata
     */
    OCR(const std::string& languages = "eng+por", const char* tessdataPath = nullptr) {
        ocr = new tesseract::TessBaseAPI();
        
        if (ocr->Init(tessdataPath, languages.c_str())) {
            delete ocr;
            throw std::runtime_error("Erro ao inicializar o Tesseract OCR!");
        }

        ocr->SetPageSegMode(tesseract::PSM_AUTO);
    }

    ~OCR() {
        if (ocr) {
            ocr->End();
            delete ocr;
        }
    }

    /**
     * @brief Aplica OCR em uma imagem pré-processada
     * @param image Imagem OpenCV (cv::Mat) já pré-processada
     * @return Texto reconhecido
     */
    std::string applyOCR(const cv::Mat& image) {
        if (image.empty()) {
            throw std::invalid_argument("Imagem vazia fornecida para OCR");
        }

        ocr->SetImage(image.data, image.cols, image.rows, 1, image.step);
        
        char* text = ocr->GetUTF8Text();
        std::string result = text ? text : "";

        delete[] text;
        return result;
    }
};

#endif // CAMERA_OCR_H

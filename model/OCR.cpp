#include "OCR.h"
#include <iostream>

OCR::OCR(const std::string& language) {
    tess = new tesseract::TessBaseAPI();
    if (tess->Init(NULL, language.c_str())) {
        std::cerr << "Erro: Não foi possível inicializar o Tesseract OCR." << std::endl;
    }
}

OCR::~OCR() {
    if (tess) {
        tess->End();
        delete tess;
    }
}

std::string OCR::extractText(const cv::Mat& inputImage) {
    if (inputImage.empty()) {
        std::cerr << "Erro: Imagem vazia passada para OCR." << std::endl;
        return "";
    }

    cv::Mat gray;
    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = inputImage.clone();
    }

    cv::Mat binary;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    tess->SetImage(binary.data, binary.cols, binary.rows, 1, binary.step);

    char* outText = tess->GetUTF8Text();
    std::string result(outText);

    delete[] outText;

    return result;
}

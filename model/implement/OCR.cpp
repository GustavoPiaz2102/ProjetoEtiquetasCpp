#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language){
    tess = new tesseract::TessBaseAPI();
    if(tess->Init(NULL, language.c_str())) std::cerr << "Erro: Não foi possível inicializar o Tesseract OCR." << "\n";
}

OCR::~OCR(){
    if(tess){
        tess->End();
        delete tess;
    }
}

std::string OCR::extractText(const cv::Mat& inputImage){
    float minConfidence = 40.0f;

    if (inputImage.empty()){
        std::cerr << "Erro: imagem vazia passada para OCR.\n";
        return "";
    }

    // Garantir que a imagem seja contínua
    cv::Mat img = inputImage.isContinuous() ? inputImage : inputImage.clone();

    // Converter para grayscale se tiver 3 canais
    cv::Mat gray;
    if (img.channels() == 3) cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else gray = img;

    // Ajuste mínimo de contraste
    cv::Mat contrast;
    cv::normalize(gray, contrast, 0, 255, cv::NORM_MINMAX);

    // Leve nitidez
    cv::Mat kernel = (cv::Mat_<float>(3,3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    cv::filter2D(contrast, contrast, contrast.depth(), kernel);

    // Passar para Tesseract
    tess->SetImage(contrast.data, contrast.cols, contrast.rows, 1, contrast.step);

    // PSM adequado para múltiplas linhas de um bloco
    tess->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);

    // Reconhece a imagem
    tess->Recognize(0);

    std::string finalText;
    tesseract::ResultIterator* ri = tess->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

    if(ri != nullptr){
        do{
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);  // Confiança de 0 a 100

            if (word && conf >= minConfidence){
                finalText += word;
                finalText += "\n";
            }

            delete[] word;
        } while (ri->Next(level));
    }

    return finalText;
}

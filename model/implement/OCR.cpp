#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language){
	tess = new tesseract::TessBaseAPI();
	if(tess->Init(nullptr, language.c_str(),tesseract::OEM_LSTM_ONLY)) std::cerr << "Erro: Não foi possível inicializar o Tesseract OCR." << "\n";
	tess->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:LFVJANFEVMARABRMAIJUNJULAGOSETOUTNOVDEZ");
	tess->SetVariable("load_system_dawg", "0");
	tess->SetVariable("load_freq_dawg", "0");
	tess->SetVariable("load_unambig_dawg", "0");
	tess->SetVariable("load_punc_dawg", "0");
	tess->SetVariable("load_number_dawg", "0");
	tess->SetVariable("load_bigram_dawg", "0");
}

OCR::~OCR(){
	if(tess){
		tess->End();
		delete tess;
	}
}

std::string OCR::extractText(const cv::Mat& inputImage){
    if (inputImage.empty()) return "";

    std::string finalText;
    int lineHeight = inputImage.rows / 3;

    for(int i = 0; i < 3; i++){
        cv::Mat linha = inputImage(cv::Rect(0, i * lineHeight, inputImage.cols, lineHeight));

        tess->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
        tess->SetImage(linha.data, linha.cols, linha.rows, 1, linha.step);
        tess->Recognize(0);

        char* text = tess->GetUTF8Text();
        if(text){
            std::string s(text);
            delete[] text;
            // Remove \n
            s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
            if(!s.empty()) finalText += s + "\n";
        }
        tess->Clear();
    }

    return finalText;
}
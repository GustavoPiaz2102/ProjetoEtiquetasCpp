#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language){
	tess = new tesseract::TessBaseAPI();
	tess->Init(NULL, language.c_str(), tesseract::OEM_LSTM_ONLY);
	tess->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	//tess->SetVariable("tessedit_char_whitelist", "0123456789/:LFVJANFEVMARABRMAIJUNJULAGOSETOUTNOVDEZ");

}

OCR::~OCR(){
	if(tess){
		tess->End();
		delete tess;
	}
}

std::string OCR::extractText(const cv::Mat& inputImage){

	if (inputImage.empty()){
		std::cerr << "Erro: imagem vazia passada para OCR.\n";
		return "";
	}
	tess->SetImage(inputImage.data, inputImage.cols, inputImage.rows, 1, inputImage.step);

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
	//tess->Clear();
	return finalText;
}

#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language){
	tess = new tesseract::TessBaseAPI();
	tess->Init(NULL, language.c_str(), tesseract::OEM_LSTM_ONLY);
	tess->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:LFVJANFEVMARABRMAIJUNJULAGOSETOUTNOVDEZ");

}

OCR::~OCR(){
	if(tess){
		tess->End();
		delete tess;
	}
}

std::string OCR::extractText(const cv::Mat& inputImage){

	m_textBuffer.clear();

	if (inputImage.empty()){
		std::cout << "Erro: imagem vazia passada para OCR.\n";
		return m_textBuffer;
	}

	if (inputImage.channels() != 1){
		std::cout << "Erro: imagem não está em grayscale.\n";
		return m_textBuffer;
	}

	tess->Clear();

	tess->SetImage(inputImage.data,inputImage.cols,inputImage.rows,1,inputImage.step);

	if (tess->Recognize(0) != 0){
		std::cout << "Erro no reconhecimento.\n";
		return m_textBuffer;
	}

	tesseract::ResultIterator* ri = tess->GetIterator();

	if (ri){
		do{
			const char* word = ri->GetUTF8Text(tesseract::RIL_WORD);
			float conf = ri->Confidence(tesseract::RIL_WORD);

			if (word && conf >= minConfidence){
				m_textBuffer += word;
				m_textBuffer += "\n";
			}

			delete[] word;

		} while (ri->Next(tesseract::RIL_WORD));
	}

	return m_textBuffer;
}
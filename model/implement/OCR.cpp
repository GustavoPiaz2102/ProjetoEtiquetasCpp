#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language){
	tess = new tesseract::TessBaseAPI();
	if(tess->Init("/home/pi/Desktop/git/ProjetoEtiquetasCpp/ocrModels/tessdata/", language.c_str())) std::cerr << "Erro: Não foi possível inicializar o Tesseract OCR." << "\n";
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:LFVJANFEVMARABRMAIJUNJULAGOSETOUTNOVDEZ");
	tess->SetVariable("load_system_dawg", "0");
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

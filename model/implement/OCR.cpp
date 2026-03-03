#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language){
	tess = new tesseract::TessBaseAPI();
	if(tess->Init("/home/pi/Desktop/git/ProjetoEtiquetasCpp/ocrModels/", language.c_str())) std::cerr << "Erro: Não foi possível inicializar o Tesseract OCR." << "\n";
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:LFVJANFEVMARABRMAIJUNJULAGOSETOUTNOVDEZ");

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
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	tess->SetImage(inputImage.data, inputImage.cols, inputImage.rows, 1, inputImage.step);
	std::cout << "Tempo para configurar a imagem: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
	start = std::chrono::steady_clock::now();
	// PSM adequado para múltiplas linhas de um bloco
	tess->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	std::cout << "Tempo para configurar o PSM: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
	start = std::chrono::steady_clock::now();
	// Reconhece a imagem
	tess->Recognize(0);
	std::cout << "Tempo para reconhecimento: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
	start = std::chrono::steady_clock::now();
	std::string finalText;
	tesseract::ResultIterator* ri = tess->GetIterator();
	std::cout << "Tempo para obter iterador: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
	start = std::chrono::steady_clock::now();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	std::cout << "Tempo para configurar nível de iteração: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
	start = std::chrono::steady_clock::now();
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
	std::cout << "Tempo para processar texto: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
	return finalText;
}

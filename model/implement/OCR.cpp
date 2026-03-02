#include "../heaters/OCR.h"

OCR::OCR(const std::string& language) {
	tess = std::make_unique<tesseract::TessBaseAPI>();
	
	if (tess->Init(nullptr, language.c_str())) {
		throw std::runtime_error("Erro ao inicializar Tesseract.");
	}

	tess->SetVariable("load_system_dawg", "0");
	tess->SetVariable("load_freq_dawg", "0");
	tess->SetVariable("load_punc_dawg", "0");
	tess->SetVariable("load_number_dawg", "0");
	tess->SetVariable("load_unambig_dawg", "0");
	tess->SetVariable("load_bigram_dawg", "0");
	tess->SetVariable("tessedit_do_invert", "0");
	
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:LFVJANFEVMARABRMAIJUNJULAGOSETOTUNOVDEZ ");
	
	tess->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
}

OCR::~OCR() {
	if (tess) {
		tess->End();
	}
}

std::string OCR::extractText(const cv::Mat& inputImage) {
	if (inputImage.empty()) return "";

	cv::Mat processed;
	if (inputImage.channels() > 1) {
		cv::cvtColor(inputImage, processed, cv::COLOR_BGR2GRAY);
	} else {
		processed = inputImage;
	}

	cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	tess->SetImage(processed.data, processed.cols, processed.rows, 1, static_cast<int>(processed.step));
	
	if (tess->Recognize(nullptr) != 0) return "";

	std::string finalText;
	finalText.reserve(40); // Espaço para os 31 caracteres + quebras de linha

	std::unique_ptr<tesseract::ResultIterator> ri(tess->GetIterator());
	const tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL; 

	if (ri) {
		do {
			float conf = ri->Confidence(level);
			if (conf >= minConfidence) {
				char* symbol = ri->GetUTF8Text(level);
				if (symbol) {
					finalText.append(symbol);
					delete[] symbol;
				}
			}

			// Detecta o fim de cada linha para manter a estrutura L / F / V
			if (ri->IsAtFinalElement(tesseract::RIL_TEXTLINE, level)) {
				finalText.append("\n");
			}
			
			// Se já pegamos o bloco todo de 3 linhas (aprox 31-35 chars), paramos
			if (finalText.length() >= 40) break;

		} while (ri->Next(level));
	}

	return finalText;
}
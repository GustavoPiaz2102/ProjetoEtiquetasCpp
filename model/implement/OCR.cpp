#include "../heaters/OCR.h"

OCR::OCR(const std::string& language) {
	tess = std::make_unique<tesseract::TessBaseAPI>();
	
	if (tess->Init(nullptr, language.c_str())) {
		throw std::runtime_error("Erro ao inicializar Tesseract.");
	}

	// Otimizações de dicionário (essencial para strings fixas/códigos)
	tess->SetVariable("load_system_dawg", "0");
	tess->SetVariable("load_freq_dawg", "0");
	tess->SetVariable("load_punc_dawg", "0");
	tess->SetVariable("load_number_dawg", "0");
	tess->SetVariable("load_unambig_dawg", "0");
	tess->SetVariable("load_bigram_dawg", "0");
	tess->SetVariable("load_fixed_length_dawgs", "0");
	
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:abdefgijlmnorstuvz ");
	
	// Como o tamanho é fixo, SINGLE_LINE ou SINGLE_BLOCK costumam ser mais rápidos
	tess->SetPageSegMode(tesseract::PSM_SINGLE_LINE); 
}

std::string OCR::extractText(const cv::Mat& inputImage) {
	if (inputImage.empty()) return "";

	// 1. Garantir Grayscale sem cópia desnecessária
	cv::Mat gray;
	if (inputImage.channels() > 1) {
		cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
	} else {
		gray = inputImage;
	}

	tess->SetImage(gray.data, gray.cols, gray.rows, 1, static_cast<int>(gray.step));
	
	if (tess->Recognize(nullptr) != 0) return "";

	// 2. Pré-alocação exata de memória
	std::string finalText;
	finalText.reserve(32); // 31 caracteres + null terminator

	std::unique_ptr<tesseract::ResultIterator> ri(tess->GetIterator());
	const tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL; // Nível de caractere para maior controle

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
			// Interrompe se já atingiu o limite (evita lixo no final da imagem)
			if (finalText.length() >= 31) break;

		} while (ri->Next(level));
	}

	return finalText;
}
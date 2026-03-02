#include "../heaters/OCR.h"
#include <iostream>

OCR::OCR(const std::string& language) {
	tess = new tesseract::TessBaseAPI();
	if (tess->Init(NULL, language.c_str()))
		std::cerr << "Erro: Não foi possível inicializar o Tesseract OCR.\n";

	tess->SetVariable("load_system_dawg", "0");
	tess->SetVariable("load_freq_dawg", "0");
	tess->SetVariable("tessedit_do_invert", "0");
	tess->SetVariable("tessedit_char_whitelist", "0123456789/:abdefgijlmnorstuvz ");
}

OCR::~OCR() {
	if (tess) {
		tess->End();
		delete tess;
	}
}

std::string OCR::extractText(const cv::Mat& inputImage) {
	if (inputImage.empty()) {
		std::cerr << "Erro: imagem vazia passada para OCR.\n";
		return {};
	}

	tess->SetImage(inputImage.data, inputImage.cols, inputImage.rows, 1, (int)inputImage.step);
	tess->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
	tess->Recognize(0);

	std::unique_ptr<tesseract::ResultIterator> ri(tess->GetIterator());
	if (!ri) return {};

	constexpr auto level = tesseract::RIL_WORD;
	m_textBuffer.clear();

	do {
	auto word = std::unique_ptr<const char[], void(*)(const char*)>(
		ri->GetUTF8Text(level),
		[](const char* p){ delete[] p; }
	);

	if (word && ri->Confidence(level) >= minConfidence) {
		m_textBuffer += word.get();
		m_textBuffer += ' ';
	}
	} while (ri->Next(level));

		if (!m_textBuffer.empty())
			m_textBuffer.pop_back();

		return m_textBuffer;
}
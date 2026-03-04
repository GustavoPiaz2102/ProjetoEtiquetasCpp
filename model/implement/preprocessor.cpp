#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
	// Inicialização necessária, se houver.
}

Preprocessor::~Preprocessor() {
	// Nada pra liberar por enquanto.
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
	cv::Mat gray;

	// 1. Converte para escala de cinza
	if (inputImage.channels() == 1)
		gray = inputImage.clone();
	else
		cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

	// 2. Binarização com Otsu (fundo claro, texto escuro)
	cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	// 3. Grayscale → RGB (PP-OCRv3 espera 3 canais)
	cv::Mat rgb;
	cv::cvtColor(gray, rgb, cv::COLOR_GRAY2RGB);

	// 4. Normaliza para [-1, 1]: (pixel / 127.5) - 1.0
	// Saída: CV_32FC3, shape [H, W, 3] — o OCR monta o tensor CHW internamente
	cv::Mat result;
	rgb.convertTo(result, CV_32F, 1.0 / 127.5, -1.0);

	return result;
}
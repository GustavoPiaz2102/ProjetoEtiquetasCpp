#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
	// Inicialização necessária, se houver.
}

Preprocessor::~Preprocessor() {
	// Nada pra liberar por enquanto.
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
	cv::Mat gray;

	// Converte a imagem para escala de cinza
	cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

	//aplicação de threashold
	cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

	// Aplica um blur (suavização)
	cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

	// Aplica um filtro de mediana para reduzir ruídos
	cv::medianBlur(gray, gray, 3);
	return gray;
}

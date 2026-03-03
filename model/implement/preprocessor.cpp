#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
	// Inicialização necessária, se houver.
}

Preprocessor::~Preprocessor() {
	// Nada pra liberar por enquanto.
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
	cv::Mat gray;

	// 1. Escala de cinza
	cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

	// 2. CLAHE - normaliza iluminação irregular
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
	clahe->apply(gray, gray);

	// 3. Resize 2x - aumenta DPI virtual para o Tesseract
	cv::resize(gray, gray, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);

	// 4. Binarização adaptativa - melhor que Otsu para iluminação não uniforme
	cv::adaptiveThreshold(gray, gray, 255,
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,
		cv::THRESH_BINARY, 31, 10);

	// 5. Median blur - remove ruído sem borrar bordas das letras
	cv::medianBlur(gray, gray, 3);

	return gray;
}
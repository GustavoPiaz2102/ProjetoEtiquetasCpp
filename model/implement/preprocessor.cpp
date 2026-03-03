#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
	// Inicialização necessária, se houver.
}

Preprocessor::~Preprocessor() {
	// Nada pra liberar por enquanto.
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
    cv::Mat output;

    // 1. Cinza
    cv::cvtColor(inputImage, output, cv::COLOR_BGR2GRAY);

    // 2. Aumentar contraste (opcional, mas ajuda se a iluminação for ruim)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
    clahe->apply(output, output);

    // 3. Blur leve para suavizar bordas serrilhadas (ajuda o OCR a ver formas contínuas)
    cv::GaussianBlur(output, output, cv::Size(3, 3), 0);

    // 4. Binarização de Otsu
    cv::threshold(output, output, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // 5. Inverter se o fundo for escuro e o texto claro
    // O Tesseract prefere Texto Preto no Fundo Branco
    // double blackPixels = cv::countNonZero(output == 0);
    // double whitePixels = cv::countNonZero(output == 255);
    // if (blackPixels > whitePixels) cv::bitwise_not(output, output);

    return output;
}
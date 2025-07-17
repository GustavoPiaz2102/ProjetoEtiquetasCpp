#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
    // Inicialização necessária, se houver.
}

Preprocessor::~Preprocessor() {
    // Nada pra liberar por enquanto.
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
    cv::Mat gray, blurred;

    // Converte a imagem para escala de cinza
    cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

    // Aplica um blur (suavização)
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    return blurred;
}

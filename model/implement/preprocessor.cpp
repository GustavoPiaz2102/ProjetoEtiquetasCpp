#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
}

Preprocessor::~Preprocessor() {
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
	cv::Mat gray;

	// Converte a imagem para escala de cinza
	cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
	// Aplica um blur (suavização)
	//cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

	if (cv::mean(gray)[0] < 127) cv::bitwise_not(gray, gray);

	cv::Mat rgb;
	cv::cvtColor(gray, rgb, cv::COLOR_GRAY2RGB);

	cv::Mat result;
	rgb.convertTo(result, CV_32F, 1.0 / 127.5, -1.0);

	return result;
}
	

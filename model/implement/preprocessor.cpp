#include "../heaters/preprocessor.h"

Preprocessor::Preprocessor() {
}

Preprocessor::~Preprocessor() {
}

cv::Mat Preprocessor::preprocess(const cv::Mat& inputImage) {
	cv::Mat gray;

	if (inputImage.channels() == 1)
		gray = inputImage.clone();
	else
		cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);

	cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	// Se a maioria dos pixels é escura, o fundo está escuro — inverte
	// (fundo deve ser claro = maioria dos pixels brancos)
	if (cv::mean(gray)[0] < 127)
		cv::bitwise_not(gray, gray);

	cv::Mat rgb;
	cv::cvtColor(gray, rgb, cv::COLOR_GRAY2RGB);

	cv::Mat result;
	rgb.convertTo(result, CV_32F, 1.0 / 127.5, -1.0);

	return result;
}
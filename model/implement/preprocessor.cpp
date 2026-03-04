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

	if (cv::mean(gray)[0] < 127)
		cv::bitwise_not(gray, gray);

	cv::Mat rgb;
	cv::cvtColor(gray, rgb, cv::COLOR_GRAY2RGB);

	cv::Mat result;
	rgb.convertTo(result, CV_32F, 1.0 / 127.5, -1.0);

	return result;
}

cv::Mat Preprocessor::prepareForRec(const cv::Mat& inputImage) {
	cv::Mat rgb;

	if (inputImage.channels() == 1)
		cv::cvtColor(inputImage, rgb, cv::COLOR_GRAY2RGB);
	else
		cv::cvtColor(inputImage, rgb, cv::COLOR_BGR2RGB);

	cv::Mat floatImg;
	rgb.convertTo(floatImg, CV_32F, 1.0 / 255.0);

	// Normalização ImageNet: mean=[0.485,0.456,0.406], std=[0.229,0.224,0.225]
	std::vector<cv::Mat> channels(3);
	cv::split(floatImg, channels);

	channels[0] = (channels[0] - 0.485f) / 0.229f;
	channels[1] = (channels[1] - 0.456f) / 0.224f;
	channels[2] = (channels[2] - 0.406f) / 0.225f;

	cv::Mat result;
	cv::merge(channels, result);

	return result;
}
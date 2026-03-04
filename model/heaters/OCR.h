#ifndef OCR_H
#define OCR_H

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

#define MODEL_DIR "/home/pi/models"

class OCR {
	private:
		Ort::Env env;
		Ort::SessionOptions sessionOptions;
		std::unique_ptr<Ort::Session> detSession;
		std::unique_ptr<Ort::Session> recSession;

		std::vector<std::string> charset;
		float minConfidence = 45.0f;

		std::vector<cv::Rect> detect(const cv::Mat& detImg);
		std::string recognize(const cv::Mat& lineImg);
		std::vector<float> buildTensor(const cv::Mat& img, int& outH, int& outW);
		std::string ctcDecode(const float* logits, int timeSteps, int numClasses);
		void loadCharset(const std::string& keysPath);

	public:
		OCR(const std::string& modelDir = MODEL_DIR);
		~OCR() = default;

		// detImg:  saída do Preprocessor::preprocess()   — binarizado, normalizado
		// origImg: imagem original BGR para recortar e passar ao recognizer
		std::string extractText(const cv::Mat& detImg, const cv::Mat& origImg);
};

#endif // OCR_H
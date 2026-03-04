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
		std::unique_ptr<Ort::Session> session;

		std::vector<std::string> charset;
		float minConfidence = 45.0f; // escala 0–100, equivalente ao Tesseract

		// Monta tensor CHW [1,3,H,W] a partir do Mat CV_32FC3 do Preprocessor
		std::vector<float> buildTensor(const cv::Mat& img);
		std::string ctcDecode(const float* logits, int timeSteps, int numClasses);
		void loadCharset(const std::string& keysPath);

	public:
		OCR(const std::string& modelDir = MODEL_DIR);
		~OCR() = default;

		// Recebe cv::Mat CV_32FC3 normalizado [-1,1] vindo do Preprocessor
		std::string extractText(const cv::Mat& inputImage);
};

#endif // OCR_H
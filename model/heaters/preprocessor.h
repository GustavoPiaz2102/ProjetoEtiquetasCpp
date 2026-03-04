#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <opencv2/opencv.hpp>

class Preprocessor {
	public:
		Preprocessor();
		~Preprocessor();

		// Para o detector: grayscale + Otsu + normalização [-1,1]
		cv::Mat preprocess(const cv::Mat& inputImage);

		// Para o recognizer: só normalização [-1,1] sem binarização
		cv::Mat prepareForRec(const cv::Mat& inputImage);
};

#endif // PREPROCESSOR_H
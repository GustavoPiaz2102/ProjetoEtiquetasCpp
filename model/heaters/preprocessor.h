#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <opencv2/opencv.hpp>

class Preprocessor {
	public:
		Preprocessor();
		~Preprocessor();

		cv::Mat preprocess(const cv::Mat& inputImage);
};

#endif // PREPROCESSOR_H

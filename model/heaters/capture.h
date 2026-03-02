#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"
#include <opencv2/core/utils/logger.hpp>

#define IMG_SZE 320
#define IMG_SZE2 240

class Capture {
public:
	Capture(int cameraIndex = 0);
	~Capture();
	void captureImage();
	cv::Mat retrieveImage();

private:
	cv::VideoCapture cap;
	cv::Mat frame;
};

#endif
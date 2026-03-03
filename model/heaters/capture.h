#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"
#include <opencv2/core/utils/logger.hpp>

#define IMG_WIDTH 960
#define IMG_HEIGHT 720

#define ROI_WIDTH int(IMG_WIDTH * .469) // 300
#define ROI_HEIGHT int(IMG_HEIGHT * .312) // 150

class Capture {
public:
	Capture(int cameraIndex = 0);
	~Capture();
	void captureImage();
	cv::Mat retrieveImage();
	cv::Rect roi;

private:
	cv::VideoCapture cap;
	cv::Mat frame;
};

#endif
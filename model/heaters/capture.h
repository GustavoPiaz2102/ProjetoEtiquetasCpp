#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"
#include <opencv2/core/utils/logger.hpp>

#define IMG_WIDTH 640
#define IMG_HEIGHT 480

#define ROI_WIDTH 360
#define ROI_HEIGHT 180

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
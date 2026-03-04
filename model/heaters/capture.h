#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"
#include <opencv2/core/utils/logger.hpp>

#define IMG_WIDTH 1280
#define IMG_HEIGHT 960

#define ROI_WIDTH  int(IMG_WIDTH * .5)      // 640  (múltiplo de 32)
#define ROI_HEIGHT int(IMG_HEIGHT * .3333)  // 320  (múltiplo de 32)

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
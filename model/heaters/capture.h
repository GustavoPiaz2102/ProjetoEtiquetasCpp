#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"

#define IMG_SZE 640
#define IMG_SZE2 480

class Capture {
public:
	Capture(int cameraIndex = 0);
	~Capture();
	void captureImage();
	cv::Mat retrieveImage();

private:
	cv::VideoCapture cap;
	cv::Mat frame;
	int debug_counter = 0;
	std::string tempFile;
	int shutter_us;
};

#endif
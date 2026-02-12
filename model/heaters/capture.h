#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"
#include <opencv2/core/utils/logger.hpp>

#define IMG_SZE 640
#define IMG_SZE2 480

std::string pipeline =
		"libcamerasrc ! "
		"video/x-raw, width=640, height=480, format=RGBx ! " 
		"videoconvert ! "
		"video/x-raw, format=BGR ! "
		"appsink drop=true max-buffers=1 sync=false";


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
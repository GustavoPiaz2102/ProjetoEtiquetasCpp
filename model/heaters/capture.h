#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fcntl.h>    // Para F_GETFL, F_SETFL e O_NONBLOCK
#include <unistd.h>   // Para read() e fileno()
#include "GPIO.h"
#include <cstdio>
#include <vector>
#include <errno.h>    // Para verificar erros de leitura

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
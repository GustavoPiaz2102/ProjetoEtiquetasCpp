#ifndef CAPTURE_H
#define CAPTURE_H

#include "GPIO.h"
#include <iostream>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

#define IMG_WIDTH 1280
#define IMG_HEIGHT 960

#define ROI_WIDTH int(IMG_WIDTH * .41)	 // 300
#define ROI_HEIGHT int(IMG_HEIGHT * .33) // 336

// Controle manual de exposição/ganho do sensor (calibrado com o flash aceso).
// Necessário pois o AE/AGC automático do libcamera diverge entre capturas
// com e sem strobe, causando estouro de luz a partir da 2ª foto em diante.
#define EXPOSURE_TIME_US 250 // microssegundos
#define ANALOGUE_GAIN 1.0f   // multiplicador (1.0 = sem ganho)

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
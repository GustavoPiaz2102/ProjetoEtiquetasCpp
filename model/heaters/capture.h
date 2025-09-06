#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "GPIO.h"
#define IMG_SZE 1280
#define IMG_SZE2 720

class Capture {
public:
    Capture(int cameraIndex = 0);
    ~Capture();
    cv::Mat captureImage();
private:
    cv::VideoCapture cap;
    cv::Mat frame;
    //Cria o objeto GPIO com os pinos do sensor e do strobo
    GPIO gpio{17, 27, "gpiochip4"}; 
};

#endif

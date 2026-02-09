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
    cv::Mat captureImage();
private:
    cv::VideoCapture cap;
    cv::Mat frame;
    //Cria o objeto GPIO com os pinos do sensor e do strobo
    GPIO gpio{21, "gpiochip4"}; 
    int debug_counter = 0;
};

#endif
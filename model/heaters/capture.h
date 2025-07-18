#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include <memory>
#include <array>
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

    bool isRaspberryPi();
};

#endif

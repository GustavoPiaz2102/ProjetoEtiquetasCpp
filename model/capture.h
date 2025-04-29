#ifndef CAPTURE_H
#define CAPTURE_H

#include <opencv2/opencv.hpp>

class Capture {
public:
    Capture(int cameraIndex = 0);
    ~Capture();

    cv::Mat captureImage();

private:
    cv::VideoCapture cap;
};

#endif

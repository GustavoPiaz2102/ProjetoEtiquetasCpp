/*#ifndef RASPCAM_H
#define RASPCAM_H

#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

#define IMG_SZE 1280
#define IMG_SZE2 720

class Capture {
public:
    Capture(int cameraIndex = 0);
    ~Capture();

    cv::Mat captureImage();

private:
    raspicam::RaspiCam_Cv cap;
    cv::Mat frame;
};

*/
#endif
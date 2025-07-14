#ifndef OCR_H
#define OCR_H

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <string>

class OCR {
public:
    OCR(const std::string& language = "eng");
    ~OCR();

    std::string extractText(const cv::Mat& inputImage);

private:
    tesseract::TessBaseAPI* tess;
};

#endif // OCR_PROCESSOR_H

#ifndef OCR_H
#define OCR_H

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <string>

class OCR {
    private:
        tesseract::TessBaseAPI* tess;

    public:
        OCR(const std::string& language = "eng");
        ~OCR();

        std::string extractText(const cv::Mat& inputImage);
};

#endif // OCR_H

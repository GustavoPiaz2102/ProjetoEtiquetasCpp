#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // pipeline ajustado sem forçar formato
    std::string pipeline =
        "libcamerasrc ! videoconvert ! video/x-raw,format=BGR ! appsink";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera via GStreamer!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        cap.read(frame);
        if (frame.empty()) break;

        cv::imshow("Camera View", frame);
        if (cv::waitKey(1) == 27) break; // ESC
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

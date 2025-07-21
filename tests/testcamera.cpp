#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap("/dev/video10"); // 0 == /dev/video0

    if (!cap.isOpened()) {
        std::cerr << "Não foi possível abrir a câmera!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    std::cout << "Pressione ESC para sair." << std::endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("Câmera", frame);
        if (cv::waitKey(30) == 27) break; // ESC
    }

    return 0;
}

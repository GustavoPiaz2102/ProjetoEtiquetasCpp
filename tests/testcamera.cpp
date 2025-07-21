#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap("/dev/video10"); // câmera virtual v4l2loopback

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera (/dev/video10)!" << std::endl;
        return -1;
    }

    std::cout << "Câmera aberta. Pressione ESC para sair.\n";

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Frame vazio!\n";
            continue;
        }

        cv::imshow("Câmera Virtual", frame);

        // Espera 1ms por tecla, sai se pressionar ESC
        if (cv::waitKey(1) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

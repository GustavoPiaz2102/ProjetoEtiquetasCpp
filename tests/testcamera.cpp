#include <opencv2/opencv.hpp>
#include <cstdlib>

int main() {
    // Captura imagem da câmera e salva em /tmp/frame.jpg
    system("libcamera-jpeg -o /tmp/frame.jpg --width 640 --height 480 --quality 95");

    // Carrega a imagem no OpenCV
    cv::Mat img = cv::imread("/tmp/frame.jpg");
    if (img.empty()) {
        std::cerr << "Erro ao carregar imagem!" << std::endl;
        return -1;
    }

    cv::imshow("Imagem", img);
    cv::waitKey(0);
    return 0;
}

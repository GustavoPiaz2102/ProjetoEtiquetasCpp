#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>

int main() {
    raspicam::RaspiCam_Cv camera;
    cv::Mat frame;

    // Configurações básicas
    camera.set(cv::CAP_PROP_FORMAT, CV_8UC3); // Formato de imagem BGR
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    std::cout << "Abrindo a câmera..." << std::endl;
    if (!camera.open()) {
        std::cerr << "Erro ao abrir a câmera!" << std::endl;
        return -1;
    }

    std::cout << "Câmera aberta! Pressione ESC para sair." << std::endl;

    while (true) {
        camera.grab();              // Captura um novo frame
        camera.retrieve(frame);     // Pega o frame capturado

        // Exibe o frame
        cv::imshow("RaspiCam", frame);

        // Sai ao pressionar ESC
        if (cv::waitKey(30) == 27)
            break;
    }

    camera.release();
    cv::destroyAllWindows();
    return 0;
}

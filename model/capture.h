#include <opencv2/opencv.hpp>
#include <memory>
#include <stdexcept>

#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

using namespace cv;
using namespace std;

class Camera {
private:
    VideoCapture cap;
    bool isCameraOpen;

    /**
     * @brief Inicializa a câmera
     * @throws runtime_error Se não for possível abrir a câmera
     */
    void initializeCamera(int cameraIndex) {
        cap.open(cameraIndex);
        if (!cap.isOpened()) {
            isCameraOpen = false;
            throw runtime_error("Não foi possível abrir a câmera!");
        }
        isCameraOpen = true;
    }

public:
    /**
     * @brief Construtor que inicializa a câmera
     * @param cameraIndex Índice da câmera (padrão: 0)
     */
    explicit Camera(int cameraIndex = 0) : isCameraOpen(false) {
        initializeCamera(cameraIndex);
    }

    /**
     * @brief Destrutor que libera a câmera
     */
    ~Camera() {
        if (isCameraOpen) {
            cap.release();
        }
    }

    /**
     * @brief Captura um único frame da câmera
     * @return Mat Imagem capturada
     * @throws runtime_error Se a câmera não estiver aberta ou frame estiver vazio
     */
    Mat captureFrame() {
        if (!isCameraOpen) {
            throw runtime_error("Câmera não está disponível para captura!");
        }

        Mat frame;
        cap >> frame;
        
        if (frame.empty()) {
            throw runtime_error("Frame vazio capturado da câmera!");
        }

        return frame;
    }

    /**
     * @brief Verifica se a câmera está pronta para captura
     * @return bool True se a câmera está aberta e disponível
     */
    bool isReady() const {
        return isCameraOpen;
    }
};

#endif 
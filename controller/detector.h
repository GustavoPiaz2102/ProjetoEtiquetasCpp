#include "../model/capture.h"
#include "../model/ocr.h"
#include "../model/preprocessor.h"
#include <opencv2/opencv.hpp>
#include <iostream>

#ifndef DETECTOR_H
#define DETECTOR_H

using namespace cv;
using namespace std;

class Detector {
private:
    Camera camera;
    OCR ocr;
    Preprocessor preprocessor;
    bool running;

public:
    /**
     * @brief Construtor do detector
     * @param cameraIndex Índice da câmera (padrão: 0)
     */
    explicit Detector(int cameraIndex = 0) : camera(cameraIndex), running(false) {}

    /**
     * @brief Executa o pipeline de detecção contínua
     */
    void executar() {
        running = true;
        
        while(running) {
            try {
                Mat frame = camera.captureFrame();
                Mat processed = preprocessor.process(frame);
                string text = ocr.applyOCR(processed);
                
                if(!text.empty()) {
                    cout << "Texto Detectado: " << text << endl;
                }
                
                // Adiciona pequena pausa para evitar uso excessivo da CPU
                waitKey(30);
                
            } catch (const exception& e) {
                cerr << "Erro no pipeline: " << e.what() << endl;
                stop();
            }
        }
    }

    /**
     * @brief Interrompe a execução do detector
     */
    void stop() {
        running = false;
    }
};

#endif // DETECTOR_H
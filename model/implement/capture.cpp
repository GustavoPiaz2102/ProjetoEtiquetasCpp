#include "../heaters/capture.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <cstdio>
#include <vector>

// Ponteiro para o processo do rpicam
static FILE* pipePtr = nullptr;
// Buffer para armazenar um frame YUV420 (640 * 480 * 1.5)
static std::vector<uchar> buffer(460800);

Capture::Capture(int cameraIndex) : shutter_us(200) {
    // Comando para rpicam-vid: 
    // -t 0 (infinito), codec yuv420, shutter e gain manuais, output para o pipe (-)
	std::string cmd = "rpicam-vid -t 0 --shutter " + std::to_string(shutter_us) + 
			" --gain 8.0 --width 640 --height 480 --nopreview --codec yuv420 --flush -o -";
    
    std::cout << "Iniciando captura via Pipe: " << cmd << std::endl;
    pipePtr = popen(cmd.c_str(), "r");

    if (!pipePtr) {
        std::cerr << "Erro ao abrir o processo rpicam-vid!" << std::endl;
    }
}

Capture::~Capture() {
    if (pipePtr) {
        pclose(pipePtr);
    }
}

void Capture::captureImage() {
    // 1. Drenar o buffer: Se o seu processamento demorou, 
    // podem existir frames "velhos" esperando no pipe.
    // Vamos ler o que estiver lá até que o pipe esteja quase vazio.
    
    int fd = fileno(pipePtr);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK); // Torna a leitura não-bloqueante temporariamente

    std::vector<uchar> junk(buffer.size());
    while (read(fd, junk.data(), junk.size()) > 0) {
        // Continua lendo e descartando enquanto houver frames completos acumulados
    }

    fcntl(fd, F_SETFL, flags); // Volta para o modo bloqueante para ler o frame atual

    // 2. Agora lemos o frame "fresco" que acabou de chegar
    size_t totalRead = 0;
    while (totalRead < buffer.size()) {
        size_t bytes = fread(buffer.data() + totalRead, 1, buffer.size() - totalRead, pipePtr);
        if (bytes <= 0) break;
        totalRead += bytes;
    }
}

cv::Mat Capture::retrieveImage() {
    // No formato YUV420, a altura total no buffer é 1.5x a altura da imagem
    cv::Mat yuvFrame(480 + 240, 640, CV_8UC1, buffer.data());
    
    // Converte de YUV420 (I420) para BGR (OpenCV padrão)
    if (!yuvFrame.empty()) {
        cv::cvtColor(yuvFrame, frame, cv::COLOR_YUV2BGR_I420);
    }

    return frame;
}
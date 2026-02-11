#include "../heaters/capture.h"


// Ponteiro para o processo do rpicam
static FILE* pipePtr = nullptr;
// Buffer para armazenar um frame YUV420 (640 * 480 * 1.5)
static std::vector<uchar> buffer(460800);

Capture::Capture(int cameraIndex) : shutter_us(200) {
    // Comando para rpicam-vid: 
    // -t 0 (infinito), codec yuv420, shutter e gain manuais, output para o pipe (-)
    std::string cmd = "rpicam-vid -t 0 --shutter " + std::to_string(shutter_us) + 
                      " --gain 4.0 --width 640 --height 480 --nopreview --codec yuv420 --flush -o -";
    
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
    // Lê exatamente o tamanho de um frame do pipe
    size_t bytesRead = fread(buffer.data(), 1, buffer.size(), pipePtr);
    
    if (bytesRead != buffer.size()) {
        // Se falhar, pode ser que o rpicam ainda esteja iniciando
        // ou o buffer esteja vazio.
    }
}

cv::Mat Capture::retrieveImage() {
    // No formato YUV420, a altura total no buffer é 1.5x a altura da imagem
    cv::Mat yuvFrame(480 + 240, 640, CV_8UC1, buffer.data());
    
    // Converte de YUV420 (I420) para BGR (OpenCV padrão)
    if (!yuvFrame.empty()) {
        cv::cvtColor(yuvFrame, frame, cv::COLOR_YUV2BGR_I420);
    }
    fseek(pipePtr, 0, SEEK_END);
    return frame;
}
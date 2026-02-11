#include "../heaters/capture.h"


// Ponteiro para o processo do rpicam
static FILE* pipePtr = nullptr;
// Buffer para armazenar um frame YUV420 (640 * 480 * 1.5)
static std::vector<uchar> buffer(460800);

Capture::Capture(int cameraIndex) : shutter_us(1600) {
    // Comando para rpicam-vid: 
    // -t 0 (infinito), codec yuv420, shutter e gain manuais, output para o pipe (-)
	std::string cmd = "rpicam-still -t 0 --signal --shutter " + std::to_string(shutter_us) + 
			" --gain 4.0 --width 640 --height 480 --nopreview --codec yuv420 -o -";
    
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
    // 1. Envia o sinal SIGUSR1 para o processo rpicam-still
    // Isso "pede" um novo frame para a câmera
    system("pkill -USR1 rpicam-still");

    // 2. Agora o pipe terá exatamente um frame esperando.
    // O fread vai bloquear aqui até que o frame chegue, garantindo sincronia total.
    size_t bytesRead = 0;
    size_t totalRead = 0;
    
    // Garantimos a leitura do frame completo (YUV420)
    while (totalRead < buffer.size()) {
        bytesRead = fread(buffer.data() + totalRead, 1, buffer.size() - totalRead, pipePtr);
        if (bytesRead <= 0) break;
        totalRead += bytesRead;
    }
    
    std::cout << "Frame capturado sob comando! Bytes: " << totalRead << std::endl;
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
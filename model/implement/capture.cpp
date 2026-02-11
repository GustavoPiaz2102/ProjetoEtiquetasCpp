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
    // 1. Limpa qualquer resíduo (lixo) que possa estar no pipe
    // Embora o modo --signal minimize isso, é uma boa prática
    
    // 2. Envia o sinal de trigger
    system("pkill -USR1 rpicam-still");

    // 3. Leitura bloqueante e completa
    size_t totalRead = 0;
    while (totalRead < buffer.size()) {
        size_t bytes = fread(buffer.data() + totalRead, 1, buffer.size() - totalRead, pipePtr);
        if (bytes <= 0) {
             // Se chegar aqui, o rpicam-still pode ter fechado ou falhado
             break; 
        }
        totalRead += bytes;
    }

    if (totalRead < buffer.size()) {
        std::cout << "Aguardando frame... (Verde evitado)" << std::endl;
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
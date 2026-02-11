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
// No construtor Capture::Capture
	std::string cmd = "rpicam-vid -t 0 --shutter " + std::to_string(shutter_us) + 
			" --gain 8.0 --width 640 --height 480 --nopreview --codec mjpeg -o -";
    
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

#include <vector>
#include <opencv2/imgcodecs.hpp> // Essencial para imdecode

// Buffer global ou na classe para armazenar o JPEG
std::vector<uchar> jpegBuffer;

void Capture::captureImage() {
    if (!pipePtr) return;

    // 1. Drenamos os frames antigos lendo rapidamente o que está no pipe
    // Para MJPEG, como queremos o frame mais recente, uma forma simples é
    // ler o buffer até o final antes de processar.
    
    // Vamos ler um bloco grande para garantir que chegamos no frame atual
    static uchar temp[1024 * 100]; // 100KB buffer temporário
    
    // Limpar o excesso (drenagem bruta para evitar lag)
    int fd = fileno(pipePtr);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    while (read(fd, temp, sizeof(temp)) > 0); 
    fcntl(fd, F_SETFL, flags);

    // 2. Captura o próximo frame completo
    // Como o tamanho do JPEG varia, vamos ler até encontrar o fim de um frame
    // ou simplesmente ler um tamanho máximo estimado para 640x480 (~150KB)
    jpegBuffer.resize(150000); 
    size_t n = fread(jpegBuffer.data(), 1, jpegBuffer.size(), pipePtr);
    if (n > 0) jpegBuffer.resize(n);
}

cv::Mat Capture::retrieveImage() {
    if (jpegBuffer.empty()) return cv::Mat();

    // O imdecode é mágico: ele procura o início do JPEG no buffer e ignora o lixo
    cv::Mat decoded = cv::imdecode(jpegBuffer, cv::IMREAD_COLOR);
    
    if (!decoded.empty()) {
        frame = decoded;
    }
    return frame;
}
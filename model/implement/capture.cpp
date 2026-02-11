#include "../heaters/capture.h"


// Ponteiro para o processo do rpicam
static FILE* pipePtr = nullptr;
// Buffer para armazenar um frame YUV420 (640 * 480 * 1.5)
static std::vector<uchar> buffer(460800);

Capture::Capture(int cameraIndex) : shutter_us(800) {
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

#include <poll.h> // Para monitorar o pipe com precisão

void Capture::captureImage() {
    if (!pipePtr) return;
    int fd = fileno(pipePtr);

    // 1. DRENAGEM TOTAL (Vesvaziar o cano)
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    uchar dummy[8192];
    while (read(fd, dummy, sizeof(dummy)) > 0); 

    // 2. SINCRONIZAÇÃO (Esperar o próximo frame "nascer")
    // Usamos poll para esperar até que o pipe tenha dados novos
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    
    // Espera até 100ms por novos dados
    int ret = poll(&pfd, 1, 100);
    
    fcntl(fd, F_SETFL, flags); // Volta para o modo bloqueante

    if (ret <= 0) {
        // Câmera parou de enviar dados ou deu timeout
        return; 
    }

    // 3. LEITURA ATÔMICA
    // Agora que sabemos que o frame acabou de começar a chegar, lemos o bloco inteiro
    size_t totalRead = 0;
    while (totalRead < buffer.size()) {
        size_t n = fread(buffer.data() + totalRead, 1, buffer.size() - totalRead, pipePtr);
        if (n <= 0) break;
        totalRead += n;
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
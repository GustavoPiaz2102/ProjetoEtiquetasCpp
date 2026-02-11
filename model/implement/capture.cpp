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
    if (!pipePtr) return;

    // 1. Pegamos o descritor de arquivo do pipe
    int fd = fileno(pipePtr);

    // 2. Modo Não-Bloqueante: "Se o cano estiver vazio, não fique esperando"
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    // 3. DRENAGEM: Lemos blocos de dados e jogamos fora até o cano secar
    static uchar dummy[4096];
    while (read(fd, dummy, sizeof(dummy)) > 0) {
        // Apenas descartando os frames antigos acumulados
    }

    // 4. Modo Bloqueante: "Agora pare e espere o próximo frame chegar inteiro"
    fcntl(fd, F_SETFL, flags);

    // 5. LEITURA DO FRAME FRESCO:
    // Garantimos que vamos ler exatamente 460.800 bytes (640x480x1.5)
    size_t totalRead = 0;
    while (totalRead < buffer.size()) {
        size_t bytes = fread(buffer.data() + totalRead, 1, buffer.size() - totalRead, pipePtr);
        
        if (bytes <= 0) {
            // Se o pipe fechar ou der erro, saímos para evitar loop infinito
            break; 
        }
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
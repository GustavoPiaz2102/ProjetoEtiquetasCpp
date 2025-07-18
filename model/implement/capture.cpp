#include "../heaters/capture.h"

bool Capture::isRaspberryPi() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("Raspberry Pi") != std::string::npos || line.find("BCM") != std::string::npos) {
            return true;
        }
    }   
    return false;
}

// Função para rodar um comando em background (retorna o pid)
pid_t runCommandBackground(const std::string& cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        // Filho: executa o comando
        execl("/bin/sh", "sh", "-c", cmd.c_str(), nullptr);
        _exit(127); // se execl falhar
    }
    // Pai retorna pid do processo filho
    return pid;
}

Capture::Capture(int cameraIndex) : cap(), frame() {
    if (isRaspberryPi()) {
        std::cout << "Raspberry Pi detectado. Inicializando libcamera-vid com v4l2loopback..." << std::endl;

        // Carrega módulo v4l2loopback se não estiver carregado
        system("sudo modprobe v4l2loopback video_nr=10 card_label=\"CamLoop\" exclusive_caps=1");

        // Roda libcamera-vid enviando para /dev/video10 em background
        // Ajuste a resolução e fps conforme quiser
        std::string cmd = "libcamera-vid -t 0 --width 1280 --height 720 --framerate 30 --codec yuv420 --inline -o /dev/video10 &";

        runCommandBackground(cmd);

        // Dá um tempinho pra a câmera iniciar
        sleep(2);

        // Abre dispositivo /dev/video10 com backend V4L2
        cap.open(10, cv::CAP_V4L2);
    } else {
        std::cout << "Sistema não Raspberry Pi. Abrindo câmera padrão " << cameraIndex << std::endl;
        cap.open(cameraIndex);
    }

    if (!cap.isOpened()) {
        std::cerr << "Erro: Não foi possível abrir a câmera!" << std::endl;
    } else {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, IMG_SZE);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, IMG_SZE2);
    }
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }

    if (isRaspberryPi()) {
        std::cout << "Finalizando libcamera-vid..." << std::endl;
        system("pkill libcamera-vid");
        system("sudo modprobe -r v4l2loopback");
    }
}

cv::Mat Capture::captureImage() {
    if (cap.isOpened()) {
        cap.read(frame);
        if (frame.empty()) {
            std::cerr << "Erro: Frame capturado está vazio!" << std::endl;
        }
    } else {
        std::cerr << "Erro: Câmera não está aberta!" << std::endl;
    }
    return frame;
}

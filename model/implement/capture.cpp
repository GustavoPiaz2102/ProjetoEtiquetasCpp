#include "../heaters/capture.h"
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <fcntl.h>      // Para open()
#include <sys/ioctl.h>  // Para ioctl()
#include <linux/v4l2-subdev.h> // Para V4L2_CID_EXPOSURE
#include <unistd.h>     // Para close()

Capture::Capture(int cameraIndex) : cap() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef __linux__
    // Pipeline simplificado para evitar erro de negociação de 'structure' no IMX500
    // O hardware (PiSP) cuidará do redimensionamento para 640x480
    std::string pipeline = 
        "libcamerasrc ! "
        "video/x-raw, width=640, height=480 ! "
        "videoconvert ! "
        "appsink drop=true max-buffers=1 sync=false";

    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro GStreamer: Falha ao abrir pipeline! Tentando V4L2..." << "\n";
        cap.open(0, cv::CAP_V4L2);
    } else {
        std::cout << "GStreamer: Câmera aberta. Aplicando controles de hardware no IMX500..." << "\n";

        // --- BYPASS PARA CONTROLE DE EXPOSIÇÃO (Kernel Style) ---
        // Abrimos o sub-device do sensor para setar os 830us (54 linhas)
        int fd = open("/dev/v4l-subdev2", O_RDWR);
        if (fd >= 0) {
            struct v4l2_control ctrl;

            // 1. Exposure: 54 linhas conforme calculado para o pixel_rate do sensor
            ctrl.id = 0x00980911; // V4L2_CID_EXPOSURE
            ctrl.value = 54;
            if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
                perror("[ERRO] Falha ao setar Exposure via ioctl");
            }

            // 2. Analogue Gain: Aumentamos para compensar a baixa exposição
            ctrl.id = 0x009e0903; // V4L2_CID_ANALOGUE_GAIN
            ctrl.value = 250;     // Ajuste entre 0 e 978 conforme a luz do local
            if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
                perror("[ERRO] Falha ao setar Analogue Gain via ioctl");
            }

            close(fd);
            std::cout << "[HARDWARE] IMX500 configurado: Exposure=54, Gain=250" << std::endl;
        } else {
            std::cerr << "[AVISO] Nao foi possivel acessar /dev/v4l-subdev2 para ajuste de hardware." << std::endl;
        }
    }
#else
    if (!cap.open(cameraIndex)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo índice!" << "\n";
    } else {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(cv::CAP_PROP_BUFFERSIZE, 1); 
    }
#endif
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat Capture::captureImage() {
    if (!cap.isOpened()) return cv::Mat();

    cv::Mat frame_out;
    if (!cap.read(frame_out)) {
        return cv::Mat();
    }
    
    return frame_out;
}
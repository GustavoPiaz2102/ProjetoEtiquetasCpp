#include "../heaters/capture.h"
#include <opencv2/core/utils/logger.hpp>
#include <iostream>

// Adicione uma flag ou variável membro no header para controlar o debug
// bool save_debug_images = false; 

Capture::Capture(int cameraIndex) : cap() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef __linux__
    // PIPELINE OTIMIZADO PARA BAIXA LATÊNCIA
    // 1. videoscale e videoconvert podem ser pesados. Tentamos pedir o formato direto antes.
    // 2. queue max-size-buffers=1 leaky=downstream: Isso é CRÍTICO. 
    //    Significa: "Se tiver mais de 1 frame na fila, jogue fora o velho e fique só com o novo".
    std::string pipeline = 
        "libcamerasrc auto-exposure=1 ! "
        "video/x-raw, width=640, height=480 ! "
        "videoconvert ! "
        "video/x-raw, format=BGR ! "
        "appsink drop=true max-buffers=1 sync=false";

    if (!cap.open(pipeline, cv::CAP_GSTREAMER)) {
        std::cerr << "Erro GStreamer: Falha ao abrir pipeline!" << "\n";
    } else {
        std::cout << "GStreamer: Câmera aberta (Modo Baixa Latência)!" << "\n";
    }
#else
    if (!cap.open(cameraIndex)) {
        std::cerr << "Erro: Não foi possível abrir a câmera pelo índice!" << "\n";
    } else {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
        // Em Windows/Mac, definimos o buffer size para 1 para evitar lag
        cap.set(cv::CAP_PROP_BUFFERSIZE, 1); 
        std::cout << "Câmera aberta com sucesso pelo índice!" << "\n";
    }
#endif
}

Capture::~Capture() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat Capture::captureImage() {
    if (!cap.isOpened()) {
        std::cerr << "Erro: Câmera não está aberta!" << "\n";
        return cv::Mat();
    }

    cv::Mat frame;

    // DICA DE LIMPEZA DE BUFFER (Opcional, mas útil se o lag persistir):
    // Às vezes o buffer interno do OpenCV segura um frame velho.
    // cap.grab(); // Descarta o frame atual no buffer para forçar a pegar o próximo

    if (!cap.read(frame)) {
        std::cerr << "Erro ao capturar o frame!" << "\n";
        return cv::Mat();
    }

    // --- CORREÇÃO DO BLOQUEIO DE DISCO ---
    // Só salva se realmente necessário. 
    // Se precisar salvar sempre, isso DEVE ser feito em outra thread.
    
    // if (save_debug_images) { 
    //    std::string filename = "./DebugImages/debug_capture(" + std::to_string(debug_counter) + ").jpg";
    //    cv::imwrite(filename, frame); // <--- ISSO TRAVA O PROCESSO POR 100-500ms
    //    std::cout << "Imagem Salva: " << filename << "\n";
    //    debug_counter++;
    // }
    
    return frame;
}
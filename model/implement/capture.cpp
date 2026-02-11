#include "../heaters/capture.h"
#include <iostream>
#include <opencv2/core/utils/logger.hpp>

// Adicione uma flag ou variável membro no header para controlar o debug
// bool save_debug_images = false;

Capture::Capture(int cameraIndex) : cap() {
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

#ifdef __linux__
	// PIPELINE OTIMIZADO PARA BAIXA LATÊNCIA
	// 1. videoscale e videoconvert podem ser pesados. Tentamos pedir o formato
	// direto antes.
	// 2. queue max-size-buffers=1 leaky=downstream: Isso é CRÍTICO.
	//    Significa: "Se tiver mais de 1 frame na fila, jogue fora o velho e fique
	//    só com o novo".

	std::string pipeline =
			"libcamerasrc ! "
			"video/x-raw, width=640, height=480, format=RGBx ! " // Pedindo RGBx que é
								 // eficiente no Pi
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

void Capture::captureImage() {
	if (!cap.isOpened())
		return cv::Mat();

	cv::Mat frame;

	// 1. O 'grab' avisa o hardware para capturar o frame AGORA.
	// Em sistemas de alta velocidade, às vezes usamos dois grabs seguidos
	// para limpar qualquer frame que tenha ficado "no cano" do CSI.
	cap.grab();
}
cv::mat Capture::retrieveImage() {
	if (!cap.retrieve(frame)) {
		std::cerr << "Erro ao decodificar o frame!" << "\n";
		return cv::Mat();
	}
	return frame;
}
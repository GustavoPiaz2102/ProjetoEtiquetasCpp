#include "../heaters/capture.h"

Capture::Capture(int cameraIndex) : cap() {

	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

	if (!cap.open(pipeline, cv::CAP_GSTREAMER)) std::cerr << "Erro GStreamer: Falha ao abrir pipeline!" << "\n";

	else std::cout << "GStreamer: Câmera aberta (Modo Baixa Latência)!" << "\n";

}

Capture::~Capture() {
	if (cap.isOpened()) {
		cap.release();
	}
}

void Capture::captureImage() {
	if (!cap.isOpened())
		return ;

	cv::Mat frame;

	cap.grab(); // grab para capturar o frame no momento e não oque tem no buffer
}
cv::Mat Capture::retrieveImage() {
	if (!cap.retrieve(frame)) {
		std::cerr << "Erro ao decodificar o frame!" << "\n";
		return cv::Mat();
	}
	return frame;
}
#include "../heaters/capture.h"

std::string pipeline =
		"libcamerasrc ! "
		"video/x-raw, width=" + std::to_string(IMG_WIDTH) + ", height=" + std::to_string(IMG_HEIGHT) + ", format=RGBx ! "
		"videoconvert ! "
		"video/x-raw, format=BGR ! "
		"appsink drop=true max-buffers=1 sync=false";

Capture::Capture(int cameraIndex) : cap() ,roi((IMG_WIDTH - ROI_WIDTH) / 2, (IMG_HEIGHT - ROI_HEIGHT) / 2, ROI_WIDTH, ROI_HEIGHT) {

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


	cap.grab(); // grab para capturar o frame no momento e não oque tem no buffer
	
}
cv::Mat Capture::retrieveImage() {
	if (!cap.retrieve(frame)) {
		std::cerr << "Erro ao decodificar o frame!" << "\n";
		return cv::Mat();
	}
	return frame(roi).clone();
}
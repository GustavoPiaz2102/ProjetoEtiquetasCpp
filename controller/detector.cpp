#include "detector.h"

Detector::Detector(Impress &imp, Interface &interface, Validator &validator) : camera(0), ocr("eng"), sensor(21, "gpiochip4"), imp(imp), interface(interface), validator(validator) {
	printer_error = false;
}

Detector::~Detector(){
	StopSensorThread();
	StopProcessThread();
	sensor.ReturnToFirst();
}

void Detector::StartProcessThread(){
	if(processing_running)	return;

	processing_running = true;
	process_thread = std::thread(&Detector::ProcessLoop, this);

	std::cout << "Thread de processamento iniciada.\n";
}

void Detector::StopProcessThread(){
	processing_running = false;

	if(process_thread.joinable()){
		process_thread.join();
		std::cout << "Thread de processamento limpa com sucesso.\n";
	}
}

void Detector::ProcessLoop(){
	while(processing_running){
		if(NewFrameAvailable){
			cv::Mat current_frame;
			bool hasFrame = false;

			{
				std::lock_guard<std::mutex> lock(frame_mutex);

				if(!frame.empty()) {
					current_frame = frame.clone();
					hasFrame = true;
				}
			}

			if(!hasFrame){
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}

			cv::Mat processed = preprocessor.preprocess(current_frame);
			std::string text = ocr.extractText(processed);

			if(!validator.Validate(text)){
				imp.setLastImpress(false);
				LastWithError = true;
			} else LastWithError = false;

			NewFrameAvailable = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	std::cout << "Esperando Pela finalização da thread de processamento na main\n";
	// Interface imprimindo off
}

void Detector::SensorCaptureImpressTHR(){
	sensor.SetStroboHigh(1000);
	while(sensor_running){
		if(sensor.ReadSensor() != 0){
			camera.captureImage();
			//sensor.SetStroboLow();
			cv::Mat newFrame = camera.retrieveImage();

			{
				std::lock_guard<std::mutex> lock(frame_mutex);

				frame = std::move(newFrame);
				NewFrameAvailable = true;
			}

			int error = 0;
			bool printReturn = imp.print(&error);

			if(printReturn){
				if (error == 1) imp.setLastImpress(true);
				//std::cout << "Impressão iniciada com sucesso.\n";
			} else{
				std::cout << "Falha ao iniciar a impressão! Parando thread." << "\n";

				printer_error = true;
				sensor_running = false;
				imp.setLastImpress(true);
			}
		}
	}

	sensor.SetStroboLow();
	sensor.ReturnToFirst();
	imp.ResetLastImpress();

	//std::cout << sensor.firstRead << std::endl;
	//std::cout << "Imprimindo:" << interface.GetImprimindo() << std::endl;
	//std::cout << "Esperando Pela finalização da thread na main\n";
}

cv::Mat Detector::GetFrame(){
	std::lock_guard<std::mutex> lock(frame_mutex);

	if (frame.empty()) return cv::Mat();

	return frame.clone();
}

void Detector::StartSensorThread(){
	if (sensor_running) {
		std::cout << "Thread já está rodando!\n";
		return;
	}

	if (sensor_thread.joinable()) sensor_thread.join();

	// Resetando as flags
	LastWithError = false;
	NewFrameAvailable = false;
	printer_error = false;
	sensor.ReturnToFirst();

	sensor_running = true;
	sensor_thread = std::thread(&Detector::SensorCaptureImpressTHR, this);
	std::cout << "Thread de captura e impressão iniciada.\n";
}

void Detector::StopSensorThread(){
	sensor_running = false;

	if (sensor_thread.joinable()) {
		sensor_thread.join();
		std::cout << "Thread de captura limpa com sucesso.\n";
	}
}

std::string Detector::RunProcess(){
	cv::Mat current_frame;

	{
		std::lock_guard<std::mutex> lock(frame_mutex);
		if (frame.empty()) return "";
		current_frame = frame.clone();
	}

	cv::Mat processed = preprocessor.preprocess(current_frame);
	std::string text = ocr.extractText(processed);
	return text;
} 
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

	setThreadPriority(process_thread, 99);
	setThreadAffinity(process_thread, {0,1,2,3});

	std::cout << "Thread de processamento iniciada.\n";
}

void Detector::StopProcessThread(){
	processing_running = false;
	frame_cv.notify_all();
	if(process_thread.joinable()){
		process_thread.join();
		std::cout << "Thread de processamento limpa com sucesso.\n";
	}
}

void Detector::ProcessLoop(){
	while(processing_running){
		cv::Mat current_frame;

		{
			std::unique_lock<std::mutex> lock(frame_mutex);

			frame_cv.wait(lock, [this]{ return NewFrameAvailable || !processing_running; });
			
			if(!processing_running) break;

			current_frame = frame.clone();
			NewFrameAvailable = false;
		}

		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		cv::Mat processed = preprocessor.preprocess(current_frame);
		std::string text = ocr.extractText(processed);
		std::cout << "Tempo de processamento: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms\n";
		std::cout << "Texto Detectado: " << text << std::endl;

		if(!validator.Validate(text)){
			imp.setLastImpress(false);
			LastWithError = true;
		} else LastWithError = false;
	}

	std::cout << "Esperando Pela finalização da thread de processamento na main\n";
	// Interface imprimindo off
}

void Detector::SensorCaptureImpressTHR(){
	sensor.SetStroboHigh(1000);
	while(sensor_running){
		if(sensor.ReadSensor() != 0){
			if(!NewFrameAvailable){
				camera.captureImage();
				cv::Mat newFrame = camera.retrieveImage();

				{
					std::lock_guard<std::mutex> lock(frame_mutex);

					frame = std::move(newFrame);
					NewFrameAvailable = true;
				}

				frame_cv.notify_one();

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
			} else std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
	//setThreadPriority(sensor_thread, 98);
	//setThreadAffinity(sensor_thread, {1});
	std::cout << "Thread de captura e impressão iniciada.\n";
}

void Detector::StopSensorThread(){
	sensor_running = false;

	if (sensor_thread.joinable()) {
		sensor_thread.join();
		std::cout << "Thread de captura limpa com sucesso.\n";
	}
}
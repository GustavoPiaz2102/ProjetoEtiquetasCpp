#include "detector.h"
#include "../view/interface.h"

Detector::Detector(Impress &imp, Interface &interface, Validator &validator, Arquiver &arquiver) : camera(0), ocr("/home/pi/models"), sensor(), strobo(21), buzzer(0, 0), imp(imp), interface(interface), validator(validator), arquiver(arquiver) {
	/*sensor.configureScale(arquiver.dict["sensorThreshold"] != "" ? std::stoi(arquiver.dict["sensorThreshold"]) : SENSOR_THRESHOLD,
						  arquiver.dict["debounceMs"] != "" ? std::stoi(arquiver.dict["debounceMs"]) : DEBOUNCE_MS);
	*/
	printer_error = false;
}

Detector::~Detector() {
	StopSensorThread();
	StopProcessThread();
	sensor.ReturnToFirst();
}

void Detector::StartProcessThread() {
	if (processing_running)
		return;

	processing_running = true;
	process_thread = std::thread(&Detector::ProcessLoop, this);

	std::cout << "Thread de processamento iniciada.\n";
}

cv::Mat Detector::GetFrame() {
	std::lock_guard<std::mutex> lock(frame_mutex);

	if (frame.empty())
		return cv::Mat();

	return frame.clone();
}

void Detector::StartSensorThread() {
	if (sensor_running) {
		std::cout << "Thread já está rodando!\n";
		return;
	}

	if (sensor_thread.joinable())
		sensor_thread.join();

	// Resetando as flags
	LastWithError = false;
	NewFrameAvailable = false;
	printer_error = false;
	sensor.ReturnToFirst();

	sensor_running = true;
	sensor_thread = std::thread(&Detector::SensorCaptureImpressTHR, this);
	std::cout << "Thread de captura e impressão iniciada.\n";
}

void Detector::ProcessLoop() {
	while (processing_running) {
		cv::Mat current_frame;

		{
			std::unique_lock<std::mutex> lock(frame_mutex);

			frame_cv.wait(lock, [this] { return NewFrameAvailable || !processing_running; });

			if (!processing_running)
				break;

			NewFrameAvailable = false;
			current_frame = frame.clone();
		}

		cv::Mat processed = preprocessor.preprocess(current_frame);
		std::string text = ocr.extractText(processed, current_frame);
		std::cout << "Texto Detectado: " << text << std::endl;

		buzzer.setBuzzerActive(false);
		if (!validator.Validate(text)) {
			imp.setLastImpress(false);
			LastWithError = true;
			buzzer.setBuzzerActive(true);
			//std::cout << ocr.getLastDebugError();
		} else {
			LastWithError = false;
			correctImpressCounter++;
		}
		totalImpressCounter++;
		sensor_cv.notify_one();
	}
	buzzer.setBuzzerActive(false);

	std::cout << "Esperando Pela finalização da thread de processamento na main\n";
}

void Detector::counterPrint() {
	std::cout << "Porcentagem de impressões corretas: " << (totalImpressCounter > 0 ? (static_cast<float>(correctImpressCounter) / totalImpressCounter) * 100.0f : 0.0f) << "%\n";
	totalImpressCounter = 0;
	correctImpressCounter = 0;
}

void Detector::SensorCaptureImpressTHR() {
	strobo.SetStroboLow();

	while (sensor_running) {
		if (sensor.ReadSensor() || firstDet) {
			strobo.SetStroboHigh(100);
			camera.captureImage();
			cv::Mat newFrame = camera.retrieveImage();
			strobo.SetStroboLow();

			{
				std::unique_lock<std::mutex> lock(frame_mutex);
				sensor_cv.wait(lock, [this] { return !NewFrameAvailable || !sensor_running; });

				if (!sensor_running)
					break;

				frame = std::move(newFrame);
				interface.setFrameCount(interface.getFrameCount() + 1);
				NewFrameAvailable = true;
			}
			frame_cv.notify_one();

			if (!imp.print(this->firstDet)) {
				std::cout << "Falha ao iniciar a impressão! Parando thread." << "\n";

				printer_error = true;
				sensor_running = false;
				imp.ResetLastImpress();
			}

			if (firstDet) {
				validator.printall();
				firstDet = false;
			}
		}
	}

	strobo.SetStroboLow();
	sensor.ReturnToFirst();
	imp.ResetLastImpress();
}

void Detector::StopProcessThread() {
	processing_running = false;
	frame_cv.notify_all();
	if (process_thread.joinable()) {
		process_thread.join();
		std::cout << "Thread de processamento limpa com sucesso.\n";
	}
}

void Detector::StopSensorThread() {
	sensor_running = false;
	sensor_cv.notify_all();
	if (sensor_thread.joinable()) {
		sensor_thread.join();
		std::cout << "Thread de captura limpa com sucesso.\n";
	}
}
void Detector::configureSensor() {
	sensor.configureScale(arquiver.dict["sensorThreshold"] != "" ? std::stoi(arquiver.dict["sensorThreshold"]) : SENSOR_THRESHOLD,
						  arquiver.dict["debounceMs"] != "" ? std::stoi(arquiver.dict["debounceMs"]) : DEBOUNCE_MS);
}

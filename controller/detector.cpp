#include "detector.h"

Detector::Detector(Impress &imp, Interface &interface, Validator &validator)
    : camera(0), ocr("eng"), sensor(19, 18,"gpiochip4"), imp(imp), interface(interface), validator(validator) {}

Detector::~Detector()
{
}

void Detector::StartProcessThread()
{
    if (processing_running)
        return;
    processing_running = true;
    process_thread = std::thread(&Detector::ProcessLoop, this);
}

void Detector::StopProcessThread()
{
    if (!processing_running)
        return;
    processing_running = false;
    if (process_thread.joinable())
        process_thread.join();
}

void Detector::ProcessLoop()
{
    while (processing_running)
    {
        if (NewFrameAvailable)
        {
            cv::Mat current_frame;
            bool hasFrame = false;

            {
                std::lock_guard<std::mutex> lock(frame_mutex);
                if (!frame.empty())
                {
                    current_frame = frame.clone();
                    hasFrame = true;
                }
            } // mutex liberado aqui

            if (!hasFrame)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            cv::Mat processed = preprocessor.preprocess(current_frame);
            std::string text = ocr.extractText(processed);

            if (!validator.Validate(text))
            {
                imp.setLastImpress(false);
                LastWithError = true;
            }
            else
            {
                LastWithError = false;
            }

            NewFrameAvailable = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Detector::SensorCaptureImpressTHR()
{
    while (running)
    {
        if (sensor.ReadSensor() == 0)
        {
            sensor.SetStroboHigh();
            cv::Mat newFrame = camera.captureImage();
            sensor.SetStroboLow();

            {
                std::lock_guard<std::mutex> lock(frame_mutex);
                frame = std::move(newFrame);
                NewFrameAvailable = true;

                bool sucesso = imp.print();
                if (sucesso)
                {
                    std::cout << "Impressão iniciada com sucesso!" << "\n";
                }
                else
                {

                imp.setLastImpress(true);

                if (imp.getQntImpressao() <= 0)
                {
                    interface.setImprimindo(false);
                    running = false; // ✅ pode parar o loop
                }
                else
                {
                    imp.setLastImpress(true);
                }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    std::cout << "Thread de captura e impressão encerrada.\n";
}
cv::Mat Detector::GetFrame()
{
    {
        std::lock_guard<std::mutex> lock(frame_mutex);
        return frame.clone();
    }
}
void Detector::StartSensorThread()
{
    if (running)
    {
        std::cout << "Thread já está rodando!\n";
        return;
    }

    running = true;
    sensor_thread = std::thread(&Detector::SensorCaptureImpressTHR, this);
    std::cout << "Thread de captura e impressão iniciada.\n";
}

void Detector::StopSensorThread()
{
    if (!running)
        return;

    running = false;
    if (sensor_thread.joinable())
    {
        sensor_thread.join();
        std::cout << "Thread de captura e impressão finalizada.\n";
    }
}

std::string Detector::RunProcess()
{
    cv::Mat current_frame;
    {
        std::lock_guard<std::mutex> lock(frame_mutex);
        if (frame.empty())
            return "";
        current_frame = frame.clone();
    }
    cv::Mat processed = preprocessor.preprocess(current_frame);
    std::string text = ocr.extractText(processed);
    return text;
}
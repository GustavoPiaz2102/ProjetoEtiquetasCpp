#include "detector.h"

Detector::Detector(Impress &imp, Interface &interface, Validator &validator)
    : camera(0), ocr("eng"), sensor(27, 18, "gpiochip4"), imp(imp), interface(interface), validator(validator) 
{
    // Inicializa flags de forma segura
    printer_error = false;
}

Detector::~Detector()
{
    StopSensorThread();
    StopProcessThread();
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
    // Sinaliza parada
    processing_running = false;
    
    // CORREÇÃO CRÍTICA: Sempre tenta o join se for possível,
    // independente do valor da flag
    if (process_thread.joinable())
    {
        process_thread.join();
    }
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

// No seu Detector.cpp
void Detector::SensorCaptureImpressTHR()
{
    bool jaImprimiuNesteCiclo = false; // Variável para controle de borda

    while (running)
    {
        int estadoSensor = sensor.ReadSensor();

        // Só tenta imprimir se o sensor disparar E ainda houver etiquetas pendentes
        if (estadoSensor != 0 && !jaImprimiuNesteCiclo && imp.getQntImpressao() > 0)
        {
            sensor.SetStroboHigh();
            cv::Mat newFrame = camera.captureImage();
            sensor.SetStroboLow();

            {
                std::lock_guard<std::mutex> lock(frame_mutex);
                frame = std::move(newFrame);
                NewFrameAvailable = true;
            }

            if (imp.print()) {
                std::cout << "Etiqueta impressa! Restam: " << imp.getQntImpressao() << "\n";
                jaImprimiuNesteCiclo = true; // Bloqueia novas impressões até o sensor resetar
            } else {
                printer_error = true; 
                running = false; 
            }
        } 
        // Se o sensor voltar a 0, liberamos para a próxima etiqueta
        else if (estadoSensor == 0) 
        {
            jaImprimiuNesteCiclo = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

cv::Mat Detector::GetFrame()
{
    std::lock_guard<std::mutex> lock(frame_mutex);
    if (frame.empty()) return cv::Mat(); // Retorna vazio se não houver frame
    return frame.clone();
}

void Detector::StartSensorThread()
{
    if (running)
    {
        std::cout << "Thread já está rodando!\n";
        return;
    }

    // Limpeza de segurança caso tenha sobrado lixo anterior
    if (sensor_thread.joinable()) {
        sensor_thread.join();
    }

    running = true;
    printer_error = false; // Reseta flag de erro ao iniciar
    sensor_thread = std::thread(&Detector::SensorCaptureImpressTHR, this);
    std::cout << "Thread de captura e impressão iniciada.\n";
}

void Detector::StopSensorThread()
{
    // 1. Garante que o loop pare
    running = false;

    // 2. CORREÇÃO CRÍTICA: Sempre verifica se precisa limpar a memória.
    // Removemos o "if (!running) return" que causava o crash.
    if (sensor_thread.joinable())
    {
        sensor_thread.join();
        std::cout << "Thread de captura limpa com sucesso.\n";
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
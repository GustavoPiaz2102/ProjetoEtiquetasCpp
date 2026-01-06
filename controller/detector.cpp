#include "detector.h"

Detector::Detector(Impress &imp, Interface &interface, Validator &validator)
    : camera(0), ocr("eng"), sensor(19, 18, "gpiochip4"), imp(imp), interface(interface), validator(validator) 
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
    std::cout << "Esperando Pela finalização da thread de processamento na main\n";
    //Interface imprimindo off

    while (true) std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Detector::SensorCaptureImpressTHR()
{
    while (running)
    {
        if (sensor.ReadSensor() != 0)
        {
            sensor.SetStroboHigh();
            cv::Mat newFrame = camera.captureImage();
            sensor.SetStroboLow();

            {
                std::lock_guard<std::mutex> lock(frame_mutex);
                
                // CORREÇÃO: Atualizamos o frame ANTES de tentar imprimir.
                // Isso garante que a imagem apareça na tela mesmo se a impressora falhar.
                frame = std::move(newFrame);
                NewFrameAvailable = true;
            }

            // Agora tentamos imprimir
            int error = 0;
            bool printReturn = imp.print(error);
            //Caso precise saber o erro usar a variavel
           if (printReturn)
            {
                if (error == 1){
                    imp.setLastImpress(true);
                }
                //Se o erro for 1 foi erro de detecção apenas então desconsidera
=                std::cout << "Impressão iniciada com sucesso.\n";
            }
            else
            {
                // CORREÇÃO: Lógica segura de erro.
                std::cout << "Falha ao iniciar a impressão! Parando thread." << "\n";
                
                // 1. Sinaliza erro para a thread principal (Controller)
                printer_error = true; 

                // 2. Para o loop desta thread imediatamente
                running = false; 
                
                // 3. Marca erro na impressão
                imp.setLastImpress(true);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    std::cout << "Esperando Pela finalização da thread na main\n";
    while (true) std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
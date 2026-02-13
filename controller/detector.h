#ifndef DETECTOR_H
#define DETECTOR_H

#include "../model/heaters/capture.h"
#include "../model/heaters/preprocessor.h"
#include "../model/heaters/OCR.h"
#include "../view/interface.h"
#include <opencv2/opencv.hpp>
#include "../model/heaters/GPIO.h"
#include "../model/heaters/impress.h"
#include "../model/heaters/validator.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <functional>

class Detector{
	private:
		Capture camera;
		Preprocessor preprocessor;
		OCR ocr;
		GPIO sensor;
		Impress &imp;
		Interface &interface;
		Validator &validator;


		cv::Mat frame;                                                      // Frame capturado da câmera
		std::mutex frame_mutex;                                             // Protege acesso ao frame
		std::thread sensor_thread;                                          // Thread de captura e impressão
		std::thread process_thread;                                         // Thread de processamento (RunProcess)  
		
		// Flags //

		std::atomic<bool> NewFrameAvailable{false}; 
		std::atomic<bool> LastWithError{false}; // Flag da validator, indica que o OCR achou um erro
		std::function<void(bool, const std::string &)> validationCallback;  // Callback para notificação de validação
		std::atomic<bool> printer_error{false};
		
		std::atomic<bool> sensor_running{false};                                   // Controle da thread de captura
		std::atomic<bool> processing_running{false};                        // Controle da thread de processamento

	public:
		/** @brief Construtor da classe Detector que inicializa todos os componentes do sistema.
		* @details Inicializa a câmera, OCR, sensor GPIO e mantém referências para impressora, interface e validador.
		* @param[in] imp Referência para o objeto Impress responsável pela impressão de etiquetas.
		* @param[in] interface Referência para o objeto Interface responsável pela UI.
		* @param[in] validator Referência para o objeto Validator responsável pela validação de códigos.
		*/
		Detector(Impress &imp, Interface &interface, Validator &validator);

		/** @brief Destrutor da classe Detector que garante o encerramento seguro das threads.
		* @details Para ambas as threads (sensor e processamento) antes da destruição do objeto.
		*/
		~Detector();

		/** @brief Processa um frame atual e extrai texto usando OCR.
		* @details Captura o frame atual de forma thread-safe, aplica pré-processamento e executa OCR para extrair texto.
		*          Esta função é síncrona e pode ser chamada diretamente.
		* @return std::string Texto extraído do frame. Retorna string vazia se não houver frame ou OCR falhar.
		*/
		std::string RunProcess();

		/** @brief Loop contínuo de processamento executado pela thread de processamento.
		* @details Monitora continuamente por novos frames disponíveis, processa com OCR e valida os textos detectados.
		*          Notifica o controller via callback sobre o resultado da validação.
		*          Executa em loop até que processing_running seja false.
		*/
		void ProcessLoop();

		/** @brief Thread de captura contínua via sensor GPIO e controle de impressão.
		* @details Monitora o sensor continuamente, captura frames quando o sensor é ativado e gerencia o processo de impressão.
		*          Sinaliza a disponibilidade de novos frames para a thread de processamento.
		*          Controla o estado de execução baseado no sensor e na interface.
		*/
		void SensorCaptureImpressTHR();

		/** @brief Inicia a thread de captura do sensor se não estiver em execução.
		* @details Cria e inicia a thread SensorCaptureImpressTHR em segundo plano.
		*          Garante que apenas uma instância da thread esteja ativa.
		*/
		void StartSensorThread();

		/** @brief Para a thread de captura do sensor de forma segura.
		* @details Sinaliza para a thread parar e aguarda sua finalização.
		*          Garante limpeza adequada dos recursos da thread.
		*/
		void StopSensorThread();

		/** @brief Inicia a thread de processamento contínuo se não estiver em execução.
		* @details Cria e inicia a thread ProcessLoop em segundo plano.
		*          Responsável pelo processamento OCR e validação contínuos.
		*/
		void StartProcessThread();

		/** @brief Para a thread de processamento de forma segura.
		* @details Sinaliza para a thread de processamento parar e aguarda sua finalização.
		*          Garante que o processamento atual seja completado antes de parar.
		*/
		void StopProcessThread();

		/** @brief Obtém uma cópia segura do frame atual para exibição na interface.
		* @details Utiliza mutex para acesso thread-safe ao frame, garantindo que a cópia seja consistente.
		* @return cv::Mat Cópia do frame atual capturado pela câmera.
		*/
		cv::Mat GetFrame();

		/** @brief Verifica se a última operação de processamento resultou em erro de validação.
		* @details Utilizado pela interface para determinar se deve exibir indicadores de erro.
		* @return bool True se a última validação falhou, False caso contrário.
		*/
		bool WasLastWithError() const{
		return LastWithError;
		}

		/** @brief Configura o callback para notificação de resultados de validação.
		* @details Permite que o Controller seja notificado quando um código for validado (com sucesso ou erro).
		*          O callback recebe dois parâmetros: isValid (bool) e text (string com o código detectado).
		* @param[in] callback Função callback com signature void(bool, const std::string&)
		*/
		void setValidationCallback(std::function<void(bool, const std::string &)> callback){
		validationCallback = callback;
		}

		/** @brief Obtém o estado de execução da thread de captura.
		* @return bool True se a thread de captura estiver em execução, False caso contrário.
		*/                                                                     
		bool GetRunning() const{
		return running;
		}

		/** @brief Obtém o estado de execução da thread de processamento.
		* @return bool True se a thread de processamento estiver em execução, False caso contrário.
		*/
		bool GetProcessingRunning() const{
		return processing_running;
		}

		/** @brief Verifica se houve erro crítico na impressora.
		* @details Utilizado pelo Controller para detectar erros de impressão e notificar a interface.
		*          Reseta a flag de erro após a verificação.
		* @return bool True se houve erro crítico, False caso contrário.
		*/
		bool HasPrinterError(){               
		if(printer_error){
			printer_error = false;
			return true;
		}

		return false;
		}
};

#endif // DETECTOR_H
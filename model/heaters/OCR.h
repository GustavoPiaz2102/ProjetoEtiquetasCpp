#ifndef OCR_H
#define OCR_H

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <future>

#define MODEL_DIR "/home/pi/models"

/**
 * @brief Motor de OCR baseado em PaddleOCR PP-OCRv3 via ONNX Runtime.
 * @details Realiza detecção e reconhecimento de texto em imagens utilizando dois modelos:
 *          um detector (det) que localiza regiões de texto e um reconhecedor (rec) que extrai o texto.
 *          O reconhecimento das 3 linhas é feito em paralelo para maximizar performance.
 *          Opera exclusivamente sobre os caracteres definidos na whitelist.
 */
class OCR {
	private:
		Ort::Env env;
		Ort::SessionOptions sessionOptions;
		std::unique_ptr<Ort::Session> detSession;
		std::unique_ptr<Ort::Session> recSession;

		std::vector<std::string> charset;
		float minConfidence = 90.0f;

		/**
		 * @brief Detecta regiões de texto na imagem pré-processada.
		 * @details Utiliza o modelo det para gerar um mapa de probabilidade,
		 *          binariza e extrai bounding boxes ordenadas de cima para baixo.
		 * @param[in] detImg Imagem normalizada CV_32FC3 [-1,1] vinda do Preprocessor::preprocess().
		 * @return Vector de cv::Rect com as bounding boxes detectadas.
		 */
		std::vector<cv::Rect> detect(const cv::Mat& detImg);

		/**
		 * @brief Reconhece o texto em um recorte de linha.
		 * @details Redimensiona o recorte para altura 48px, monta o tensor CHW
		 *          e executa o modelo rec. Decodifica a saída via CTC greedy decode.
		 *          Thread-safe — pode ser chamado simultaneamente por múltiplas threads.
		 * @param[in] lineImg Recorte de uma linha normalizado CV_32FC3 [-1,1].
		 * @return String com o texto reconhecido na linha.
		 */
		std::string recognize(const cv::Mat& lineImg);

		/**
		 * @brief Constrói tensor CHW [1,3,H,W] a partir de um cv::Mat CV_32FC3.
		 * @param[in]  img  Imagem float normalizada.
		 * @param[out] outH Altura da imagem.
		 * @param[out] outW Largura da imagem.
		 * @return Vector<float> com os dados no formato channel-first.
		 */
		std::vector<float> buildTensor(const cv::Mat& img, int& outH, int& outW);

		/**
		 * @brief Decodifica a saída CTC do modelo rec em string.
		 * @details Aplica greedy decode: argmax por timestep, remove repetições e blanks.
		 *          Filtra por confiança mínima e whitelist de caracteres permitidos.
		 * @param[in] logits     Ponteiro para os logits de saída do modelo (já em softmax).
		 * @param[in] timeSteps  Número de timesteps na saída.
		 * @param[in] numClasses Número de classes (tamanho do charset + 1 blank).
		 * @return String com o texto decodificado.
		 */
		std::string ctcDecode(const float* logits, int timeSteps, int numClasses);

		/**
		 * @brief Carrega o charset a partir do arquivo de chaves do PaddleOCR.
		 * @details Insere "blank" no índice 0 e espaço no último índice,
		 *          conforme a convenção do PP-OCRv3.
		 * @param[in] keysPath Caminho para o arquivo ppocr_keys_v1.txt.
		 */
		void loadCharset(const std::string& keysPath);

	public:
		/**
		 * @brief Construtor que carrega os modelos det e rec do diretório especificado.
		 * @details Inicializa as sessões ONNX Runtime com 4 threads e otimização máxima.
		 *          Carrega o charset do arquivo de chaves.
		 * @param[in] modelDir Caminho para o diretório contendo os modelos .onnx e o arquivo de chaves.
		 */
		OCR(const std::string& modelDir = MODEL_DIR);

		/** @brief Destrutor padrão. */
		~OCR() = default;

		/**
		 * @brief Executa o pipeline completo de OCR: detecção + reconhecimento paralelo.
		 * @details Detecta bounding boxes usando detImg, recorta cada linha de origImg
		 *          e reconhece as linhas em paralelo usando std::async.
		 *          O tempo total do rec é reduzido ao tempo de uma única linha (~117ms).
		 * @param[in] detImg  Saída do Preprocessor::preprocess() — binarizado e normalizado [-1,1].
		 * @param[in] origImg Imagem original BGR para recorte das linhas detectadas.
		 * @return String com o texto extraído, uma linha por '\\n', na ordem top→bottom.
		 */
		std::string extractText(const cv::Mat& detImg, const cv::Mat& origImg);
};

#endif // OCR_H
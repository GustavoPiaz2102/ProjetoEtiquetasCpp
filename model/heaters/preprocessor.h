#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <opencv2/opencv.hpp>

/**
 * @brief Responsável pelo pré-processamento de imagens para o pipeline de OCR.
 * @details Fornece dois métodos de preparação: um para o detector (com binarização)
 *          e outro para o reconhecedor (apenas normalização, preservando textura natural).
 */
class Preprocessor {
	public:
		/**
		 * @brief Construtor padrão.
		 */
		Preprocessor();

		/**
		 * @brief Destrutor padrão.
		 */
		~Preprocessor();

		/**
		 * @brief Prepara a imagem para o modelo detector (det).
		 * @details Converte para escala de cinza, aplica binarização de Otsu,
		 *          garante fundo claro (inverte se necessário), converte para RGB
		 *          e normaliza para [-1, 1].
		 * @param[in] inputImage Imagem de entrada BGR ou grayscale.
		 * @return cv::Mat CV_32FC3 normalizado em [-1, 1], pronto para o detector.
		 */
		cv::Mat preprocess(const cv::Mat& inputImage);

		/**
		 * @brief Prepara um recorte de linha para o modelo reconhecedor (rec).
		 * @details Converte para RGB e normaliza para [-1, 1] sem binarização,
		 *          preservando a textura natural da imagem para melhor reconhecimento.
		 * @param[in] inputImage Recorte de linha BGR ou grayscale.
		 * @return cv::Mat CV_32FC3 normalizado em [-1, 1], pronto para o reconhecedor.
		 */
		cv::Mat prepareForRec(const cv::Mat& inputImage);
};

#endif // PREPROCESSOR_H
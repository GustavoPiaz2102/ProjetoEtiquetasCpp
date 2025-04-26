#include <opencv2/opencv.hpp>
#include <vector>
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
using namespace cv;
using namespace std;

class Preprocessor {
public:
    // Construtor
    Preprocessor() = default;

    // Pré-processa a imagem para OCR
    Mat process(const Mat& inputImage, bool denoise = true, bool deskew = true) {
        if (inputImage.empty()) {
            throw runtime_error("Imagem de entrada vazia!");
        }

        Mat processedImage = inputImage.clone();

        // Converter para escala de cinza se necessário
        if (processedImage.channels() > 1) {
            cvtColor(processedImage, processedImage, COLOR_BGR2GRAY);
        }

        // Redução de ruído
        if (denoise) {
            applyDenoising(processedImage);
        }

        // Correção de inclinação
        if (deskew) {
            processedImage = correctSkew(processedImage);
        }

        // Binarização
        applyBinarization(processedImage);

        // Melhorar contraste
        enhanceContrast(processedImage);

        return processedImage;
    }

private:
    // Aplicar binarização adaptativa
    void applyBinarization(Mat& image) {
        // Usar limiarização adaptativa para lidar com diferentes condições de iluminação
        adaptiveThreshold(image, image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, 
                         THRESH_BINARY, 11, 2);
    }

    // Redução de ruído
    void applyDenoising(Mat& image) {
        // Aplicar desfoque gaussiano leve para reduzir ruído
        GaussianBlur(image, image, Size(3, 3), 0);

        // Alternativa: filtro bilateral que preserva bordas
        // bilateralFilter(image, image, 5, 75, 75);
    }

    // Melhorar contraste
    void enhanceContrast(Mat& image) {
        // Equalização de histograma para melhorar contraste
        equalizeHist(image, image);
    }

    // Correção de inclinação (deskew)
    Mat correctSkew(const Mat& image) {
        // Criar uma imagem binária invertida para encontrar contornos
        Mat binary;
        threshold(image, binary, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

        // Encontrar contornos
        vector<Point> points;
        findNonZero(binary, points);

        // Calcular o retângulo rotacionado que engloba todos os pontos
        RotatedRect box = minAreaRect(points);
        double angle = box.angle;

        if (angle < -45.) {
            angle += 90.;
        }

        // Rotacionar a imagem para corrigir a inclinação
        Point2f center(image.cols / 2.f, image.rows / 2.f);
        Mat rotationMat = getRotationMatrix2D(center, angle, 1.0);
        Mat rotated;
        warpAffine(image, rotated, rotationMat, image.size(), 
                  INTER_CUBIC, BORDER_REPLICATE);

        return rotated;
    }
};
#endif
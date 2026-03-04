#include "../heaters/OCR.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstring>

// Apenas estes caracteres serão retornados — equivalente ao whitelist do Tesseract original
static const std::string WHITELIST = "0123456789/:LFV";

static bool inWhitelist(const std::string& ch) {
	return WHITELIST.find(ch) != std::string::npos;
}

OCR::OCR(const std::string& modelDir)
	: env(ORT_LOGGING_LEVEL_WARNING, "OCR")
{
	sessionOptions.SetIntraOpNumThreads(4);
	sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

	std::string modelPath = modelDir + "/en_PP-OCRv3_rec_infer.onnx";
	std::string keysPath  = modelDir + "/ppocr_keys_v1.txt";

	session = std::make_unique<Ort::Session>(env, modelPath.c_str(), sessionOptions);
	loadCharset(keysPath);

	std::cout << "[OCR] Modelo carregado: " << modelPath << "\n";
	std::cout << "[OCR] Charset: " << charset.size() << " entradas\n";
}

void OCR::loadCharset(const std::string& keysPath) {
	std::ifstream f(keysPath);
	if (!f.is_open())
		throw std::runtime_error("[OCR] Não foi possível abrir: " + keysPath);

	charset.clear();
	charset.push_back("blank"); // índice 0 = CTC blank

	std::string line;
	while (std::getline(f, line)) {
		if (!line.empty())
			charset.push_back(line);
	}
	charset.push_back(" "); // último índice = espaço (convenção PaddleOCR)
}

// Recebe cv::Mat CV_32FC3 normalizado [-1,1] vindo do Preprocessor
// Monta tensor CHW [1, 3, H, W] para o ONNX Runtime
std::vector<float> OCR::buildTensor(const cv::Mat& img) {
	int H = img.rows;
	int W = img.cols;

	std::vector<cv::Mat> channels(3);
	cv::split(img, channels);

	std::vector<float> tensor(3 * H * W);
	for (int c = 0; c < 3; ++c) {
		std::memcpy(
			tensor.data() + c * H * W,
			channels[c].ptr<float>(),
			H * W * sizeof(float)
		);
	}
	return tensor;
}

std::string OCR::ctcDecode(const float* logits, int timeSteps, int numClasses) {
	std::string result;
	int lastIdx = -1;

	for (int t = 0; t < timeSteps; ++t) {
		const float* row = logits + t * numClasses;

		// Softmax → confiança em escala 0–100
		float maxVal = *std::max_element(row, row + numClasses);
		std::vector<float> probs(numClasses);
		float sum = 0.0f;
		for (int i = 0; i < numClasses; ++i) {
			probs[i] = std::exp(row[i] - maxVal);
			sum += probs[i];
		}

		int maxIdx = static_cast<int>(
			std::max_element(probs.begin(), probs.end()) - probs.begin()
		);
		float confidence = (probs[maxIdx] / sum) * 100.0f;

		if (maxIdx != lastIdx && maxIdx != 0) { // 0 = CTC blank
			if (maxIdx < static_cast<int>(charset.size())) {
				const std::string& ch = charset[maxIdx];
				if (confidence >= minConfidence && inWhitelist(ch))
					result += ch;
			}
		}
		lastIdx = maxIdx;
	}

	return result;
}

std::string OCR::extractText(const cv::Mat& inputImage) {
	if (inputImage.empty()) {
		std::cerr << "[OCR] Erro: imagem vazia passada para OCR.\n";
		return "";
	}

	// inputImage já vem normalizado CV_32FC3 do Preprocessor
	int H = inputImage.rows;
	int W = inputImage.cols;

	std::vector<float> inputTensor = buildTensor(inputImage);
	std::vector<int64_t> inputShape = {1, 3, H, W};

	Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(
		OrtArenaAllocator, OrtMemTypeDefault
	);
	Ort::Value inputOrt = Ort::Value::CreateTensor<float>(
		memInfo,
		inputTensor.data(), inputTensor.size(),
		inputShape.data(),  inputShape.size()
	);

	const char* inputNames[]  = {"x"};
	const char* outputNames[] = {"softmax_0.tmp_0"};

	auto outputs = session->Run(
		Ort::RunOptions{nullptr},
		inputNames,  &inputOrt, 1,
		outputNames, 1
	);

	auto& out      = outputs[0];
	auto  shape    = out.GetTensorTypeAndShapeInfo().GetShape();
	int timeSteps  = static_cast<int>(shape[1]);
	int numClasses = static_cast<int>(shape[2]);

	return ctcDecode(out.GetTensorData<float>(), timeSteps, numClasses);
}
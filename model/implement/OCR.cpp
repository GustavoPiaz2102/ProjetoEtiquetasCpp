#include "../heaters/OCR.h"
#include "../heaters/preprocessor.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>

static const std::string WHITELIST = "0123456789/:LFV";

static bool inWhitelist(const std::string& ch) {
	return WHITELIST.find(ch) != std::string::npos;
}

OCR::OCR(const std::string& modelDir)
	: env(ORT_LOGGING_LEVEL_WARNING, "OCR")
{
	sessionOptions.SetIntraOpNumThreads(4);
	sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

	std::string detPath  = modelDir + "/en_PP-OCRv3_det_infer.onnx";
	std::string recPath  = modelDir + "/en_PP-OCRv3_rec_infer.onnx";
	std::string keysPath = modelDir + "/ppocr_keys_v1.txt";

	detSession = std::make_unique<Ort::Session>(env, detPath.c_str(), sessionOptions);
	recSession = std::make_unique<Ort::Session>(env, recPath.c_str(), sessionOptions);
	loadCharset(keysPath);
}

void OCR::loadCharset(const std::string& keysPath) {
	std::ifstream f(keysPath);
	if (!f.is_open())
		throw std::runtime_error("[OCR] Nao foi possivel abrir: " + keysPath);

	charset.clear();
	charset.push_back("blank");

	std::string line;
	while (std::getline(f, line))
		if (!line.empty()) charset.push_back(line);

	charset.push_back(" ");
}

std::vector<float> OCR::buildTensor(const cv::Mat& img, int& outH, int& outW) {
	outH = img.rows;
	outW = img.cols;

	std::vector<cv::Mat> channels(3);
	cv::split(img, channels);

	std::vector<float> tensor(3 * outH * outW);
	for (int c = 0; c < 3; ++c)
		std::memcpy(tensor.data() + c * outH * outW,
			    channels[c].ptr<float>(),
			    outH * outW * sizeof(float));
	return tensor;
}

std::vector<cv::Rect> OCR::detect(const cv::Mat& detImg) {
	int W = (detImg.cols / 32) * 32;
	int H = (detImg.rows / 32) * 32;

	cv::Mat input;
	if (W != detImg.cols || H != detImg.rows)
		cv::resize(detImg, input, cv::Size(W, H));
	else
		input = detImg;

	// DEBUG
	cv::Mat debugInput;
	input.convertTo(debugInput, CV_8UC3, 127.5, 127.5);
	cv::imwrite("/tmp/debug_det_input.png", debugInput);
	std::cout << "[DET] input size: " << input.cols << "x" << input.rows << "\n";

	int inH, inW;
	std::vector<float> tensor = buildTensor(input, inH, inW);
	std::vector<int64_t> shape = {1, 3, inH, inW};

	Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value inputOrt = Ort::Value::CreateTensor<float>(
		memInfo, tensor.data(), tensor.size(), shape.data(), shape.size());

	const char* inputNames[]  = {"x"};
	const char* outputNames[] = {"sigmoid_0.tmp_0"};

	auto outputs = detSession->Run(
		Ort::RunOptions{nullptr}, inputNames, &inputOrt, 1, outputNames, 1);

	auto& outTensor = outputs[0];
	auto  outShape  = outTensor.GetTensorTypeAndShapeInfo().GetShape();
	int   outH      = static_cast<int>(outShape[2]);
	int   outW      = static_cast<int>(outShape[3]);
	const float* data = outTensor.GetTensorData<float>();

	cv::Mat probMap(outH, outW, CV_32F, const_cast<float*>(data));
	cv::Mat binary;
	cv::threshold(probMap, binary, 0.3f, 255.0f, cv::THRESH_BINARY);
	binary.convertTo(binary, CV_8U);

	// DEBUG
	cv::Mat debugProb;
	probMap.convertTo(debugProb, CV_8U, 255.0);
	cv::imwrite("/tmp/debug_det_probmap.png", debugProb);
	cv::imwrite("/tmp/debug_det_binary.png", binary);
	std::cout << "[DET] probmap max: " << *std::max_element(data, data + outH * outW) << "\n";

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(binary, binary, kernel);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::cout << "[DET] contornos: " << contours.size() << "\n";

	float scaleX = static_cast<float>(detImg.cols) / outW;
	float scaleY = static_cast<float>(detImg.rows) / outH;

	std::vector<cv::Rect> boxes;
	for (const auto& contour : contours) {
		cv::Rect r = cv::boundingRect(contour);
		if (r.area() < 50) continue;

		int pad = 8;
		r.x      = std::max(0, static_cast<int>(r.x * scaleX) - pad);
		r.y      = std::max(0, static_cast<int>(r.y * scaleY) - pad);
		r.width  = std::min(detImg.cols - r.x, static_cast<int>(r.width  * scaleX) + 2 * pad);
		r.height = std::min(detImg.rows - r.y, static_cast<int>(r.height * scaleY) + 2 * pad);

		boxes.push_back(r);
	}

	// DEBUG: desenha boxes sobre imagem original desnormalizada
	cv::Mat debugBoxes;
	debugInput.copyTo(debugBoxes);
	for (const auto& b : boxes)
		cv::rectangle(debugBoxes, b, cv::Scalar(0, 255, 0), 2);
	cv::imwrite("/tmp/debug_det_boxes.png", debugBoxes);
	std::cout << "[DET] boxes validas: " << boxes.size() << "\n";

	std::sort(boxes.begin(), boxes.end(),
		[](const cv::Rect& a, const cv::Rect& b){ return a.y < b.y; });

	return boxes;
}

std::string OCR::recognize(const cv::Mat& lineImg) {
	const int targetH = 48;
	const int maxW    = 320;

	float ratio = static_cast<float>(lineImg.cols) / static_cast<float>(lineImg.rows);
	int targetW = static_cast<int>(targetH * ratio);
	targetW = std::max(32, std::min(targetW, maxW));
	targetW = ((targetW + 31) / 32) * 32;

	cv::Mat resized;
	cv::resize(lineImg, resized, cv::Size(targetW, targetH));

	// DEBUG
	static int lineCount = 0;
	cv::Mat debugLine;
	resized.convertTo(debugLine, CV_8UC3, 127.5, 127.5);
	cv::imwrite("/tmp/debug_line_" + std::to_string(lineCount++) + ".png", debugLine);

	int H, W;
	std::vector<float> tensor = buildTensor(resized, H, W);
	std::vector<int64_t> shape = {1, 3, H, W};

	Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
	Ort::Value inputOrt = Ort::Value::CreateTensor<float>(
		memInfo, tensor.data(), tensor.size(), shape.data(), shape.size());

	const char* inputNames[]  = {"x"};
	const char* outputNames[] = {"softmax_2.tmp_0"};

	auto outputs = recSession->Run(
		Ort::RunOptions{nullptr}, inputNames, &inputOrt, 1, outputNames, 1);

	auto& out      = outputs[0];
	auto  outShape = out.GetTensorTypeAndShapeInfo().GetShape();
	int timeSteps  = static_cast<int>(outShape[1]);
	int numClasses = static_cast<int>(outShape[2]);

	std::string text = ctcDecode(out.GetTensorData<float>(), timeSteps, numClasses);
	std::cout << "[REC] linha " << (lineCount-1) << ": '" << text << "'\n";
	return text;
}

std::string OCR::ctcDecode(const float* logits, int timeSteps, int numClasses) {
	std::string result;
	int lastIdx = -1;

	for (int t = 0; t < timeSteps; ++t) {
		const float* row = logits + t * numClasses;

		float maxVal = *std::max_element(row, row + numClasses);
		std::vector<float> probs(numClasses);
		float sum = 0.0f;
		for (int i = 0; i < numClasses; ++i) {
			probs[i] = std::exp(row[i] - maxVal);
			sum += probs[i];
		}

		int maxIdx = static_cast<int>(
			std::max_element(probs.begin(), probs.end()) - probs.begin());
		float confidence = (probs[maxIdx] / sum) * 100.0f;

		if (maxIdx != lastIdx && maxIdx != 0) {
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

std::string OCR::extractText(const cv::Mat& detImg, const cv::Mat& origImg) {
	if (detImg.empty() || origImg.empty()) {
		std::cerr << "[OCR] Erro: imagem vazia.\n";
		return "";
	}

	std::cout << "[OCR] detImg: " << detImg.cols << "x" << detImg.rows
	          << " origImg: " << origImg.cols << "x" << origImg.rows << "\n";

	std::vector<cv::Rect> boxes = detect(detImg);

	if (boxes.empty()) {
		std::cerr << "[OCR] Nenhuma linha detectada.\n";
		return "";
	}

	// Preprocessor para normalizar recortes da imagem original para o rec
	Preprocessor prep;

	std::string finalText;
	for (const auto& box : boxes) {
		cv::Mat crop   = origImg(box);
		cv::Mat recImg = prep.prepareForRec(crop);
		std::string lineText = recognize(recImg);
		if (!lineText.empty())
			finalText += lineText + "\n";
	}

	return finalText;
}
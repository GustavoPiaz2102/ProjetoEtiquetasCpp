#include "../heaters/OCR.h"
#include "../heaters/preprocessor.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>
#include <future>

static const std::string WHITELIST = "0123456789/:LFV";

static bool inWhitelist(const std::string& ch) {
	return WHITELIST.find(ch) != std::string::npos;
}


OCR::~OCR(){
	if(tess){
		tess->End();
		delete tess;
	}

	// Descarta boxes muito largas (detecção espúria juntando linhas)
	boxes.erase(std::remove_if(boxes.begin(), boxes.end(),
		[](const cv::Rect& r){ return r.height > r.width * 0.5f; }), boxes.end());

	// Limita a 3 boxes (lote, fabricação, validade)
	if (boxes.size() > 3) boxes.resize(3);

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

	return ctcDecode(out.GetTensorData<float>(), timeSteps, numClasses);
}

std::string OCR::ctcDecode(const float* logits, int timeSteps, int numClasses) {
	std::string result;
	int lastIdx = -1;

	for (int t = 0; t < timeSteps; ++t) {
		const float* row = logits + t * numClasses;

		int maxIdx = static_cast<int>(
			std::max_element(row, row + numClasses) - row);
		float confidence = row[maxIdx] * 100.0f;

		if (maxIdx != lastIdx && maxIdx != 0) {
			if (maxIdx < static_cast<int>(charset.size())) {
				const std::string& ch = charset[maxIdx];
				lastDebugError = "[CTC] ch='" + ch + "' conf=" + std::to_string(confidence) +
					" whitelist=" + std::to_string(inWhitelist(ch)) + "\n";
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

	std::vector<cv::Rect> boxes = detect(detImg);

	if (boxes.empty()) {
		std::cerr << "[OCR] Nenhuma linha detectada.\n";
		return "";
	}

	// Prepara todos os recortes antes de lançar as threads
	Preprocessor prep;
	std::vector<cv::Mat> recImgs;
	recImgs.reserve(boxes.size());
	for (const auto& box : boxes)
		recImgs.push_back(prep.prepareForRec(origImg(box)));

	// Lança reconhecimento de cada linha em paralelo
	std::vector<std::future<std::string>> futures;
	futures.reserve(recImgs.size());
	for (const auto& recImg : recImgs)
		futures.push_back(std::async(std::launch::async, &OCR::recognize, this, recImg));

	// Coleta resultados na ordem original (top → bottom)
	std::string finalText;
	for (auto& f : futures) {
		std::string line = f.get();
		if (!line.empty())
			finalText += line + "\n";
	}
	return finalText;
}

std::string OCR::getLastDebugError() const {
	return lastDebugError;
}
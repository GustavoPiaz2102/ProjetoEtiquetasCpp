#include "../heaters/arquiver.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

Arquiver::Arquiver(const std::string &caminho) : path(caminho) {}

bool Arquiver::load() {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Erro ao abrir o arquivo: " << path << "\n";
		return false;
	}

	dict.clear();
	std::string line;
	while (std::getline(file, line)) {
		std::string key, value;
		std::stringstream ss(line);

		if (std::getline(ss, key, ':')) {
			key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end()); // Remove espaços da chave
			std::getline(ss, value);						 // Lê o restante como valor, limpando ':' e espaços
			value.erase(0, value.find_first_not_of(" :"));

			dict[key] = value;
		}
	}

	file.close();
	return true;
}

bool Arquiver::save() const {
	// Primeiro carregar o conteúdo existente
	std::ifstream inFile(path);
	std::vector<std::pair<std::string, std::string>> existingLines;
	std::string line;

	if (inFile.is_open()) {
		while (std::getline(inFile, line)) {
			std::string key, value;
			std::stringstream ss(line);

			if (std::getline(ss, key, ':')) {
				key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
				std::getline(ss, value);
				value.erase(0, value.find_first_not_of(" :"));
				existingLines.emplace_back(key, value);
			}
		}

		inFile.close();
	}

	std::map<std::string, std::string> dictCopy = dict;

	// Atualizar as linhas existentes com os valores do dicionário
	for (auto &existingPair : existingLines) {
		auto it = dictCopy.find(existingPair.first);
		if (it != dictCopy.end()) {
			existingPair.second = it->second;
			dictCopy.erase(it); // Remove da cópia para não adicionar novamente
		}
	}

	// Adicionar os pares que não existiam no arquivo
	for (const auto &newPair : dictCopy)
		existingLines.emplace_back(newPair.first, newPair.second);

	// Reescrever o arquivo completo
	std::ofstream outFile(path);
	if (!outFile.is_open()) {
		std::cerr << "Erro ao abrir o arquivo para escrita: " << path << "\n";
		return false;
	}

	for (const auto &pair : existingLines)
		outFile << pair.first << ":" << pair.second << "\n";

	outFile.close();
	return true;
}

void Arquiver::show() const {
	for (const auto &pair : dict)
		std::cout << pair.first << " => " << pair.second << "\n";
}

void Arquiver::fallback() {
	dict.clear();
	dict["tamanho_etiqueta"] = "22 mm,20 mm";
	dict["espacamento"] = "2 mm,0 mm";
	dict["densidade"] = "15";
	dict["velocidade"] = "2";
	dict["direcao"] = "1";
	dict["tamanho_fonte"] = "1";
	dict["posicao_x"] = "35";
	dict["posicao_y_lote"] = "20";
	dict["posicao_y_fabricacao"] = "40";
	dict["posicao_y_validade"] = "60";
	dict["rotacao"] = "0";
	dict["escala_x"] = "1";
	dict["escala_y"] = "1";
	dict["fonte"] = "1";

	if (!save())
		std::cerr << "[Arquiver] Erro ao salvar arquivo de fallback: " << path << "\n";
}
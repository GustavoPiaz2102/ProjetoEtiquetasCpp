#include "../heaters/arquiver.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <utility>

Arquiver::Arquiver(const std::string& caminho) : path(caminho) {}

bool Arquiver::load() {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << path << std::endl;
        return false;
    }

    dict.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::string key, value;
        std::stringstream ss(line);

        if (std::getline(ss, key, ':')) {
            // Remove espaços da chave
            key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

            // Lê o restante como valor, limpando ':' e espaços
            std::getline(ss, value);
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

    // Criar uma cópia do dicionário para manipulação
    std::map<std::string, std::string> dictCopy = dict;

    // Atualizar as linhas existentes com os valores do dicionário
    for (auto& existingPair : existingLines) {
        auto it = dictCopy.find(existingPair.first);
        if (it != dictCopy.end()) {
            existingPair.second = it->second;
            dictCopy.erase(it); // Remove da cópia para não adicionar novamente
        }
    }

    // Adicionar os pares que não existiam no arquivo
    for (const auto& newPair : dictCopy) {
        existingLines.emplace_back(newPair.first, newPair.second);
    }

    // Reescrever o arquivo completo
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << path << std::endl;
        return false;
    }

    for (const auto& pair : existingLines) {
        outFile << pair.first << ":" << pair.second << "\n";
    }

    outFile.close();
    return true;
}

void Arquiver::show() const {
    for (const auto& pair : dict) {
        std::cout << pair.first << " => " << pair.second << std::endl;
    }
}
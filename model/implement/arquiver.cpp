#include "../heaters/arquiver.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

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
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << path << std::endl;
        return false;
    }

    for (const auto& pair : dict) {
        file << pair.first << ":" << pair.second << "\n";
    }

    file.close();
    return true;
}

void Arquiver::show() const {
    for (const auto& pair : dict) {
        std::cout << pair.first << " => " << pair.second << std::endl;
    }
}

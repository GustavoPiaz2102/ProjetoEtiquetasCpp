#include "arquiver.h"
void load_file(std::string & lt, std::string& fab, std::string& val){
    std::ifstream file(FILE_PATH);
    if (file.is_open()) {
        std::getline(file, lt);
        std::getline(file, fab);
        std::getline(file, val);
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para leitura: " << FILE_PATH << std::endl;
    }
}
void save_file(const std::string & lt, const std::string& fab, const std::string& val){
    std::ofstream file(FILE_PATH);
    if (file.is_open()) {
        file << lt << std::endl;
        file << fab << std::endl;
        file << val << std::endl;
        file.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << FILE_PATH << std::endl;
    }
}
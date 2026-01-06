#ifndef IMPRESS_H
#define IMPRESS_H
#include "arquiver.h"
#include <string>
#include <iostream>
#include <cstring> // para strcpy
#include <cstdlib> // para std::stoi, std::stof
#include <fstream> // ofstream
#include <vector>
#include <chrono>
class Impress
{
private:
    char tamanho_etiqueta[64] = "60 mm,40 mm";
    char espacamento[32] = "2 mm,0";
    int densidade = 8;
    int velocidade = 4;
    int direcao = 1;
    char texto[64] = "Teste";
    char tamanho_fonte[8] = "3";
    int posicao_x = 100;
    int posicao_y_lote = 100;
    int posicao_y_fabricacao = 150;
    int posicao_y_validade = 200;
    int rotacao = 0;
    float escala_x = 1.0f;
    float escala_y = 1.0f;
    char fonte[8] = "3";
    int qnt = 1;
    // float TempoEntreImpressao = 2.0f; // Tempo mínimo entre impressões em segundos
    Arquiver arq;
    std::vector<std::string> StrList;
    bool LastImpress = true;
    int QntImpressao = 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> TimeLastPrint;

public:
    Impress(Arquiver &arquiver);
    void LoadAtributes();
    void SaveAtributes();
    bool print(int *errorCode = nullptr); // Retorna true se a impressão foi iniciada com sucesso

    void setStrList(const std::vector<std::string> &strList)
    {
        StrList = strList;
    }
    void setLastImpress(bool lastImpress)
    {
        LastImpress = lastImpress;
    }
    bool getLastImpress() const
    {
        return LastImpress;
    }
    void setQntImpressao(int qnt)
    {
        QntImpressao = qnt;
    }
    int getQntImpressao() const
    {
        return QntImpressao;
    }
};
#endif // IMPRESS_H

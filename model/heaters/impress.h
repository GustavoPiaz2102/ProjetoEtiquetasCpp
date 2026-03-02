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

class Impress{
	private:
		Arquiver arq;
		std::vector<std::string> StrList;
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
		int QuantidadeDeImpressõesPorOrdem = 1;

		int QntImpressao = 0;

		// Flags //

		bool LastImpress = true; // Ultima impressão teve sucesso

	public:
		Impress(Arquiver &arquiver);
		void LoadAtributes();
		void SaveAtributes();
		bool print(int *errorCode = nullptr); // Retorna true se a impressão foi iniciada com sucesso

		void setStrList(const std::vector<std::string> &strList){
			StrList = strList;
		}

		void setLastImpress(bool lastImpress){
			LastImpress = lastImpress;
		}

		bool getLastImpress() const{
			return LastImpress;
		}

		void setQntImpressao(int qnt){
			QntImpressao = qnt;
		}

		int getQntImpressao() const{
			return QntImpressao;
		}

		void ResetLastImpress(){
			LastImpress = true;
		}
};

#endif // IMPRESS_H

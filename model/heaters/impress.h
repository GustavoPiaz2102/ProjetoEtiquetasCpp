#ifndef IMPRESS_H
#define IMPRESS_H

#include "arquiver.h"
#include <atomic>
#include <chrono>
#include <cstdlib> // para std::stoi, std::stof
#include <cstring> // para strcpy
#include <fstream> // ofstream
#include <iostream>
#include <string>
#include <vector>

class Impress {
	private:
		Arquiver arq;
		std::vector<std::string> StrList;
		char tamanho_etiqueta[64] = "22 mm,20 mm";
		char espacamento[32] = "2 mm,0 mm";
		int densidade = 15;
		int velocidade = 2;
		int direcao = 1;
		char texto[64] = "Teste";
		char tamanho_fonte[8] = "1";
		int posicao_x = 35;
		int posicao_y_lote = 20;
		int posicao_y_fabricacao = 40;
		int posicao_y_validade = 60;
		int rotacao = 0;
		int escala_x = 1;
		int escala_y = 1;
		char fonte[8] = "1";
		int QuantidadeDeImpressõesPorOrdem = 1;

		int QntImpressao = 0;

		// Flags //

		std::atomic<bool> LastImpress = true; // Ultima impressão teve sucesso

	public:
		Impress(Arquiver &arquiver);
		void LoadAtributes();
		void SaveAtributes();
		bool print(bool firstDet = true); // Retorna true se a impressão foi iniciada com sucesso

		void setStrList(const std::vector<std::string> &strList) {
			StrList = strList;
		}

		void setLastImpress(bool lastImpress) {
			LastImpress = lastImpress;
		}

		bool getLastImpress() const {
			return LastImpress;
		}

		void setQntImpressao(int qnt) {
			QntImpressao = qnt;
		}

		int getQntImpressao() const {
			return QntImpressao;
		}

		void ResetLastImpress() {
			LastImpress = true;
		}
};

#endif // IMPRESS_H

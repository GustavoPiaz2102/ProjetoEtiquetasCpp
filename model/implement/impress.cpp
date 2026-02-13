#include <iomanip> // para std::setprecision
#include <sstream> // para std::ostringstream
#include "../heaters/impress.h"

Impress::Impress(Arquiver &arquiver) : arq(arquiver) {}

void Impress::LoadAtributes(){
	if (!arq.load()) return;

	if (arq.dict.count("tamanho_etiqueta")) strcpy(tamanho_etiqueta, arq.dict["tamanho_etiqueta"].c_str());
	if (arq.dict.count("espacamento")) strcpy(espacamento, arq.dict["espacamento"].c_str());
	if (arq.dict.count("densidade")) densidade = std::stoi(arq.dict["densidade"]);
	if (arq.dict.count("velocidade")) velocidade = std::stoi(arq.dict["velocidade"]);
	if (arq.dict.count("direcao")) direcao = std::stoi(arq.dict["direcao"]);
	if (arq.dict.count("texto")) strcpy(texto, arq.dict["texto"].c_str());
	if (arq.dict.count("tamanho_fonte")) strcpy(tamanho_fonte, arq.dict["tamanho_fonte"].c_str());
	if (arq.dict.count("posicao_x")) posicao_x = std::stoi(arq.dict["posicao_x"]);
	if (arq.dict.count("posicao_y_lote")) posicao_y_lote = std::stoi(arq.dict["posicao_y_lote"]);
	if (arq.dict.count("posicao_y_fabricacao")) posicao_y_fabricacao = std::stoi(arq.dict["posicao_y_fabricacao"]);
	if (arq.dict.count("posicao_y_validade")) posicao_y_validade = std::stoi(arq.dict["posicao_y_validade"]);
	if (arq.dict.count("rotacao")) rotacao = std::stoi(arq.dict["rotacao"]);
	if (arq.dict.count("escala_x")) escala_x = std::stof(arq.dict["escala_x"]);
	if (arq.dict.count("escala_y")) escala_y = std::stof(arq.dict["escala_y"]);
	if (arq.dict.count("fonte")) strcpy(fonte, arq.dict["fonte"].c_str());
}

void Impress::SaveAtributes(){
	arq.dict["tamanho_etiqueta"] = tamanho_etiqueta;
	arq.dict["espacamento"] = espacamento;
	arq.dict["densidade"] = std::to_string(densidade);
	arq.dict["velocidade"] = std::to_string(velocidade);
	arq.dict["direcao"] = std::to_string(direcao);
	arq.dict["texto"] = texto;
	arq.dict["tamanho_fonte"] = tamanho_fonte;
	arq.dict["posicao_x"] = std::to_string(posicao_x);
	arq.dict["posicao_y_lote"] = std::to_string(posicao_y_lote);
	arq.dict["posicao_y_fabricacao"] = std::to_string(posicao_y_fabricacao);
	arq.dict["posicao_y_validade"] = std::to_string(posicao_y_validade);
	arq.dict["rotacao"] = std::to_string(rotacao);
	arq.dict["escala_x"] = std::to_string(escala_x);
	arq.dict["escala_y"] = std::to_string(escala_y);
	arq.dict["fonte"] = fonte;
	arq.save();
}

// Função auxiliar para formatar float sem casas decimais desnecessárias
std::string formatFloat(float val){
	std::ostringstream ss;

	if (val == (int)val) ss << std::fixed << std::setprecision(0) << val;
	else ss << std::fixed << std::setprecision(2) << val;
	
	return ss.str();
}

bool Impress::print(int *errorCode){

	if (LastImpress == false){
		std::cout << "A ultima impressão Teve Falhas" << "\n";
		if (errorCode != nullptr) *errorCode = 1;
		LastImpress = true;
	}

	LoadAtributes();

	if (strlen(fonte) == 0) strcpy(fonte, "3");

	std::string comando;
	comando += "SIZE " + std::string(tamanho_etiqueta) + "\r\n";
	comando += "GAP " + std::string(espacamento) + "\r\n";
	comando += "DENSITY " + std::to_string(densidade) + "\r\n";
	comando += "SPEED " + std::to_string(velocidade) + "\r\n";
	comando += "DIRECTION " + std::to_string(direcao) + "\r\n";
	comando += "CLS\r\n";

	if (StrList.size() > 0)
	comando += "TEXT " + std::to_string(posicao_x) + "," + std::to_string(posicao_y_lote) +
		   ",\"" + fonte + "\"," + std::to_string(rotacao) + "," +
		   formatFloat(escala_x) + "," + formatFloat(escala_y) + ",\"L:" + StrList[0] + "\"\r\n";

	if (StrList.size() > 1)
	comando += "TEXT " + std::to_string(posicao_x) + "," + std::to_string(posicao_y_fabricacao) +
		   ",\"" + fonte + "\"," + std::to_string(rotacao) + "," +
		   formatFloat(escala_x) + "," + formatFloat(escala_y) + ",\"F:" + StrList[1] + "\"\r\n";

	if (StrList.size() > 2)
	comando += "TEXT " + std::to_string(posicao_x) + "," + std::to_string(posicao_y_validade) +
		   ",\"" + fonte + "\"," + std::to_string(rotacao) + "," +
		   formatFloat(escala_x) + "," + formatFloat(escala_y) + ",\"V:" + StrList[2] + "\"\r\n";

	comando += "PRINT " + std::to_string(QuantidadeDeImpressõesPorOrdem) + "\r\n";

	try{
		std::ofstream impressora("/dev/usb/lp0", std::ios::binary);
		if (!impressora.is_open()){
			std::cerr << "❌ Erro: não foi possível abrir a impressora." << "\n";
			return false;
	}

	std::cout << "Enviando comando para impressora:\n" << comando << "\n";

	impressora.write(comando.c_str(), comando.size());
	impressora.close();

	std::cout << "✅ Etiqueta enviada com sucesso!" << "\n";
	QntImpressao--;
	return true;
	
	} catch(const std::exception &e){
	std::cerr << "❌ Erro ao imprimir: " << e.what() << "\n";
	return false;
	}
}

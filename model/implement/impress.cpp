#include "../heaters/impress.h"

Impress::Impress(Arquiver& arquiver) : arq(arquiver) {}

void Impress::LoadAtributes() {
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

void Impress::SaveAtributes() {
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

bool Impress::print(std::vector<std::string> StrList) {
    std::string comando;
    LoadAtributes();
    // Monta o comando com os valores do objeto
    comando += "SIZE " + std::string(tamanho_etiqueta) + "\n";
    comando += "GAP " + std::string(espacamento) + "\n";
    comando += "DENSITY " + std::to_string(densidade) + "\n";
    comando += "SPEED " + std::to_string(velocidade) + "\n";
    comando += "DIRECTION " + std::to_string(direcao) + "\n";
    comando += "CLS\n";

    // Usa os textos do parâmetro StrList
    // Espera: [lote, fabricacao, validade]
    if (StrList.size() > 0)
        comando += "TEXT " + std::to_string(posicao_x) + "," + std::to_string(posicao_y_lote) +
                   ",\"" + fonte + "\"," + std::to_string(rotacao) + "," +
                   std::to_string(escala_x) + "," + std::to_string(escala_y) + ",\"L.:" + StrList[0] + "\"\n";

    if (StrList.size() > 1)
        comando += "TEXT " + std::to_string(posicao_x) + "," + std::to_string(posicao_y_fabricacao) +
                   ",\"" + fonte + "\"," + std::to_string(rotacao) + "," +
                   std::to_string(escala_x) + "," + std::to_string(escala_y) + ",\"FAB.:" + StrList[1] + "\"\n";

    if (StrList.size() > 2)
        comando += "TEXT " + std::to_string(posicao_x) + "," + std::to_string(posicao_y_validade) +
                   ",\"" + fonte + "\"," + std::to_string(rotacao) + "," +
                   std::to_string(escala_x) + "," + std::to_string(escala_y) + ",\"VAL.:" + StrList[2] + "\"\n";

    comando += "PRINT " + std::to_string(qnt) + "\n";

    // Envia para impressora
    try {
        std::ofstream impressora("/dev/usb/lp0", std::ios::binary);
        if (!impressora.is_open()) {
            std::cerr << "❌ Erro: não foi possível abrir a impressora." << std::endl;
            return false;
        }
        impressora.write(comando.c_str(), comando.size());
        impressora.close();
        std::cout << "✅ Etiqueta enviada com sucesso!" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Erro ao imprimir: " << e.what() << std::endl;
        return false;
    }
}

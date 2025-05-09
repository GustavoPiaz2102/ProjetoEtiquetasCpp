#ifndef INTERFACE_H
#define INTERFACE_H

#include <map>
#include <string>
#include <vector>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combobox.hpp>
#include <nana/gui/widgets/date_chooser.hpp>
#include <nana/gui/place.hpp>
#include <sstream>

class Interface {
public:
    /**
     * @brief Menu deve dar ao usuário um menu de escolha e retornar um inteiro para que a controller defina a ação seguinte.
     * @return 1 para iniciar sistema de detecção
     * @return 2 para mudar data
     * @return 3 para sair do programa
     */
    int menu();

    /**
     * @brief mudar_data deve abrir um calendário para mudança de data e um menu de seleção de lote que vai de 1 a 100.
     * @param data Mapa com dados atuais (pode ser ignorado ou usado para preencher valores iniciais).
     * @return Mapa com as chaves "fab", "val" e "lt" nos formatos:
     *         fab = DD/MES/AAAA (ex: 02/MAI/2025)
     *         val = DD/MES/AAAA
     *         lt  = NNN/AA (ex: 006/24)
     */
    std::map<std::string, std::string> mudar_data(std::map<std::string, std::string> data);
};

#endif // INTERFACE_H

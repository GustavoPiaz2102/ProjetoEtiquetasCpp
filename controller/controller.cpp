#include "controller.h"
#include <functional>
#include <iostream>

Controller::Controller()
    : validator("000/00", "00/000/0000", "00/000/0000"),
      interface(validator),
      detector(interface),
      selected_option(-1),
      arquiver("data/config.txt")
{
    // Carrega os dados do arquivo para o validador
    glewInit();
    arquiver.load();
    std::string lt_value = arquiver.dict["lt"];
    std::string fab_value = arquiver.dict["fab"];
    std::string val_value = arquiver.dict["val"];

    // --- SOLUÇÃO PARA O ERRO DE BINDING ---
    // Criamos uma variável local para cada valor e a passamos para a função Set.
    if (!lt_value.empty()) {
        std::string full_lt = lt_value; // Cria a variável local
        validator.SetLT(full_lt);              // Passa a variável (lvalue)
    }
    if (!fab_value.empty()) {
        std::string full_fab = fab_value;
        validator.SetFAB(full_fab);
    }
    if (!val_value.empty()) {
        std::string full_val = val_value;
        validator.SetVAL(full_val);
    }
    std::cout << "Dados carregados: ";
    validator.printall();
}

Controller::~Controller() {
    // Salva os dados do validador no arquivo ao destruir o controlador
    arquiver.dict["lt"]=validator.GetLT();
    arquiver.dict["fab"]=validator.GetFAB();
    arquiver.dict["val"]=validator.GetVAL();
    arquiver.save();
    std::cout << "Dados salvos: ";
    validator.printall();
}

void Controller::run() {
    interface.iniciar_janela();
    while (true) {
        interface.process_events();
        interface.begin_frame();

        switch (selected_option) {
            case -1:
                interface.menu(selected_option);
                break;

            case 0:
                rodar_detector();
                break;

            case 1:
                if (requisitar_data_e_setar(0, [&](const std::string& d) { validator.SetFAB(d); }))
                    selected_option = 3;
                break;
                
            case 3:
                if (requisitar_data_e_setar(1, [&](const std::string& d) { validator.SetVAL(d); }))
                    selected_option = 4;
                break;


            case 4: {
                std::string lt;
                if (interface.requisitar_lt(lt)) {
                    validator.SetLT(lt);
                    std::cout << "Lote selecionado: " << lt << std::endl;
                    selected_option = -1;
                    arquiver.dict["lt"]=validator.GetLT();
                    arquiver.dict["fab"]=validator.GetFAB();
                    arquiver.dict["val"]=validator.GetVAL();
                    arquiver.save();

                }
                break;
            }
            case 5:{
                if(interface.config_menu()) selected_option = -1;
                break;
            }
            case 2:{
                selected_option = -1;
                break;
            }
        }

        interface.end_frame();
    }
}

bool Controller::requisitar_data_e_setar(int tipo, std::function<void(const std::string&)> setter) {
    std::string data;
    if (interface.requisitar_data(data, tipo)) {
        std::cout << "Data selecionada: " << data << std::endl;
        setter(data);
        return true;
    }
    return false;
}

void Controller::rodar_detector() {
    std::string str = detector.run();
    std::cout << "Resultado do detector: " << str << std::endl;
    if (str == "return") {
        selected_option = -1;
        return;
    }

    if (!validator.Validate(str)) {
        // TODO: Implementar aviso de erro para validação
        std::cout << "Erro: código inválido." << std::endl;
    }
}

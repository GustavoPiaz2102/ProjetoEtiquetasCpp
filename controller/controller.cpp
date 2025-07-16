#include "controller.h"
#include <functional>
#include <iostream>

Controller::Controller()
    : validator("L. 000/00", "Fab 00/000/0000", "Val 00/000/0000"),
      interface(validator),
      detector(interface),
      selected_option(-1)
{
    // Carrega os dados do arquivo para o validador
    std::string lt, fab, val;
    load_file(lt, fab, val);
    validator.SetLT(lt);
    validator.SetFAB(fab);
    validator.SetVAL(val);

    std::cout << "Dados carregados: ";
    validator.printall();
}

Controller::~Controller() {
    // Salva os dados do validador no arquivo ao destruir o controlador
    save_file(validator.GetLT(), validator.GetFAB(), validator.GetVAL());
    std::cout << "Dados salvos: ";
    validator.printall();
}

void Controller::run() {
    interface.iniciar_janela();

    while (!interface.shouldClose()) {
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
                }
                break;
            }

            case 2:
                std::cout << "Saindo do sistema..." << std::endl;
                return; // Encerra o loop e a função
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

    if (str == "return") {
        selected_option = -1;
        return;
    }

    if (!validator.Validate(str)) {
        // TODO: Implementar aviso de erro para validação
        std::cout << "Erro: código inválido." << std::endl;
    }
}

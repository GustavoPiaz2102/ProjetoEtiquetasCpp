#include "controller.h"

Controller::Controller() 
    : validator("L. 000/00","Fab 00/000/0000","Val 00/000/0000"),interface(validator), detector(interface),selected_option(-1) // inicializa interface e passa para o detector
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

        if (selected_option == -1) {
            interface.menu(selected_option);
        } 
        else if(selected_option == 1) { //Selecionar data
            std::string selected_date;
            if(interface.requisitar_data(selected_date,0)) {
                selected_option = 3; //Recebe o Valor de Fab
                std::cout<<"Data selecionada: " << selected_date << std::endl;
                validator.SetFAB(selected_date);
            }
        }
        else if (selected_option == 3) { // Recebe os valores de val
            std::string selected_date2;
            if(interface.requisitar_data(selected_date2,1)) {
                selected_option = 4; //Recebe o Valor de Fab
                std::cout<<"Data selecionada: " << selected_date2 << std::endl;
                validator.SetVAL(selected_date2);
            }
        }
        else if (selected_option == 4) { 
            std::string selected_lt;
            if(interface.requisitar_lt(selected_lt)) {
                selected_option = -1; 
                std::cout<<"Lote selecionado: " << selected_lt << std::endl;
                validator.SetLT(selected_lt);
            }
        }
        else if (selected_option == 2) { // Sair
            std::cout << "Saindo do sistema..." << std::endl;
            break;
        }
        else if (selected_option == 0) { // Rodar Sistema
            std::string str = detector.run();
            if (str == "return") {
                selected_option = -1;
                interface.end_frame();
                continue;
            } else {
                if (validator.Validate(str)) {
                    continue;
                } else {
                    // Implementar aviso de erro
                }
            }
        }

        interface.end_frame();

}

}
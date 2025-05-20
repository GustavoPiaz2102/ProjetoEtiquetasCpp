#include "controller.h"

Controller::Controller() 
    : interface(), detector(interface),selected_option(-1) // inicializa interface e passa para o detector
{}

Controller::~Controller() {}

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
            if(interface.requisitar_data(selected_date)) {
                selected_option = 3; //Recebe o Valor de Fab
                std::cout<<"Data selecionada: " << selected_date << std::endl;
            }
        }
        else if (selected_option == 3) { // Recebe os valores de val
            std::string selected_date2;
            if(interface.requisitar_data(selected_date2)) {
                selected_option = -1; //Recebe o Valor de Fab
                std::cout<<"Data selecionada: " << selected_date2 << std::endl;
            }
        }
        else if (selected_option == 2) { // Sair
            break;
        }
        else if (selected_option == 0) { // Rodar Sistema
                if(detector.run() == "return"){
                    selected_option = -1;
                    interface.end_frame();
                    continue;
                }
            
            }
        interface.end_frame();

}

}
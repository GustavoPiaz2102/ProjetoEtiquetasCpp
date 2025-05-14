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

        if (selected_option == -1 || selected_option == 1) {
            interface.menu(selected_option);
        } 
        else if (selected_option == 2) { // Sair
            break;
        }
        else if (selected_option == 0) { // Rodar Sistema
                if(detector.run() == "return"){
                    selected_option = -1;
                    continue;
                }
            
            }
        interface.end_frame();

}
}
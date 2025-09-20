#include "controller.h"
#include <functional>
#include <iostream>

Controller::Controller()
    : validator("000/00", "00/00/0000", "00/00/0000"),
      interface(validator),
      detector(interface),
      selected_option(-1),
      arquiver("data/config.txt"),
      imp(arquiver),
      qnt_impress(0),
      Lastimp(true)
      
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

    try {
        while (true) {
            interface.process_events();
            interface.begin_frame();

            switch (selected_option) {
                case -1:
                    if(qnt_impress<=0){
                        interface.setImprimindo(false);
                    }
                    interface.menu(selected_option,qnt_impress);
                    if(interface.GetImprimindo()&&qnt_impress>0){ 
                        if (qnt_impress > 0) {
                            strList = {
                                validator.GetLT(),
                                validator.GetFAB(),
                                validator.GetVAL()
                            };
                        } else {
                            interface.setImprimindo(false);
                        }
                    }
                    else{
                        qnt_impress=0;
                    }
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
                        std::cout << "Lote selecionado: " << lt << "\n";
                        selected_option = -1;
                        arquiver.dict["lt"] = validator.GetLT();
                        arquiver.dict["fab"] = validator.GetFAB();
                        arquiver.dict["val"] = validator.GetVAL();
                        arquiver.save();
                    }
                    break;
                }

                case 5: {
                    if (interface.config_menu(arquiver)){
                        selected_option = -1;
                        imp.LoadAtributes();
                        imp.SaveAtributes();
                    }
                    break;
                }
                //Imprime
                case 2: {
                    if (interface.config_impress(qnt_impress))
                        selected_option = -1;
                    break;
                }
                //
                case -10:{
                    arquiver.save();
                    std::system("shutdown now");

                }
            }

            interface.end_frame();
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro capturado: " << e.what() << "\n";
        arquiver.save();
        // Aqui você pode decidir se quer abortar, reiniciar o loop, ou outra coisa
    } catch (...) {
        std::cerr << "Erro desconhecido capturado.\n";
        arquiver.save();
    }
}


bool Controller::requisitar_data_e_setar(int tipo, std::function<void(const std::string&)> setter) {
    std::string data;
    if (interface.requisitar_data(data, tipo)) {
        std::cout << "Data selecionada: " << data << "\n";
        setter(data);
        return true;
    }
    return false;
}

void Controller::rodar_detector() {
    if(interface.GetImprimindo()){
    if (imp.print(strList,Lastimp)) {
        std::cout << "Impressão iniciada com sucesso!" << "\n";
        qnt_impress--;
    } else {
        if(interface.PopUpError("Erro ao iniciar a impressão.")){
            Lastimp = true;
            if(qnt_impress<=0){
                interface.setImprimindo(false);
                selected_option = -1;
                Lastimp = true;
                return;
            }
            else{
                Lastimp = true;
                return;
            }
    }
    }
}
    //Aqui vai ter que dar um sleep para a etiqueta chegar no lugar da detecção
    //Aqui também faz o OUT do strobo
    std::string str = detector.run();
    std::cout << "Resultado do detector: " << str << std::endl;
    if (str == "return") {
        selected_option = -1;
        Lastimp = true;
        return;
    }
    if (!validator.Validate(str)) {
        Lastimp = false;
        std::cout << "Erro: código inválido." << "\n";
    }
}

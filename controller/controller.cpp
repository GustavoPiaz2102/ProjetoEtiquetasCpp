#include "controller.h"
#include <functional>
#include <iostream>

Controller::Controller()
    : validator("000/00", "00/00/0000", "00/00/0000"),
      interface(validator),
      selected_option(-1),
      arquiver("data/config.txt"),
      imp(arquiver),
      detector(imp, interface, validator)
{
    // Carrega os dados do arquivo para o validador
    glewInit();
    arquiver.load();
    std::string lt_value = arquiver.dict["lt"];
    std::string fab_value = arquiver.dict["fab"];
    std::string val_value = arquiver.dict["val"];

    // --- SOLUÇÃO PARA O ERRO DE BINDING ---
    // Criamos uma variável local para cada valor e a passamos para a função Set.
    if (!lt_value.empty())
    {
        std::string full_lt = lt_value; // Cria a variável local
        validator.SetLT(full_lt);       // Passa a variável (lvalue)
    }
    if (!fab_value.empty())
    {
        std::string full_fab = fab_value;
        validator.SetFAB(full_fab);
    }
    if (!val_value.empty())
    {
        std::string full_val = val_value;
        validator.SetVAL(full_val);
    }
    std::cout << "Dados carregados: ";
    validator.printall();
}

Controller::~Controller()
{
    // Salva os dados do validador no arquivo ao destruir o controlador
    arquiver.dict["lt"] = validator.GetLT();
    arquiver.dict["fab"] = validator.GetFAB();
    arquiver.dict["val"] = validator.GetVAL();
    arquiver.save();
    std::cout << "Dados salvos: ";
    validator.printall();
}

void Controller::run()
{
    interface.iniciar_janela();

    try
    {
        while (true)
        {
            interface.process_events();
            interface.begin_frame();

            switch (selected_option)
            {
            case -1:
                if (imp.getQntImpressao() <= 0)
                {
                    interface.setImprimindo(false);
                }
                interface.menu(selected_option, imp.getQntImpressao());
                if (interface.GetImprimindo() && imp.getQntImpressao() > 0)
                {
                    if (imp.getQntImpressao() > 0)
                    {
                        imp.setStrList({validator.GetLT(),
                                        validator.GetFAB(),
                                        validator.GetVAL()});
                    }
                    else
                    {
                        interface.setImprimindo(false);
                    }
                }
                else
                {
                    imp.setQntImpressao(0);
                }
                break;

            case 0:
                rodar_detector();
                break;

            case 1:
                if (requisitar_data_e_setar(0, [&](const std::string &d)
                                            { validator.SetFAB(d); }))
                    selected_option = 3;
                break;

            case 3:
                if (requisitar_data_e_setar(1, [&](const std::string &d)
                                            { validator.SetVAL(d); }))
                    selected_option = 4;
                break;

            case 4:
            {
                std::string lt;
                if (interface.requisitar_lt(lt))
                {
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

            case 5:
            {
                if (interface.config_menu(arquiver))
                {
                    selected_option = -1;
                    imp.LoadAtributes();
                    imp.SaveAtributes();
                }
                break;
            }
            // Configura impressão
            case 2:
            {
                bool InstantImpress = false;
                qnt_impress = imp.getQntImpressao();
                if (interface.config_impress(qnt_impress, &InstantImpress))
                {
                    selected_option = -1;
                }
                imp.setQntImpressao(qnt_impress);
                if (InstantImpress && qnt_impress > 0)
                {
                    interface.setImprimindo(true);
                    for (int i = 0; i < qnt_impress; i++)
                    {
                        imp.print();
                    }
                }
                interface.setImprimindo(false);
                break;
            }
            //
            case -10:
            {
                arquiver.save();
                // std::system("shutdown now");
            }
            }

            interface.end_frame();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro capturado: " << e.what() << "\n";
        arquiver.save();
        // Aqui você pode decidir se quer abortar, reiniciar o loop, ou outra coisa
    }
    catch (...)
    {
        std::cerr << "Erro desconhecido capturado.\n";
        arquiver.save();
    }
}

bool Controller::requisitar_data_e_setar(int tipo, std::function<void(const std::string &)> setter)
{
    std::string data;
    if (interface.requisitar_data(data, tipo))
    {
        std::cout << "Data selecionada: " << data << "\n";
        setter(data);
        return true;
    }
    return false;
}
/*
void Controller::rodar_detector() {
    if(interface.GetImprimindo()){
    if (imp.print()) {
        std::cout << "Impressão iniciada com sucesso!" << "\n";
        qnt_impress--;
    } else {
        if(interface.PopUpError("Erro ao iniciar a impressão.")){
            imp.setLastImpress(true);
            if(qnt_impress<=0){
                interface.setImprimindo(false);
                selected_option = -1;
                imp.setLastImpress(true);
                return;
            }
            else{
                imp.setLastImpress(true);
                return;
            }
    }
    }
}
    //Aqui vai ter que dar um sleep para a etiqueta chegar no lugar da detecção
    active = 0;
    while (!active){
        active = Sensor.ReadSensor();
    }

    //Aqui também faz o OUT do strobo
    Sensor.OutStrobo()
    std::string str = detector.run();
    std::cout << "Resultado do detector: " << str << std::endl;
    if (str == "return") {
        selected_option = -1;
        imp.setLastImpress(true);
        return;
    }
    if (!validator.Validate(str)) {
        imp.setLastImpress(false);
        std::cout << "Erro: código inválido." << "\n";
    }
}
*/
void Controller::rodar_detector()
{
    if (interface.GetImprimindo())
    {
        // aciona o thread para ler o sensor ,capturar imagem e imprimir
        if (!SensorActive)
        {
            SensorActive = true;
            detector.StartSensorThread();
        }

        if (!FirstDet)
        {
            interface.atualizar_frame(detector.GetFrame());
        }
        else
        {
            interface.atualizar_frame(cv::Mat::zeros(480, 640, CV_8UC3));
        }

        if (!ProcessActive)
        {
            ProcessActive = true;
            detector.StartProcessThread();
        }
    }
    else
    {
        if (SensorActive)
        {
            SensorActive = false;
            detector.StopSensorThread();
        }
        if (ProcessActive)
        {
            ProcessActive = false;
            detector.StopProcessThread();
        }
        FirstDet = true;
        selected_option = -1;
    }
}

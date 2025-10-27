#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"
#include "../model/heaters/validator.h"
#include "../view/interface.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "../model/heaters/arquiver.h" // Para carregar e salvar os dados do validador
#include "../model/heaters/impress.h"
#include <mutex>

class Controller {
public:
    Controller();  // Somente a declaração aqui
    ~Controller();     
    void run();

private:
    Interface interface;
    Detector detector;
    int selected_option;
    Validator validator;
    bool requisitar_data_e_setar(int tipo, std::function<void(const std::string&)> setter);
    void rodar_detector();
    Arquiver arquiver;
    Impress imp;
    int qnt_impress; // Quantidade de impressões
    bool SensorActive = false;
    bool ProcessActive = false;
    bool FirstDet = true;
};

#endif // CONTROLLER_H

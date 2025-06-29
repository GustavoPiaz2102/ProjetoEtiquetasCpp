#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"
#include "../model/validator.h"
#include "../view/interface.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "../model/arquiver.h" // Para carregar e salvar os dados do validador

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

};

#endif // CONTROLLER_H

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"
#include "../view/interface.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <iostream>

class Controller {
public:
    Controller();  // Somente a declaração aqui
    ~Controller();     
    void run();

private:
    Interface interface;
    Detector detector;
    int selected_option;
};

#endif // CONTROLLER_H

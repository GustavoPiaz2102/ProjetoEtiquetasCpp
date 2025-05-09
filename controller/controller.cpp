#include "controller.h"
#include <opencv2/opencv.hpp>
#include <iostream>

Controller::Controller() {}

Controller::~Controller() {}

void Controller::run() {
    switch (interface.menu())
    {
    case 1:
        detector.run()
        break;

    case 2:
        std::map<std::string, std::string data;
        data = interface.mudar_data(data);

    case 3:
        exit;
        
    default:
        break;
    }
}

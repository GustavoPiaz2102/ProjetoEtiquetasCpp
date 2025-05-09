#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"
#include "interface.h"
#include <memory>

class Controller {
public:
    Controller();
    ~Controller();
    void run();

private:
    Interface interface;
    Detector detector;
};

#endif // CONTROLLER_H
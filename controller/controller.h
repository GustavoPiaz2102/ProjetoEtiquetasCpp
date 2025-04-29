#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "detector.h"
#include <memory>

class Controller {
public:
    Controller();
    ~Controller();
    void run();

private:
    Detector detector;
};

#endif // CONTROLLER_H
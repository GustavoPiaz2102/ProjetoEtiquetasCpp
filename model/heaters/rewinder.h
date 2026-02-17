#ifndef REWINDER_H
#define REWINDER_H

#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include "../../view/interface.h"

class Rewinder {
    private:
        std::atomic<bool> running{true};
        std::atomic<int> currentSpeed; 
        std::thread rewinderThread; 
        Interface& interface;

    public:
        Rewinder(uint16_t initialSpeed, Interface& interface);
        ~Rewinder();

        void run();
        void setSpeed(uint16_t rpm);
        void step(int steps);
        int getSensorSpeed();
};

#endif // REWINDER_H

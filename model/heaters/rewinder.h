#ifndef REWINDER_H
#define REWINDER_H

#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include "../../view/interface.h"
#include "../heaters/hardwarePWM.h"
#include "../heaters/GPIO.h"

class Rewinder {
    private:
        std::atomic<bool> running{true};
        std::thread rewinderThread; 
        Interface& interface;
        HardwarePWM pwmMode{0, 0};
        GPIO gpio
        
        std::atomic<double> targetSpeed{0};

        double Kp, Ki, Kd; // Ganhos do PID
        double integral;
        double lastError;
        double currentFreqHz; // Frequência atual do motor em Hz

    public:
        Rewinder(double targetSpeed, Interface& interface, GPIO& gpio);
        ~Rewinder();

        void run();
        void setSpeed(double rpm);
        double getSensorSpeed(double dt_seconds);
};

#endif // REWINDER_H

#include "../heaters/rewinder.h"

Rewinder::Rewinder(uint16_t initialSpeed, Interface& interface) : currentSpeed(initialSpeed), interface(interface) {
    running = true;
    rewinderThread = std::thread(&Rewinder::run, this);
}

Rewinder::~Rewinder() {
    running = false;
    if (rewinderThread.joinable()) rewinderThread.join();
}

void Rewinder::run() {
    while(running){
        if(interface.GetImprimindo()){

        }
    }
}

void Rewinder::setSpeed(uint16_t rpm) {
    currentSpeed = rpm;
}

void Rewinder::step(int steps) {
    
}

int Rewinder::getSensorSpeed() {
    
    return 0; // Retorna um valor de exemplo
}
#include "../heaters/rewinder.h"

Rewinder::Rewinder(double targetSpeed, Interface& interface, GPIO& gpio) : targetSpeed(targetSpeed), interface(interface), gpio(gpio) {
    Kp = 5.0; 
    Ki = 0.5;
    Kd = 0.1;
    
    integral = 0.0;
    lastError = 0.0;
    currentFreqHz = 0.0;
    
    running = true;
    rewinderThread = std::thread(&Rewinder::run, this);
}

Rewinder::~Rewinder() {
    running = false;
    if (rewinderThread.joinable()) rewinderThread.join();
}

void Rewinder::run() {
    pwmMode.enable(true);
    auto lastTime = std::chrono::steady_clock::now();

    while(running){
        if(interface.GetImprimindo()){
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> dt = now - lastTime;
            lastTime = now;

            double currentSpeed = getSensorSpeed(dt.count());

            double error = targetSpeed - currentSpeed;

            integral += error * dt.count();
            double derivative = (error - lastError) / dt.count();
            lastError = error;

            double pidOutput = (Kp * error) + (Ki * integral) + (Kd * derivative);

            currentFreqHz += pidOutput;

            if (currentFreqHz < 100.0) currentFreqHz = 100.0;    // Min RPM para manter a fita tensionada
            if (currentFreqHz > 3000.0) currentFreqHz = 3000.0;  // Max RPM (Evita travamento do rotor)

            pwmMode.setFrequency(static_cast<uint32_t>(currentFreqHz));
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Tempo de amostragem
        } else{
            pwmMode.setFrequency(0);
            integral = 0.0; 
            currentFreqHz = 100.0;
            lastTime = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Rewinder::setSpeed(double rpm) {
    targetSpeed = rpm;
}

double Rewinder::getSensorSpeed(double dt_seconds) {
    int pulses = gpio.GetAndResetEncoderPulses();

    double distance_per_pulse = 0.5; 
    return (pulses * distance_per_pulse) / dt_seconds;
}
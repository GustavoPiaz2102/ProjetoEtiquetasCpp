#include <Stepper.h>

// Definimos o número de passos por revolução para o motor interno (32) 
// multiplicado pela redução (64), resultando em aprox. 2048 passos por volta.
const int passosPorRevolucao = 2048; 

// Inicializa a biblioteca nos pinos 8, 10, 9, 11 (Sequência correta para o 28BYJ-48)
Stepper meuMotor(passosPorRevolucao, 8, 10, 9, 11);

void setup() {
  // Ajusta a velocidade inicial (RPM)
  // Como discutimos, para este motor o ideal é entre 10 e 15 RPM.
  meuMotor.setSpeed(14);
  Serial.begin(9600);
}

void loop() {
  // Gira uma volta completa no sentido horário
  Serial.println("Girando no sentido horario...");
  meuMotor.step(passosPorRevolucao);
  delay(500);

  // Gira uma volta completa no sentido anti-horário
  Serial.println("Girando no sentido anti-horario...");
  meuMotor.step(-passosPorRevolucao);
  delay(500);
}
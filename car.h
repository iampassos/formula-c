#ifndef CAR_H
#define CAR_H

#include "raylib.h"

typedef struct {
    int id;
    float lapTime;
    Vector2 pos;
    float vel;
    float acc;
    int width;
    int height;
    Color color;
    float angle;
    float angularAcc;
    float minTurnSpeed;
    float breakForce;
    float dragForce;
} Car;

Car* Car_create(Vector2 pos,float vel, float acc, int width, int height, Color color, float angle, float angularAcc, float minTurnSpeed, float breakForce, float dragForce, int id);
void Car_free(Car* car); // Libera a memória de um carro

void Car_update(Car *car);       // Atualizar a posição do carro a cada frame
void Car_accelerate(Car *car);     // Acelerar o carro
void Car_draw(Car *car);         // Desenhar o carro na tela
//void Car_turn(Car *car, float angle); // Rotacionar o carro
void Car_turnLeft(Car *car);          // Virar o carro para esquerda
void Car_turnRight(Car *car);         // Virar o carro para direita

void Car_break(Car *car);   // Freio ativo
void Car_reverse(Car *car); // Ré do carro

Color Car_getFloor(Car* car, Image trackMask, Color *trackPixels); // FUncao para ler cor da pista

void Car_move(Car* car, int up, int down, int right, int left); // Atualiza o carro de acordo com os inputs do usuário

void Car_info(Car* car); // Mostra as informações do carro no console (PARA DEBUG)

#endif
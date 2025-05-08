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

void Car_setDrag(float track_drag, float light_escape_area_drag, float hard_escape_area_drag, float ouside_track_drag);

Car* Car_create(Vector2 pos,float vel, float acc, int width, int height, Color color, float angle, float angularAcc, float minTurnSpeed, float breakForce,int id);
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

void Car_setMask (Image mask, Color *maskPixel); // Definindo a mascara de pixel para carros lerem

#endif
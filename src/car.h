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

void Track_setDrag(
    float track_drag, float light_escape_area_drag, float hard_escape_area_drag,
    float ouside_track_drag); // Função que define as propriedades da pista
void Track_setMask(
    Image mask,
    Color *maskPixel); // Definindo a mascara de pixel para carros lerem
void Track_setColor(Color track, Color light_escape, Color hard_escape,
                    Color outside); // Definindo (rgb) para cada parte da pista

void Track_Unload();

Car *Car_create(Vector2 pos, float acc, int width, int height, Color color,
                float angle, float angularAcc, float minTurnSpeed,
                float breakForce, int id);
void Car_free(Car *car); // Libera a memória de um carro

void Car_update(Car *car);     // Atualizar a posição do carro a cada frame
void Car_accelerate(Car *car); // Acelerar o carro
void Car_draw(Car *car);       // Desenhar o carro na tela
// void Car_turn(Car *car, float angle); // Rotacionar o carro
void Car_turnLeft(Car *car);  // Virar o carro para esquerda
void Car_turnRight(Car *car); // Virar o carro para direita

void Car_break(Car *car);   // Freio ativo
void Car_reverse(Car *car); // Ré do carro

Color Car_getFloor(Car *car, Image trackMask,
                   Color *trackPixels); // FUncao para ler cor da pista

void Car_move(Car *car, int up, int down, int right,
              int left); // Atualiza o carro de acordo com os inputs do usuário

void Car_setPos(Car *car, Vector2 newPos); // Definir uma nova posição do carro
void Car_setAngle(Car *car,
                  float angle); // Definindo um novo ângulo para o carro

void Car_info(
    Car *car); // Mostra as informações do carro no console (PARA DEBUG)

#endif

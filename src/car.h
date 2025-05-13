#ifndef CAR_H
#define CAR_H

#include "raylib.h"

typedef struct {
    Color   color;
    Vector2 pos;
    float   angle;
} Checkpoint;

typedef struct {
    Color color;
    float dragForce;
} TrackArea;

typedef struct {
    int       id;
    int       lap;
    double    startLapTime;
    double    bestLapTime;
    double    raceTime;
    int       checkpoint;
    Vector2   pos;
    float     vel;
    float     acc;
    Texture2D texture;
    int       width;
    int       height;
    float     angle;
    float     angularAcc;
    float     minTurnSpeed;
    float     breakForce;
    float     dragForce;
    float     reverseForce;
} Car;

void Track_setMask(char *track_mask_path); // Definindo a mascara de pixel para carros lerem
void Track_setOutsideColor(Color color);
void Track_setAreas(TrackArea areas[], int size);

void Track_setCheckpoints(Checkpoint checkpoints[], int size); // Define as cores dos checkpoints

void Track_Unload(); // Libera a memória de recursos da mascara de pixels

Car *Car_create(Vector2 pos,   // posição inicial
                float   angle, // orientação inicial

                float acc,             // aceleração
                float reverseForce,    // força de ré
                float breakCoeficient, // coeficiente de frenagem

                float angularAcc,   // aceleração angular
                float minTurnSpeed, // velocidade mínima para virar

                int width,  // largura do carro
                int height, // altura do carro

                const char *texturePath, // path da textura
                int         id           // identificador único
);

Car *Car_createEmpty();

void Car_free(Car *car); // Libera a memória de um carro

void Car_update(Car *car); // Atualizar a posição do carro a cada frame
void Car_draw(Car *car);   // Desenhar o carro na tela

void Car_move(Car *car, int up, int down, int right, int left,
              int q); // Atualiza o carro de acordo com os inputs do usuário

void Car_showInfo(Car *car, int x, int y, int fontSize,
                  Color fontColor); // Mostra as informações do carro no console (PARA DEBUG)

#endif

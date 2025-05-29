#ifndef CAR_H
#define CAR_H

#include "common.h"
#include "raylib.h"
#include <SDL2/SDL_gamecontroller.h>

typedef struct {
    int       id;
    int       lap;
    double    startLapTime;
    double    lastLapTime;
    double    bestLapTime;
    int       checkpoint;
    Vector2   pos;
    float     vel;
    float     acc;
    Texture2D texture;
    Music     sound;
    Color     color;
    Color     floorColor;
    bool      ghost;
    bool      ghostActive;
    bool      changeLapFlag;
    int       width;
    int       height;
    float     angle;
    float     angularSpeed;
    float     minTurnSpeed;
    float     maxAngularSpeed;
    float     minAngularSpeed;
    float     breakForce;
    float     dragForce;
    float     reverseForce;
    float     maxVelocity;
    char      name[32];
    int       refFrame;
} Car;

void Track_setMask(char *track_mask_path); // Definindo a mascara de pixel para carros lerem

void Track_setCheckpoints(Checkpoint checkpoints[], int size); // Define as cores dos checkpoints

void Track_Unload(); // Libera a memória de recursos da mascara de pixels

Car *Car_create(Vector2 pos,   // posição inicial
                float   angle, // orientação inicial

                CarConfig   config,
                const char *texturePath, // path da textura
                Color color, bool ghost,
                int   id, // identificador único
                char *name);

void Car_free(Car *car); // Libera a memória de um carro

void Car_update(Car *car); // Atualizar a posição do carro a cada frame
void Car_draw(Car *car);   // Desenhar o carro na tela

void Car_move(Car *car, int up, int down, int right, int left, SDL_GameController *controller);

#endif

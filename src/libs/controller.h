#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <raylib.h>

typedef struct ControllerInput {
    Vector2 rightAxis;
    Vector2 leftAxis;
    float   rt;
    float   lt;
    bool    up;
    bool    down;
    bool    left;
    bool    right;
    bool    a;
    bool    b;
    bool    x;
    bool    y;
} ControllerInput;

void            Controllers_init(SDL_GameController *controllers[], int *controllers_n);
ControllerInput Controller_input(SDL_GameController *controller);

#endif

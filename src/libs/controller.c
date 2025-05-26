#include "controller.h"
#include <SDL2/SDL.h>
#include <raylib.h>

void Controllers_init(SDL_GameController *controllers[], int *controllers_n) {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            if (SDL_GameControllerOpen(i)) {
                controllers[*controllers_n] = SDL_GameControllerOpen(i);
                printf("Controle: %s\n", SDL_GameControllerName(controllers[*controllers_n]));
                (*controllers_n)++;
            }

            if (*controllers_n >= 2) {
                break;
            }
        }
    }
}

ControllerInput Controller_input(SDL_GameController *controller) {
    if (!controller) {
        return (ControllerInput) {0};
    }

    float rightX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
    float rightY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;

    float leftX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
    float leftY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;

    float rt = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
    float lt = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;

    return (ControllerInput) {{rightX, rightY}, {leftX, leftY}, rt, lt};
}

#include "controller.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

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

    bool up    = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
    bool down  = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    bool left  = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    bool right = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    bool a = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
    bool b = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
    bool x = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
    bool y = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);

    bool menu = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);

    return (ControllerInput) {
        {rightX, rightY}, {leftX, leftY}, rt, lt, up, down, left, right, a, b, x, y, menu};
}

ControllerInput Controller_allButtonInputs(SDL_GameController *controllers[], int controller_n) {
    ControllerInput temp = {0};

    for (int i = 0; i < controller_n; i++) {
        ControllerInput input = Controller_input(controllers[i]);
        temp.up |= input.up;
        temp.down |= input.down;
        temp.left |= input.left;
        temp.right |= input.right;

        temp.a |= input.a;
        temp.b |= input.b;
        temp.x |= input.x;
        temp.y |= input.y;

        temp.menu |= input.menu;
    }

    return temp;
}

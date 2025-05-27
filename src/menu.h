#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef enum MenuStep {
    CHOOSE_MODE,
    CHOOSE_MAP,
} MenuStep;

typedef struct {
    char    text[100];
    Vector2 pos;
    bool    selected;
} Button;

//----------------------------------------------------------------------------------
// Funções públicas
//----------------------------------------------------------------------------------

void Menu_setup(void (*play)());
void Menu_cleanup();
void Menu_update();
void Menu_draw();

#endif

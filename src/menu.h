#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef struct {
    char    text[100];
    Vector2 pos;
    bool    hovered;
    bool    selected;
    void (*action)();
} Button;

//----------------------------------------------------------------------------------
// Funções públicas
//----------------------------------------------------------------------------------

void Menu_setup(void (*play)());
void Menu_cleanup();
void Menu_update();
void Menu_draw();

#endif

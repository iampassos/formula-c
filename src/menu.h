#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef struct {
    char    text[100];
    Vector2 pos;
    int     hovered;
} Button;

void Menu_setup();
void Menu_cleanup();
void Menu_update();
void Menu_draw();

#endif

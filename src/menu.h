#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef struct {
    char    text[100];
    Vector2 pos;
    int     hovered;
} Button;

void setup_menu();
void update_menu();
void draw_menu();

#endif

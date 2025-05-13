#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef struct {
    char text[100];
    Rectangle rect;
    int hovered;
    void (*action)();
} Button;

void setup_menu();
void update_menu();
void draw_menu();

#endif

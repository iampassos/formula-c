#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "raylib.h"

typedef struct {
    Vector2 pos;
    float   angle;
    double  time;
} CarFrame;

typedef struct {
    unsigned int length;
    unsigned int capacity;
    CarFrame    *data;
} ArrayList;

ArrayList *ArrayList_create();
void       ArrayList_free(ArrayList *array);

void         ArrayList_push(ArrayList *array, CarFrame car);
unsigned int ArrayList_length(ArrayList *array);

void ArrayList_clear(ArrayList *array);
void ArrayList_copy(ArrayList *a, ArrayList *b);

CarFrame ArrayList_get(ArrayList *array, unsigned int idx);

#endif

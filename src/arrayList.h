#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "raylib.h"

typedef struct {
    Vector2 pos;
    float angle;
} GhostCarFrame;

typedef struct {
    unsigned int length;
    unsigned int capacity;
    GhostCarFrame* data;
} ArrayList;

ArrayList* ArrayList_create();
void ArrayList_free(ArrayList* arr);

void ArrayList_push(ArrayList* arr, GhostCarFrame car);
unsigned int ArrayList_length(ArrayList* arr);

void ArrayList_clear(ArrayList* arr);
void ArrayList_copy(ArrayList* a, ArrayList* b);

GhostCarFrame ArrayList_get(ArrayList* arr, unsigned int idx);

#endif
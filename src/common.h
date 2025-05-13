#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

typedef enum { INTERLAGOS } Map;

typedef enum { SINGLEPLAYER, SPLITSCREEN } Mode;

typedef enum { MENU, GAME } Screen;

typedef struct {
    Screen screen;
    Map    map;
    Mode   mode;
} State;

extern State state;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

#endif

#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#define CHECKPOINTS_SIZE 3

typedef struct {
    Color   color;
    Vector2 pos;
    float   angle;
} Checkpoint;

typedef struct {
    char      *backgroundPath;
    char      *maskPath;
    Checkpoint checkpoints[CHECKPOINTS_SIZE];
} Map;

typedef struct {
    Color color;
    float dragForce;
} TrackArea;

typedef enum { SINGLEPLAYER, SPLITSCREEN } Mode;

typedef enum { MENU, GAME } Screen;

typedef struct {
    Screen screen;
    Mode   mode;
} State;

extern State state;

extern int       SCREEN_WIDTH;
extern int       SCREEN_HEIGHT;
extern char     *gameMusicPath;
extern char     *carSoundPath;
extern float     carVolume;
extern float     gameMusicVolume;
extern char     *gameName;
extern char     *backgroundPath;
extern char     *clickButtonSoundPath;
extern char     *menuMusicPath;
extern Map       maps[];
extern TrackArea TRACK_AREAS[];
extern int       TRACK_AREA_SIZE;
extern Color     OUTSIDE_TRACK_COLOR;

#endif

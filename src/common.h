#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

typedef struct {
    Vector2 pos;
    float   angle;
    bool    isRaceStart;
} Checkpoint;

typedef struct {
    char      *name;
    char      *backgroundPath;
    char      *maskPath;
    Vector2    startCarPos;
    float      startAngle;
    int        checkpointSize;
    Checkpoint checkpoints[100];
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

// Configurações de tela
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// Informações do jogo
extern char *GAME_NAME;
extern char  GAME_MODES[][100];
extern int   TOTAL_GAME_MODES;

// Dados do ghost car
extern char *BEST_LAP_DATA_PATH;

// Áudio
extern char *GAME_MUSIC_PATH;
extern char *MENU_MUSIC_PATH;
extern char *CAR_SOUND_PATH;
extern char *CLICK_BUTTON_SOUND_PATH;
extern float GAME_MUSIC_VOLUME;
extern float MENU_MUSIC_VOLUME;
extern float CAR_VOLUME;

// Mapas e pistas
extern Map       MAPS[];
extern int       TOTAL_MAPS;
extern int       SELECTED_MAP_IDX;
extern TrackArea TRACK_AREAS[];
extern int       TRACK_AREA_SIZE;
extern Color     OUTSIDE_TRACK_COLOR;
// extern Color     RACE_START_COLOR;
// extern Color     CHECKPOINT_COLOR;

// Recursos visuais
extern char *BACKGROUND_PATH;
extern char *CAR_IMAGE_PATH;
extern char *LOGO_IMAGE_PATH;
extern int   HUD_OPACITY;

#endif
#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

typedef struct {
    Vector2 pos;
    float   angle;
} Checkpoint;

typedef struct {
    char      *name;
    char      *backgroundPath;
    char      *maskPath;
    char      *minimapPath;
    Vector2    startCarPos;
    float      startAngle;
    int        checkpointSize;
    Checkpoint checkpoints[100];
} Map;

typedef struct {
    Color color;
    float dragForce;
} TrackArea;

typedef enum { INTERLAGOS } CurrentMap;

typedef enum { SINGLEPLAYER, SPLITSCREEN } Mode;

typedef enum { MENU, GAME } Screen;

typedef enum { FIRST_PERSON, THIRD_PERSON } CameraView;

typedef struct {
    Screen     screen;
    Mode       mode;
    bool       debug;
    CurrentMap map;
    CameraView cameraView;
} State;

typedef struct {
    float acc;
    float reverseForce;
    float breakForce;
    float angularSpeed;
    int   width;
    int   height;
} CarConfig;

extern CarConfig DEFAULT_CAR_CONFIG;

extern State state;

// Configurações de tela
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// Suavidade da camera
extern float CAMERA_SMOOTHNESS;

// Informações do jogo
extern char *GAME_NAME;
extern char  GAME_MODES[][100];
extern int   TOTAL_GAME_MODES;

// Pasta de gravação dos arquivos dos carros fantasmas
extern char *GHOST_CAR_DATA_PATH;

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
extern TrackArea TRACK_AREAS[];
extern int       TRACK_AREA_SIZE;
extern Color     OUTSIDE_TRACK_COLOR;
extern Color     CHECKPOINTS_COLOR;

// Recursos visuais
extern char *BACKGROUND_PATH;
extern char *CAR_IMAGE_PATH;
extern char *LOGO_IMAGE_PATH;
extern int   HUD_OPACITY;
extern char *CAR_IMAGES_PATH[];

#endif

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
    char      *referencePath;
    Vector2    startCarPos[2];
    float      startAngle;
    int        checkpointSize;
    Checkpoint checkpoints[100];
} Map;

typedef struct {
    Color color;
    float dragForce;
} TrackArea;

typedef enum { INTERLAGOS, SECRET } CurrentMap;

typedef enum { SINGLEPLAYER, SPLITSCREEN } Mode;

typedef enum { MENU, GAME } Screen;

typedef enum { FIRST_PERSON, THIRD_PERSON } CameraView;

typedef enum { COUNTDOWN, STARTED, ENDED } Status;

typedef struct {
    Screen     screen;
    Mode       mode;
    bool       debug;
    CurrentMap map;
    CameraView cameraView;
    double     raceTime;
    Status     status;
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
extern char GAME_MODES[][100];
extern int  TOTAL_GAME_MODES;

// Pasta de gravação dos arquivos dos carros fantasmas
extern char *GHOST_CAR_DATA_PATH;

// Áudio
extern char *GAME_MUSIC_PATH;
extern char *MENU_MUSIC_PATH;
extern char *CAR_SOUND_PATH;
extern char *CLICK_BUTTON_SOUND_PATH;
extern char *SMAPHORE_SOUND_PATH;
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
extern char *SEMAPHORE_PATH[];
extern char *BACKGROUND_PATH;
extern char *CAR_IMAGE_PATH;
extern char *LOGO_BG_IMAGE_PATH;
extern char *LOGO_IMAGE_PATH;
extern int   HUD_OPACITY;
extern char *CAR_IMAGES_PATH[];
extern char *SPEEDOMETER_PATH;

extern char *FONTS_PATH[];
extern int   FONTS_N;
extern Font  FONTS[];

extern char strBuffer[1000];

// Funções extras
bool  Color_equals(Color a, Color b);
float Vector2_dist(Vector2 a, Vector2 b);
float LerpAngle(float a, float b, float t);

void drawCenteredText(char *text, float x, float y, float width, float heigth, int size,
                      Color color, Font font);

void drawTextWithShadow(char *text, float x, float y, int size, Color color, Font font);

void stringifyTime(char *buffer, double time, int signFlag);

#endif

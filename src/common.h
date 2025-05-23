#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

//----------------------------------------------------------------------------------
// 📦 TIPOS PERSONALIZADOS
//----------------------------------------------------------------------------------

typedef struct {
    Vector2 pos;
    float   angle;
} Checkpoint;

typedef struct {
    char      *name;
    char      *backgroundPath;
    char      *maskPath;
    char      *minimapPath;
    Vector2    startCarPos[2];
    float      startAngle;
    int        checkpointSize;
    int        maxLaps;
    Checkpoint checkpoints[100];
} Map;

typedef struct {
    Color color;
    float dragForce;
} TrackArea;

typedef struct {
    float acc;
    float reverseForce;
    float breakForce;
    float angularSpeed;
    int   width;
    int   height;
} CarConfig;

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
    int        maxLaps;
} State;

//----------------------------------------------------------------------------------
// 🧠 VARIÁVEIS GLOBAIS
//----------------------------------------------------------------------------------

// Configurações de tela
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int MAP_WIDTH;
extern int MAP_HEIGHT;

// Estado do jogo e câmera
extern State state;
extern float CAMERA_SMOOTHNESS;

// Configurações de carro
extern CarConfig DEFAULT_CAR_CONFIG;

// Modos de jogo
extern char GAME_MODES[][100];
extern int  TOTAL_GAME_MODES;
extern int  MAX_LAPS;

// 📁 Arquivos de Dados
extern char *GHOST_CAR_DATA_PATH;
extern char *REFERENCE_DATA_PATH;

//----------------------------------------------------------------------------------
// 🔊 ÁUDIO
//----------------------------------------------------------------------------------

extern char *GAME_MUSIC_PATH;
extern char *MENU_MUSIC_PATH;
extern char *CAR_SOUND_PATH;
extern char *CLICK_BUTTON_SOUND_PATH;
extern char *SMAPHORE_SOUND_PATH;

extern float GAME_MUSIC_VOLUME;
extern float MENU_MUSIC_VOLUME;
extern float CAR_VOLUME;

//----------------------------------------------------------------------------------
// 🗺️ MAPAS E PISTAS
//----------------------------------------------------------------------------------

extern Map       MAPS[];
extern int       TOTAL_MAPS;
extern TrackArea TRACK_AREAS[];
extern int       TRACK_AREA_SIZE;

extern Color OUTSIDE_TRACK_COLOR;
extern Color CHECKPOINTS_COLOR;

//----------------------------------------------------------------------------------
// 🎨 RECURSOS VISUAIS
//----------------------------------------------------------------------------------

extern char *SEMAPHORE_PATH[];
extern char *BACKGROUND_PATH;
extern char *CAR_IMAGE_PATH;
extern char *CAR_IMAGES_PATH[];
extern char *LOGO_BG_IMAGE_PATH;
extern char *LOGO_IMAGE_PATH;
extern char *SPEEDOMETER_PATH;

extern int HUD_OPACITY;

// Fontes
extern char *FONTS_PATH[];
extern int   FONTS_N;
extern Font  FONTS[];

// Buffer auxiliar
extern char strBuffer[1000];

//----------------------------------------------------------------------------------
// 🛠️ FUNÇÕES UTILITÁRIAS
//----------------------------------------------------------------------------------

bool  Color_equals(Color a, Color b);
float Vector2_dist(Vector2 a, Vector2 b);
float LerpAngle(float a, float b, float t);

void drawCenteredText(char *text, float x, float y, float width, float height, int size,
                      Color color, Font font);

void drawTextWithShadow(char *text, float x, float y, int size, Color color, Font font);

void stringifyTime(char *buffer, double time, int signFlag);

#endif

#ifndef GAME_H
#define GAME_H

#include "arrayList.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"

//----------------------------------------------------------------------------------
// Funções públicas
//----------------------------------------------------------------------------------

void Game_setup();
void Game_cleanup();
void Game_load();
void Game_update();
void Game_draw();

//----------------------------------------------------------------------------------
// Funções de uso interno
//----------------------------------------------------------------------------------

void drawMap();
void drawPlayerHud(Car *player, int x);
void drawTextWithShadow(char *text, float x, float y, int size, Color color, Font font);
void drawLapTime(Car *player, float x, float y);
void drawPlayerInMinimap(Car *player);
void drawLaps(Car *player, float x, float y);
void drawSpeedometer(Car *player, float x, float y);

//----------------------------------------------------------------------------------
// Singleplayer
//----------------------------------------------------------------------------------

void loadSingleplayer(Map map);
void updateSingleplayer();
void drawSingleplayer();
void drawHudSingleplayer();
void cleanUpSingleplayer();

//----------------------------------------------------------------------------------
// Splitscreen
//----------------------------------------------------------------------------------

extern Car *winner;
extern int  MAX_LAPS;

void loadSplitscreen(Map map);
void updateSplitscreen();
void drawSplitscreen();
void drawHudSplitscreen();
void cleanUpSplitscreen();

//----------------------------------------------------------------------------------
// Variáveis internas
//----------------------------------------------------------------------------------

extern Texture2D trackBackground;
extern Texture2D trackHud;

extern LinkedList *cars;

extern Camera2D *camera1;
extern Camera2D *camera2;

extern char textBuffer[1000];
extern int  minimapWidth;
extern int  minimapHeigth;

extern Vector2 minimapPos;

#endif

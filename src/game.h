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

void loadAssets(Map *map);

//----------------------------------------------------------------------------------
// Funções de uso interno
//----------------------------------------------------------------------------------

void drawMap();
void drawPlayerHud(Car *player, int x);
void drawLapTime(Car *player, float x, float y);
void drawPlayerInMinimap(Car *player);
void drawLaps(Car *player, float x, float y);
void drawSpeedometer(Car *player, float x, float y);
void drawPlayerDebug(Car *player, int x, int y);
void drawPlayerList(Car *player, float x, float y);
void drawGameLogo(float x, float y);
void drawBestLapMessage(float x, float y);

//----------------------------------------------------------------------------------
// Singleplayer
//----------------------------------------------------------------------------------

void *loadSingleplayer(void *arg);
void  updateSingleplayer();
void  drawSingleplayer();
void  drawHudSingleplayer();
void  cleanUpSingleplayer();

//----------------------------------------------------------------------------------
// Splitscreen
//----------------------------------------------------------------------------------

void *loadSplitscreen(void *arg);
void  updateSplitscreen();
void  drawSplitscreen();
void  drawHudSplitscreen();
void  cleanUpSplitscreen();

//----------------------------------------------------------------------------------
// Variáveis internas
//----------------------------------------------------------------------------------

extern bool done;
extern bool loaded;
extern char load_msg[100];

extern LinkedList *cars;

extern Camera2D *camera1;
extern Camera2D *camera2;

extern Vector2 minimapPos;

extern Image carsImage[3];

extern Texture2D minimapTexture;
extern Texture2D trackTexture;
extern Texture2D debugMinimapTexture;
extern Texture2D debugTrackTexture;
extern Texture2D speedometerTexture;
extern Texture2D logoNoBgTexture;

extern Car *bestLapTimePlayer;

#endif

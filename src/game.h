#ifndef GAME_H
#define GAME_H

#include "arrayList.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"

void Game_setup();
void Game_cleanup();
void Game_load();
void Game_update();
void Game_draw();

void drawMap();
void drawPlayerHud(Car *player, int x);
void drawTextWithShadow(char *text, float x, float y, int size, Color color);
void drawLapTime(Car *player, float x, float y);
void drawPlayerInMinimap(Car *player);
void drawSpeedometer(Car *player, float x, float y);
void drawLaps(Car *player, float x, float y);

void loadSingleplayer(Map map);
void updateSingleplayer();
void drawSingleplayer();
void drawHudSingleplayer();
void cleanUpSingleplayer();

void loadSplitscreen(Map map);
void updateSplitscreen();
void drawSplitscreen();
void drawHudSplitscreen();
void cleanUpSplitscreen();

extern Texture2D trackBackground;
extern Texture2D trackHud;

extern LinkedList *cars;

extern Camera2D *camera1;
extern Camera2D *camera2;

extern char textBuffer[1000];
extern int  minimapWidth;
extern int  minimapHeigth;

extern Vector2 minimapPos;

extern Car *winner;

extern int MAX_LAPS;

#endif

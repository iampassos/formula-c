#ifndef GAME_H
#define GAME_H

#include "common.h"

void Game_loadSingleplayer();
void Game_loadSplitscreen();

void Game_mapCleanup();

void Game_setup();

void Game_cleanup();
void Game_update();
void Game_draw();

#endif

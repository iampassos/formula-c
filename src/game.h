#ifndef GAME_H
#define GAME_H

#include "car.h"
#include "common.h"

void Game_loadSingleplayer();

void Game_loadMap(Map map);
void Game_map_cleanup();

void Game_setup();

void Game_cleanup();
void Game_update();
void Game_draw();

#endif

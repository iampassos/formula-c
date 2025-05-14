#ifndef GAME_H
#define GAME_H

#include "car.h"
#include "common.h"

void setup_game(Mode mode, Map map);

void cleanup_game();
void update_game();
void draw_game();

#endif

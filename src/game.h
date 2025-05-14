#ifndef GAME_H
#define GAME_H

#include "car.h"
#include "common.h"

// Checkpoints
#define CHECKPOINTS                                                                                \
    (Checkpoint[]) {                                                                               \
        {(Color) {0, 255, 0}, (Vector2) {4371, 2537}, 2.66f},                                      \
            {(Color) {0, 0, 255}, (Vector2) {6700, 8147}, 0.0f},                                   \
            {(Color) {255, 0, 0}, (Vector2) {11069, 2257}, 2.17f}                                  \
    }

void setup_game(Mode mode, Map map);

void cleanup_game();
void update_game();
void draw_game();

#endif

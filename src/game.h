#ifndef GAME_H
#define GAME_H

#include "car.h"
#include "common.h"

// Areas da pista
#define TRACK_AREAS                                                                                                    \
    (TrackArea[]) {                                                                                                    \
        /* pista */ {(Color) {127, 127, 127}, 0.01}, /* fraca */ {(Color) {255, 127, 39}, 0.04},                       \
            /* forte */ {(Color) {163, 73, 164}, 0.07}, /* grama */ {(Color) {34, 177, 76}, 0.02}                      \
    }

// Cor que representa fora da pista
#define OUTSIDE_TRACK_COLOR (Color){255, 255, 255}

// Checkpoints
#define CHECKPOINTS                                                                                                    \
    (Checkpoint[]) {                                                                                                   \
        {(Color) {0, 255, 0}, (Vector2) {4371, 2537}, 2.66f}, {(Color) {0, 0, 255}, (Vector2) {6700, 8147}, 0.0f},     \
            {(Color) {255, 0, 0}, (Vector2) {11069, 2257}, 2.17f}                                                      \
    }

void setup_game(Mode mode);

void cleanup_game();
void update_game();
void draw_game();

#endif

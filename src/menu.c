#include "common.h"
#include "game.h"

void update_menu() {
    if (IsKeyDown(KEY_ENTER)) {
        setup_game(SINGLEPLAYER);
        state.screen = GAME;
    }
}

void draw_menu() {
    ClearBackground(BLACK);
    char *text = "Pressione ENTER para continuar";
    DrawText(text, (SCREEN_WIDTH - MeasureText(text, 20)) / 2.0f, (SCREEN_HEIGHT - 20) / 2.0f, 20,
             WHITE);
}

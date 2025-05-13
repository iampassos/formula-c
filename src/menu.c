#include "menu.h"
#include "common.h"
#include "game.h"
#include <raylib.h>

static const int width  = 200;
static const int height = 50;

static Button buttons[] = {
    (Button) {"1 JOGADOR", (Vector2) {0, 0}, 0},
    (Button) {"2 JOGADORES", (Vector2) {0, 0}, 0},
};

static const int buttonsLen = sizeof(buttons) / sizeof(Button);

void setup_menu() {
    for (int i = 0; i < buttonsLen; i++) {
        buttons[i].pos.y = (SCREEN_HEIGHT + (i * height * 2.5f)) / 2.0f;
        buttons[i].pos.x = (SCREEN_WIDTH - width) / 2.0f;
    }
}

void update_menu() {
    Vector2 mouse = GetMousePosition();

    if (CheckCollisionPointRec(mouse,
                               (Rectangle) {buttons[0].pos.x, buttons[0].pos.y, width, height})) {
        buttons[0].hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            setup_game(SINGLEPLAYER);
            state.screen = GAME;
        }
    } else {
        buttons[0].hovered = 0;
    }

    if (CheckCollisionPointRec(mouse,
                               (Rectangle) {buttons[0].pos.x, buttons[0].pos.y, width, height})) {
        buttons[0].hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            setup_game(SINGLEPLAYER);
            state.screen = GAME;
        }
    } else {
        buttons[0].hovered = 0;
    }
}

void draw_menu() {
    ClearBackground(BLACK);

    char *text = "FORMULA C";
    DrawText(text, (SCREEN_WIDTH - MeasureText(text, 20)) / 2.0f,
             (SCREEN_HEIGHT / 2.0f) - (height * 2), 20, WHITE);

    for (int i = 0; i < buttonsLen; i++) {
        Button    btn  = buttons[i];
        Rectangle rect = (Rectangle) {btn.pos.x, btn.pos.y, width, height};
        DrawRectangleRec(rect, btn.hovered ? GOLD : WHITE);
        DrawText(btn.text, (rect.x + ((rect.width - MeasureText(btn.text, 20)) / 2.0f)),
                 (rect.y + ((rect.height - 20) / 2.0f)), 20, BLACK);
    }
}

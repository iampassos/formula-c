#include "menu.h"
#include "common.h"
#include "game.h"
#include <raylib.h>

static const int width  = 200;
static const int height = 50;
static const int padding = 50; // Espaçamento entre os botões

static const int textBoxHeight = 20;

static char* textContent = "FORMULA C";
static int fontSize = 30;

static Button BUTTONS[] = {
    {"1 JOGADOR", {0, 0}, 0},
    {"2 JOGADORES", {0, 0}, 0},
};

static const int buttonsLen = sizeof(BUTTONS) / sizeof(Button);

static Vector2 textBox = {0, 0};

void setup_menu() {
    int dy = padding + height;
    int y = (SCREEN_HEIGHT - dy * (buttonsLen - 1) + padding) / 2;
    for (int i = 0; i < buttonsLen; i++) {
        BUTTONS[i].pos.y = y;
        BUTTONS[i].pos.x = (SCREEN_WIDTH - width) / 2.0f;
        y += dy;
    }
    
    textBox.x = (SCREEN_WIDTH - MeasureText(textContent, fontSize)) / 2;
    textBox.y = (SCREEN_HEIGHT - dy * buttonsLen + textBoxHeight) / 2;
}

void update_menu() {
    Vector2 mouse = GetMousePosition();

    if (CheckCollisionPointRec(mouse,
                               (Rectangle) {BUTTONS[0].pos.x, BUTTONS[0].pos.y, width, height})) {
        BUTTONS[0].hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            setup_game(SINGLEPLAYER);
            state.screen = GAME;
        }
    } else {
        BUTTONS[0].hovered = 0;
    }

    if (CheckCollisionPointRec(mouse,
                               (Rectangle) {BUTTONS[1].pos.x, BUTTONS[1].pos.y, width, height})) {
        BUTTONS[1].hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            setup_game(SINGLEPLAYER);
            state.screen = GAME;
        }
    } else {
        BUTTONS[1].hovered = 0;
    }
}

void draw_menu() {
    ClearBackground(BLACK);
    DrawText(textContent, textBox.x, textBox.y, fontSize, PURPLE);

    for (int i = 0; i < buttonsLen; i++) {
        Button    btn  = BUTTONS[i];
        Rectangle rect = (Rectangle) {btn.pos.x, btn.pos.y, width, height};
        DrawRectangleRec(rect, btn.hovered ? GOLD : WHITE);
        DrawText(btn.text, (rect.x + ((rect.width - MeasureText(btn.text, 20)) / 2)),
                 (rect.y + ((rect.height - textBoxHeight) / 2)), textBoxHeight, BLACK);
    }
}
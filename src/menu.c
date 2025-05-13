#include "menu.h"
#include "common.h"
#include "game.h"

static const int width  = 200;
static const int height = 50;

static Button buttons[] = {
    (Button) {"1 JOGADOR", (Rectangle) {0, 0, width, height}, 0},
    (Button) {"2 JOGADORES", (Rectangle) {0, 0, width, height}, 0},
};

static const int buttonsLen = sizeof(buttons) / sizeof(Button);

void setup_menu() {
    int y;
    for (int i = 0; i < buttonsLen; i++) {
        y                 = SCREEN_HEIGHT * ((i + 1) / (float) (buttonsLen + 1));
        buttons[i].rect.y = y;
        buttons[i].rect.x = SCREEN_WIDTH / 2.0f - buttons[i].rect.width / 2.0f;
    }
}

void update_menu() {
    Vector2 mouse = GetMousePosition();

    if (CheckCollisionPointRec(mouse, buttons[0].rect)) {
        buttons[0].hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            setup_game(SINGLEPLAYER);
            state.screen = GAME;
        }
    } else {
        buttons[0].hovered = 0;
    }

    if (CheckCollisionPointRec(mouse, buttons[1].rect)) {
        buttons[1].hovered = -1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        }
    } else {
        buttons[1].hovered = 0;
    }
}

void draw_menu() {
    ClearBackground(BLACK);

    char *text = "FORMULA C";
    DrawText(text, (SCREEN_WIDTH - MeasureText(text, 20)) / 2.0f,
             (SCREEN_HEIGHT / 4.0f) - (height * 2), 20, WHITE);

    for (int i = 0; i < buttonsLen; i++) {
        Button btn = buttons[i];
        DrawRectangleRec(btn.rect, btn.hovered ? GOLD : WHITE);
        DrawText(btn.text, (btn.rect.x + ((btn.rect.width - MeasureText(btn.text, 20)) / 2.0f)),
                 (btn.rect.y + ((btn.rect.height - 20) / 2.0f)), 20, BLACK);
    }
}

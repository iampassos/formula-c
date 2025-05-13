#include "common.h"
#include "game.h"

int width  = 200;
int height = 50;

Rectangle button_singleplayer = {0};
int       is_sglplr_hovered   = 0;

Rectangle button_splitscreen     = {0};
int       is_splitscreen_hovered = 0;

void update_menu() {
    Vector2 mouse = GetMousePosition();

    if (CheckCollisionPointRec(mouse, button_singleplayer)) {
        is_sglplr_hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            setup_game(SINGLEPLAYER);
            state.screen = GAME;
        }
    } else {
        is_sglplr_hovered = 0;
    }

    if (CheckCollisionPointRec(mouse, button_splitscreen)) {
        is_splitscreen_hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        }
    } else {
        is_splitscreen_hovered = 0;
    }
}

static void draw_button(Rectangle button, char *text, int hovered) {
    DrawRectangleRec(button, hovered ? GOLD : WHITE);
    DrawText(text, (button.x + ((button.width - MeasureText(text, 20)) / 2.0f)),
             (button.y + ((button.height - 20) / 2.0f)), 20, BLACK);
}

void draw_menu() {
    ClearBackground(BLACK);

    char *text = "FORMULA C";
    DrawText(text, (SCREEN_WIDTH - MeasureText(text, 20)) / 2.0f,
             (SCREEN_HEIGHT / 2.0f) - (height * 2), 20, WHITE);

    button_singleplayer = (Rectangle) {(SCREEN_WIDTH - width) / 2.0f,
                                       (SCREEN_HEIGHT - height - 10) / 2.0f, width, height};

    button_splitscreen = (Rectangle) {(SCREEN_WIDTH - width) / 2.0f,
                                      (SCREEN_HEIGHT + height + 10) / 2.0f, width, height};

    draw_button(button_singleplayer, "1 JOGADOR", is_sglplr_hovered);
    draw_button(button_splitscreen, "2 JOGADORES", is_splitscreen_hovered);
}

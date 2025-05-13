#include "menu.h"
#include "common.h"
#include "game.h"

//static const int width = 200;
static const int height = 50;

static void singleplayerAction() {
    setup_game(SINGLEPLAYER);
    state.screen = GAME;
}

static void splitscreenAction() {
    return;
}

static Button buttons[] = {
    (Button){"1 Jogador", (Rectangle){300, 300, 100, 100}, 0, singleplayerAction},
    (Button){"2 Jogadores", (Rectangle){300, 500, 100, 100}, 0, splitscreenAction},
};

static const int buttonsLen = sizeof(buttons) / sizeof(Button);

static Vector2 mousePos;

void setup_menu(){
    int y;
    for (int i = 0; i < buttonsLen; i++){
        y = SCREEN_HEIGHT * ((i + 1) / (float)(buttonsLen + 1));
        buttons[i].rect.y = y;
        buttons[i].rect.x = SCREEN_WIDTH / 2 - buttons[i].rect.width / 2;
    }
}

static void draw_button(Button btn) {
    DrawRectangleRec(btn.rect, btn.hovered ? GOLD : WHITE);
    DrawText(btn.text, (btn.rect.x + ((btn.rect.width - MeasureText(btn.text, 20)) / 2.0f)),
             (btn.rect.y + ((btn.rect.height - 20) / 2.0f)), 20, BLACK);
}

static int button_action(Button *button) {
    if (CheckCollisionPointRec(mousePos, button->rect)) {
        button->hovered = 1;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            button->action();
            return 1;
        }
    } else
        button->hovered = 0;

    return 0;
}

void update_menu() {
    mousePos = GetMousePosition();

    for (int i = 0; i < buttonsLen; i++) {
        if (button_action(buttons + i))
            return;
    }
}

void draw_menu() {
    ClearBackground(BLACK);

    char *text = "FORMULA C";
    DrawText(text, (SCREEN_WIDTH - MeasureText(text, 20)) / 2.0f,
             (SCREEN_HEIGHT / 4.0f) - (height * 2), 20, WHITE);

    for (int i = 0; i < buttonsLen; i++) {
        draw_button(buttons[i]);
    }
}

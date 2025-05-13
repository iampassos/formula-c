#include "menu.h"
#include "common.h"
#include "game.h"
#include <raylib.h>

static char *textContent = "FORMULA C";
static Color titleColor  = {0, 100, 255, 255};

static Button BUTTONS[] = {
    {"1 JOGADOR", {0, 0}, 0},
    {"2 JOGADORES", {0, 0}, 0},
};

static const int buttonsLen = sizeof(BUTTONS) / sizeof(Button);

static Vector2 textBox = {0, 0};

static int width;
static int height;
static int padding;
static int buttonFontSize;
static int titleFontSize;

void setup_menu() {
    width   = SCREEN_WIDTH / 5;
    height  = SCREEN_HEIGHT / 10;
    padding = SCREEN_HEIGHT / 9;

    titleFontSize  = SCREEN_WIDTH / 25;
    buttonFontSize = SCREEN_WIDTH / 40;

    int dy = padding + height;
    int y  = (SCREEN_HEIGHT - dy * (buttonsLen - 1) + padding) / 2;
    for (int i = 0; i < buttonsLen; i++) {
        BUTTONS[i].pos.y = y;
        BUTTONS[i].pos.x = (SCREEN_WIDTH - width) / 2.0f;
        y += dy;
    }

    textBox.x = (SCREEN_WIDTH - MeasureText(textContent, titleFontSize)) / 2;
    textBox.y = (SCREEN_HEIGHT - dy * buttonsLen ) / 2;
}

void DrawButton(Button btn) {
    Rectangle rect      = (Rectangle) {btn.pos.x, btn.pos.y, width, height};
    Color     baseColor = btn.hovered ? GOLD : WHITE;
    Color     textColor = BLACK;

    DrawRectangleRounded((Rectangle) {rect.x + 4, rect.y + 4, rect.width, rect.height}, 0.3f, 10,
                         GRAY);

    float     scale  = btn.hovered ? 1.05f : 1.0f;
    Rectangle scaled = {rect.x - rect.width * (scale - 1) / 2,
                        rect.y - rect.height * (scale - 1) / 2, rect.width * scale,
                        rect.height * scale};
    DrawRectangleRounded(scaled, 0.3f, 10, baseColor);

    DrawText(btn.text, rect.x + (rect.width - MeasureText(btn.text, buttonFontSize)) / 2,
             rect.y + (rect.height - buttonFontSize) / 2, buttonFontSize, textColor);
}

void update_menu() {
    Vector2 mouse = GetMousePosition();

    BUTTONS[0].hovered = CheckCollisionPointRec(
        mouse, (Rectangle) {BUTTONS[0].pos.x, BUTTONS[0].pos.y, width, height});

    if (BUTTONS[0].hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        setup_game(SINGLEPLAYER);
        state.screen = GAME;
    }

    BUTTONS[1].hovered = CheckCollisionPointRec(
        mouse, (Rectangle) {BUTTONS[1].pos.x, BUTTONS[1].pos.y, width, height});

    if (BUTTONS[1].hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    }
}

void draw_menu() {
    ClearBackground(BLACK);
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        Color grad = ColorLerp(BLACK, RED, (float) i / SCREEN_HEIGHT);
        DrawLine(0, i, SCREEN_WIDTH, i, grad);
    }
    DrawText(textContent, textBox.x + 2, textBox.y + 2, titleFontSize, WHITE); // sombra
    DrawText(textContent, textBox.x, textBox.y, titleFontSize, titleColor);

    for (int i = 0; i < buttonsLen; i++) {
        DrawButton(BUTTONS[i]);
    }
}
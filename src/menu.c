#include "menu.h"
#include "common.h"
#include "game.h"
#include "raylib.h"
#include <math.h>

static Button BUTTONS[] = {
    {"1 JOGADOR", {0, 0}, 0},
    {"2 JOGADORES", {0, 0}, 0},
};

static Sound     clickSound;
static Music     music;
static const int buttonsLen = sizeof(BUTTONS) / sizeof(Button);
static Vector2   textBox    = {0, 0};
static Texture2D background;
static int       width;
static int       height;
static int       padding;
static int       buttonFontSize;
static int       titleFontSize;

void Menu_setup() {
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

    textBox.x = (SCREEN_WIDTH - MeasureText(gameName, titleFontSize)) / 2.0f;
    textBox.y = (SCREEN_HEIGHT - dy * buttonsLen) / 2.0f;

    Image img = LoadImage(backgroundPath);
    ImageResize(&img, SCREEN_WIDTH, SCREEN_HEIGHT); // redimensiona a imagem
    background = LoadTextureFromImage(img);
    UnloadImage(img);

    clickSound = LoadSound(clickButtonSoundPath);
    music      = LoadMusicStream(menuMusicPath);

    PlayMusicStream(music);
}

void Menu_cleanup() {
    UnloadSound(clickSound);
    UnloadMusicStream(music);
}

static void drawButton(Button btn) {
    Rectangle rect       = (Rectangle) {btn.pos.x, btn.pos.y, width, height};
    Color     baseColor  = btn.hovered ? GOLD : WHITE;
    Color     textColor  = BLACK;
    Rectangle shadowRect = (Rectangle) {rect.x + 6, rect.y + 6, rect.width, rect.height};

    float     scale  = btn.hovered ? 1.05f : 1.0f;
    Rectangle scaled = {rect.x - rect.width * (scale - 1) / 2,
                        rect.y - rect.height * (scale - 1) / 2, rect.width * scale,
                        rect.height * scale};

    DrawRectangleRounded(shadowRect, 0.3f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(scaled, 0.3f, 10, baseColor);

    DrawText(btn.text, rect.x + (rect.width - MeasureText(btn.text, buttonFontSize)) / 2,
             rect.y + (rect.height - buttonFontSize) / 2, buttonFontSize, textColor);
}

void Menu_update() {
    Vector2 mouse = GetMousePosition();

    BUTTONS[0].hovered = CheckCollisionPointRec(
        mouse, (Rectangle) {BUTTONS[0].pos.x, BUTTONS[0].pos.y, width, height});

    if (BUTTONS[0].hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound);
        Game_loadMap(maps[0]);
        Game_loadSingleplayer();
        state.mode = SINGLEPLAYER;
        state.screen = GAME;
    }

    BUTTONS[1].hovered = CheckCollisionPointRec(
        mouse, (Rectangle) {BUTTONS[1].pos.x, BUTTONS[1].pos.y, width, height});

    if (BUTTONS[1].hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound);
    }

    SetMusicVolume(music, gameMusicVolume); // Se precisar abaixar o som da música
    UpdateMusicStream(music);
}

void Menu_draw() {
    // 1. Fundo
    DrawTexture(background, 0, 0, WHITE);

    // 2. Gradiente por cima (leve transparência)
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        Color grad = ColorLerp(BLACK, RED, (float) i / SCREEN_HEIGHT);
        grad.a     = 100; // transparência
        DrawLine(0, i, SCREEN_WIDTH, i, grad);
    }

    // 3. Título com sombra e cor pulsante
    float t          = sinf(GetTime()) * 0.5f + 0.5f; // varia de 0 a 1
    Color pulseColor = ColorLerp(RED, BLUE, t);
    DrawText(gameName, textBox.x + 2, textBox.y + 2, titleFontSize, WHITE); // sombra
    DrawText(gameName, textBox.x, textBox.y, titleFontSize, pulseColor);

    // 4. Botões
    for (int i = 0; i < buttonsLen; i++) {
        drawButton(BUTTONS[i]);
    }
}
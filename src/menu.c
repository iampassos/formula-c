#include "menu.h"
#include "common.h"
#include "game.h"
#include "raylib.h"
#include <math.h>
#include <string.h>

// Botões
static Button BUTTONS[10];
static Button MAPS_BUTTONS[10];

// Áudio
static Sound clickSound;
static Music music;

// Recursos visuais
static Texture2D background;
static Vector2 textBox;

// Dimensões e espaçamento
static int width;
static int height;
static int padding;

// Tamanhos de fonte
static int buttonFontSize;
static int titleFontSize;

void interlagosMapButtonAction() {
    SELECTED_MAP_IDX = 0;
}

void debugMapButtonAction() {
    SELECTED_MAP_IDX = 1;
}

void Menu_setup() {
    BUTTONS[0].action      = Game_loadSingleplayer;
    BUTTONS[1].action      = Game_loadSplitscreen;
    MAPS_BUTTONS[0].action = interlagosMapButtonAction;
    MAPS_BUTTONS[1].action = debugMapButtonAction;

    width   = SCREEN_WIDTH / 5;
    height  = SCREEN_HEIGHT / 10;
    padding = SCREEN_HEIGHT / 9;

    titleFontSize  = SCREEN_WIDTH / 20;
    buttonFontSize = SCREEN_WIDTH / 40;

    int dy = padding + height;
    int y  = (SCREEN_HEIGHT - dy * (TOTAL_GAME_MODES - 1) + padding) / 2;
    for (int i = 0; i < TOTAL_GAME_MODES; i++) {
        strcpy(BUTTONS[i].text, GAME_MODES[i]);
        BUTTONS[i].pos.y = y;
        BUTTONS[i].pos.x = (SCREEN_WIDTH - width) / 2.0f;
        y += dy;
    }

    int yMaps = (SCREEN_HEIGHT - dy * (TOTAL_MAPS - 1) + padding) / 2;
    for (int i = 0; i < TOTAL_MAPS; i++) {
        strcpy(MAPS_BUTTONS[i].text, MAPS[i].name);
        if (i == SELECTED_MAP_IDX)
            MAPS_BUTTONS[i].selected = true;
        MAPS_BUTTONS[i].pos.y = yMaps;
        MAPS_BUTTONS[i].pos.x = SCREEN_WIDTH / 4.0f - width / 2.0f;
        yMaps += dy;
    }

    textBox.x = (SCREEN_WIDTH - MeasureText(GAME_NAME, titleFontSize)) / 2.0f;
    textBox.y = (SCREEN_HEIGHT - dy * TOTAL_GAME_MODES) / 2.0f;

    Image img = LoadImage(BACKGROUND_PATH);
    ImageResize(&img, SCREEN_WIDTH, SCREEN_HEIGHT); // redimensiona a imagem
    background = LoadTextureFromImage(img);
    UnloadImage(img);

    clickSound = LoadSound(CLICK_BUTTON_SOUND_PATH);
    music      = LoadMusicStream(MENU_MUSIC_PATH);

    PlayMusicStream(music);
}

void Menu_cleanup() {
    UnloadSound(clickSound);
    UnloadMusicStream(music);
    UnloadTexture(background);
}

static void drawButton(Button btn) {
    Rectangle rect      = (Rectangle) {btn.pos.x, btn.pos.y, width, height};
    Color     baseColor = btn.hovered ? GOLD : WHITE;
    if (btn.selected)
        baseColor = RED;
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

static bool pressedButton(Button *btn, Vector2 mousePos, bool canSelect) {
    btn->hovered =
        CheckCollisionPointRec(mousePos, (Rectangle) {btn->pos.x, btn->pos.y, width, height});

    if (btn->hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        btn->selected = canSelect && true;
        PlaySound(clickSound);
        btn->action();
        return true;
    }

    return false;
}

void Menu_update() {
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_GAME_MODES; i++)
        pressedButton(BUTTONS + i, mouse, false);

    for (int i = 0; i < TOTAL_MAPS; i++) {
        if (pressedButton(MAPS_BUTTONS + i, mouse, true)) {
            for (int j = 0; j < TOTAL_MAPS; j++) {
                if (j != i)
                    MAPS_BUTTONS[j].selected = false;
            }
        }
    }

    SetMusicVolume(music, MENU_MUSIC_VOLUME); // Se precisar abaixar o som da música
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
    DrawText(GAME_NAME, textBox.x + 2, textBox.y + 2, titleFontSize, WHITE); // sombra
    DrawText(GAME_NAME, textBox.x, textBox.y, titleFontSize, pulseColor);

    // 4. Botões
    for (int i = 0; i < TOTAL_GAME_MODES; i++) {
        drawButton(BUTTONS[i]);
    }

    for (int i = 0; i < TOTAL_MAPS; i++) {
        drawButton(MAPS_BUTTONS[i]);
    }
}
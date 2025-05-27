#include "menu.h"
#include "common.h"
#include "controller.h"
#include "game.h"
#include "raylib.h"

#define MAX_BUTTONS 10

MenuStep   menu_step = CHOOSE_MODE;
Mode       mode      = SINGLEPLAYER;
CurrentMap map       = INTERLAGOS;

Button mode_buttons[2]     = {(Button) {"1 JOGADOR", {0}, 0}, (Button) {"2 JOGADORES", {0}, 0}};
int    mode_buttons_length = 2;
int    mode_buttons_i      = 0;

Button map_buttons[2]     = {(Button) {"INTERLAGOS", {0}, 0}, (Button) {"MONACO", {0}, 0}};
int    map_buttons_length = 2;
int    map_buttons_i      = 0;

double last  = 0;
double last2 = 0;

// --- Variáveis internas ---

static Sound clickSound;
static Music music;

static Texture2D background;

static int width;
static int height;
static int padding;
static int margin;

static int buttonFontSize;

// --- Funções internas ---

static void setupButtons();
static void loadMenuAssets();
static void drawButton(Button btn);
static void unselectButtons(Button arr[], int length);

//----------------------------------------------------------------------------------
// Carregar o menu
//----------------------------------------------------------------------------------

void Menu_setup(void (*play)()) {
    width          = SCREEN_WIDTH / 5;
    height         = SCREEN_HEIGHT / 10;
    padding        = SCREEN_HEIGHT / 9;
    buttonFontSize = SCREEN_WIDTH / 42;
    margin         = SCREEN_WIDTH / 60;

    setupButtons();
    loadMenuAssets();
}

void Menu_cleanup() {
    UnloadSound(clickSound);
    UnloadMusicStream(music);
    UnloadTexture(background);
}

//----------------------------------------------------------------------------------
// Atualizar iteração do usuário com o menu
//----------------------------------------------------------------------------------

bool checkSelected(Button *btn, Vector2 mouse, ControllerInput input, int length, int *i, int j) {
    if (CheckCollisionPointRec(mouse, (Rectangle) {btn->pos.x, btn->pos.y, width, height})) {
        *i = j;
    } else if (btn->selected && (input.down || input.up)) {
        if (GetTime() - last > 0.150f) {
            *i   = (*i + (input.down ? 1 : *i == 0 ? length - 1 : -1)) % length;
            last = GetTime();
        }
    }
}

void Menu_update() {
    Vector2 mouse = GetMousePosition();
    SDL_GameControllerUpdate();

    ControllerInput input = Controller_input(controllers_n > 0 ? controllers[0] : NULL);

    if (IsKeyPressed(KEY_Q) || input.b) {
        menu_step = menu_step == 0 ? 0 : menu_step - 1;
    }

    switch (menu_step) {
    case CHOOSE_MODE:
        for (int i = 0; i < mode_buttons_length; i++) {
            Button *btn = &mode_buttons[i];

            checkSelected(btn, mouse, input, mode_buttons_length, &mode_buttons_i, i);
            btn->selected = mode_buttons_i == i;

            if ((btn->selected && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || input.a))) {
                PlaySound(clickSound);

                mode_buttons_i           = i;
                mode_buttons[i].selected = true;

                state.mode = mode + i;
                menu_step  = CHOOSE_MAP;
                last2      = GetTime();
                break;
            }
        }
        break;
    case CHOOSE_MAP:
        for (int i = 0; i < map_buttons_length; i++) {
            Button *btn = &map_buttons[i];

            checkSelected(btn, mouse, input, map_buttons_length, &map_buttons_i, i);
            btn->selected = map_buttons_i == i;

            if ((btn->selected && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                                   (input.a && GetTime() - last2 > 0.150f)))) {
                PlaySound(clickSound);

                map_buttons_i           = i;
                map_buttons[i].selected = true;

                state.map = map + i;
                Game_load();
                break;
            }
        }
        break;
    }

    SetMusicVolume(music, MENU_MUSIC_VOLUME);
    UpdateMusicStream(music);
}

//----------------------------------------------------------------------------------
// Desenhar o menu
//----------------------------------------------------------------------------------

void Menu_draw() {
    DrawTexture(background, 0, 0, WHITE);

    switch (menu_step) {
    case CHOOSE_MODE:
        for (int i = 0; i < mode_buttons_length; i++) {
            drawButton(mode_buttons[i]);
        }
        break;
    case CHOOSE_MAP:
        for (int i = 0; i < map_buttons_length; i++) {
            drawButton(map_buttons[i]);
        }
        break;
    }
}

//----------------------------------------------------------------------------------
// Inicialização dos botões
//----------------------------------------------------------------------------------

static void setupButtons() {
    int dy = padding + height;
    int y  = (SCREEN_HEIGHT - dy * (mode_buttons_length - 1) + padding) / 2;

    for (int i = 0; i < mode_buttons_length; i++) {
        mode_buttons[i].pos = (Vector2) {(SCREEN_WIDTH - width) / 2.0f, y};
        y += dy;
    }

    y = (SCREEN_HEIGHT - dy * (mode_buttons_length - 1) + padding) / 2;

    for (int i = 0; i < map_buttons_length; i++) {
        map_buttons[i].pos = (Vector2) {(SCREEN_WIDTH - width) / 2.0f, y};
        y += dy;
    }
}

//----------------------------------------------------------------------------------
// Carregar img, audio do menu
//----------------------------------------------------------------------------------

static void loadMenuAssets() {
    Image img = LoadImage(BACKGROUND_PATH);
    ImageResize(&img, SCREEN_WIDTH, SCREEN_HEIGHT);
    background = LoadTextureFromImage(img);
    UnloadImage(img);

    clickSound = LoadSound(CLICK_BUTTON_SOUND_PATH);
    music      = LoadMusicStream(MENU_MUSIC_PATH);
    PlayMusicStream(music);
}

//----------------------------------------------------------------------------------
// Desenhar botões
//----------------------------------------------------------------------------------

static void drawButton(Button btn) {
    Rectangle rect      = (Rectangle) {btn.pos.x, btn.pos.y, width, height};
    Color     baseColor = btn.selected ? RED : (Color) {215, 215, 215, 255};

    const int shadowOffset = 6;
    Rectangle shadowRect =
        (Rectangle) {rect.x + shadowOffset, rect.y + shadowOffset, rect.width, rect.height};

    float     scale  = 1.05f;
    Rectangle scaled = {rect.x - rect.width * (scale - 1) / 2,
                        rect.y - rect.height * (scale - 1) / 2, rect.width * scale,
                        rect.height * scale};

    DrawRectangleRounded(shadowRect, 0.3f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(scaled, 0.3f, 10, baseColor);

    DrawTextEx(
        FONTS[1], btn.text,
        (Vector2) {rect.x +
                       (rect.width - MeasureTextEx(FONTS[1], btn.text, buttonFontSize, 1.0f).x) / 2,
                   rect.y + (rect.height - buttonFontSize) / 2},
        buttonFontSize, 1.0f, BLACK);
}

//----------------------------------------------------------------------------------
// Atualizar estados dos botões
//----------------------------------------------------------------------------------

static void unselectButtons(Button arr[], int length) {
    for (int i = 0; i < length; i++) {
        arr[i].selected = false;
    }
}

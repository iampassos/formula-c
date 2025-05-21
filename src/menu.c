#include "menu.h"
#include "common.h"
#include "raylib.h"
#include <string.h>

#define MAX_BUTTONS 10

// --- Variáveis internas ---

static Button BUTTONS[MAX_BUTTONS];
static Button MAPS_BUTTONS[MAX_BUTTONS];
static Button debugButton;
static Button playButton;

static Sound clickSound;
static Music music;

static Texture2D background;

static int width;
static int height;
static int padding;
static int margin;

static int buttonFontSize;

// --- Funções internas ---

static void setupGameModeButtons();
static void setupMapButtons();
static void setupMainButtons(void (*play)());
static void loadMenuAssets();

static void drawButton(Button btn);

static bool pressedButton(Button *btn, Vector2 mousePos);
static void unselectButtons(Button arr[]);

//----------------------------------------------------------------------------------
// Ações dos botões
//----------------------------------------------------------------------------------

static void interlagosMapButtonAction() {
    state.map = INTERLAGOS;
}

static void debugButtonAction() {
    state.debug          = !state.debug;
    debugButton.selected = state.debug;
}

static void singleplayerButtonAction() {
    state.mode = SINGLEPLAYER;
}

static void splitscreenButtonAction() {
    state.mode = SPLITSCREEN;
}

//----------------------------------------------------------------------------------
// Carregar o menu
//----------------------------------------------------------------------------------

void Menu_setup(void (*play)()) {
    width          = SCREEN_WIDTH / 5;
    height         = SCREEN_HEIGHT / 10;
    padding        = SCREEN_HEIGHT / 9;
    buttonFontSize = SCREEN_WIDTH / 40;
    margin         = SCREEN_WIDTH / 60;

    setupGameModeButtons();
    setupMapButtons();
    setupMainButtons(play);
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

void Menu_update() {
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < TOTAL_GAME_MODES; i++) {
        if (pressedButton(BUTTONS + i, mouse)) {
            unselectButtons(BUTTONS);
            BUTTONS[i].selected = true;
            break;
        }
    }

    for (int i = 0; i < TOTAL_MAPS; i++) {
        if (pressedButton(MAPS_BUTTONS + i, mouse)) {
            unselectButtons(MAPS_BUTTONS);
            MAPS_BUTTONS[i].selected = true;
            break;
        }
    }

    pressedButton(&debugButton, mouse);
    pressedButton(&playButton, mouse);

    SetMusicVolume(music, MENU_MUSIC_VOLUME);
    UpdateMusicStream(music);
}

//----------------------------------------------------------------------------------
// Desenhar o menu
//----------------------------------------------------------------------------------

void Menu_draw() {
    DrawTexture(background, 0, 0, WHITE);

    for (int i = 0; i < TOTAL_GAME_MODES; i++) {
        drawButton(BUTTONS[i]);
    }

    for (int i = 0; i < TOTAL_MAPS; i++) {
        drawButton(MAPS_BUTTONS[i]);
    }

    drawButton(debugButton);
    drawButton(playButton);
}

//----------------------------------------------------------------------------------
// Inicialização dos botões
//----------------------------------------------------------------------------------

static void setupGameModeButtons() {
    int dy = padding + height;
    int y  = (SCREEN_HEIGHT - dy * (TOTAL_GAME_MODES - 1) + padding) / 2;

    for (int i = 0; i < TOTAL_GAME_MODES; i++) {
        strcpy(BUTTONS[i].text, GAME_MODES[i]);
        BUTTONS[i].pos      = (Vector2) {(SCREEN_WIDTH - width) / 2.0f, y};
        BUTTONS[i].selected = (i == state.mode);
        y += dy;
    }

    BUTTONS[0].action = singleplayerButtonAction;
    BUTTONS[1].action = splitscreenButtonAction;
}

static void setupMapButtons() {
    int dy = padding + height;
    int y  = (SCREEN_HEIGHT - dy * (TOTAL_MAPS - 1) + padding) / 2;

    for (int i = 0; i < TOTAL_MAPS; i++) {
        strcpy(MAPS_BUTTONS[i].text, MAPS[i].name);
        MAPS_BUTTONS[i].pos      = (Vector2) {SCREEN_WIDTH / 4.0f - width / 2.0f, y};
        MAPS_BUTTONS[i].selected = (i == state.map);
        y += dy;
    }

    MAPS_BUTTONS[0].action = interlagosMapButtonAction;
}

static void setupMainButtons(void (*play)()) {
    playButton = (Button) {
        "Play", {SCREEN_WIDTH - width - margin, SCREEN_HEIGHT - height - margin}, 0, 0, play};

    debugButton =
        (Button) {"Debug",
                  {SCREEN_WIDTH - width - margin, SCREEN_HEIGHT - 2 * height - 2 * margin},
                  0,
                  state.debug,
                  debugButtonAction};
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
    Color     baseColor = btn.hovered ? GOLD : (Color) {215, 215, 215, 255};
    if (btn.selected)
        baseColor = RED;
    Color     textColor    = BLACK;
    const int shadowOffset = 6;
    Rectangle shadowRect =
        (Rectangle) {rect.x + shadowOffset, rect.y + shadowOffset, rect.width, rect.height};

    float     scale  = btn.hovered ? 1.05f : 1.0f;
    Rectangle scaled = {rect.x - rect.width * (scale - 1) / 2,
                        rect.y - rect.height * (scale - 1) / 2, rect.width * scale,
                        rect.height * scale};

    DrawRectangleRounded(shadowRect, 0.3f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(scaled, 0.3f, 10, baseColor);

    DrawText(btn.text, rect.x + (rect.width - MeasureText(btn.text, buttonFontSize)) / 2,
             rect.y + (rect.height - buttonFontSize) / 2, buttonFontSize, textColor);
}

//----------------------------------------------------------------------------------
// Atualizar estados dos botões
//----------------------------------------------------------------------------------

static bool pressedButton(Button *btn, Vector2 mousePos) {
    btn->hovered =
        CheckCollisionPointRec(mousePos, (Rectangle) {btn->pos.x, btn->pos.y, width, height});

    if (btn->hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PlaySound(clickSound);
        btn->action();
        return true;
    }

    return false;
}

static void unselectButtons(Button arr[]) {
    for (int i = 0; i < MAX_BUTTONS; i++) {
        arr[i].selected = false;
    }
}

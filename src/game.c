#include "game.h"
#include "raylib.h"
#include <stdio.h>

// --- Variáveis públicas ---

Texture2D   trackBackground;
Texture2D   trackHud;
LinkedList *cars;

Camera2D *camera1;
Camera2D *camera2;

char textBuffer[1000];

int minimapWidth;
int minimapHeigth;

Vector2 minimapPos;

// --- Funções internas ---

static void drawHud();
static void loadMap(Map map);
static void mapCleanup();

//----------------------------------------------------------------------------------
// Carregamento do jogo
//----------------------------------------------------------------------------------

void Game_setup() {
    cars = LinkedList_create();
}

void Game_load() {
    state.screen = GAME;
    Map map      = MAPS[state.map];
    loadMap(map);
    switch (state.mode) {
    case SINGLEPLAYER:
        loadSingleplayer(map);
        break;
    case SPLITSCREEN:
        loadSplitscreen(map);
        break;
    }
}

void Game_cleanup() {
    if (state.screen != GAME) {
        mapCleanup();
    }
    LinkedList_free(cars);
}

//----------------------------------------------------------------------------------
// Atualização dos componentes do jogo (carros, camera...)
//----------------------------------------------------------------------------------

void Game_update() {
    if (IsKeyDown(KEY_Q) || (winner && GetTime() - winner->startLapTime > 3)) {
        state.screen = MENU;
        mapCleanup();
        return;
    }

    switch (state.mode) {
    case SINGLEPLAYER:
        updateSingleplayer();
        break;
    case SPLITSCREEN:
        updateSplitscreen();
        break;
    }
}

//----------------------------------------------------------------------------------
// Desenhando a tela do jogo
//----------------------------------------------------------------------------------

void Game_draw() {
    if (state.screen != GAME)
        return;

    switch (state.mode) {
    case SINGLEPLAYER:
        drawSingleplayer();
        break;
    case SPLITSCREEN:
        drawSplitscreen();
        break;
    }

    if (state.status == STARTED) {
        drawHud();
    }
}

//----------------------------------------------------------------------------------
// Carregamento e limpeza do mapa
//----------------------------------------------------------------------------------

static void loadMap(Map map) {
    state.raceTime  = GetTime();
    trackBackground = LoadTexture(state.debug ? map.maskPath : map.backgroundPath);

    Image minimap = LoadImage(state.debug ? map.maskPath : map.minimapPath);
    minimapWidth  = SCREEN_WIDTH / 4;
    minimapHeigth = SCREEN_HEIGHT / 4;
    ImageResize(&minimap, minimapWidth, minimapHeigth);
    trackHud = LoadTextureFromImage(minimap);
    UnloadImage(minimap);

    Track_setMask(map.maskPath);
    Track_setCheckpoints(map.checkpoints, map.checkpointSize);

    Camera_Background_setSize(trackBackground.width, trackBackground.height);
}

static void mapCleanup() {
    Track_Unload();
    LinkedList_clear(cars);
    UnloadTexture(trackBackground);
    UnloadTexture(trackHud);
    if (state.mode == SINGLEPLAYER) {
        cleanUpSingleplayer();
    } else {
        cleanUpSplitscreen();
    }
    Camera_free(camera1);
}

//----------------------------------------------------------------------------------
// Draw map
//----------------------------------------------------------------------------------

void drawMap() {
    DrawTexture(trackBackground, 0, 0, WHITE);
    LinkedList_forEach(cars, Car_draw);
}

//----------------------------------------------------------------------------------
// Draw hud
//----------------------------------------------------------------------------------

void drawHud() {
    switch (state.mode) {
    case SINGLEPLAYER:
        drawHudSingleplayer();
        break;
    case SPLITSCREEN:
        drawHudSplitscreen();
        break;
    }

    DrawTexture(trackHud, minimapPos.x, minimapPos.y, (Color) {255, 255, 255, HUD_OPACITY});
    LinkedList_forEach(cars, drawPlayerInMinimap);
}

//----------------------------------------------------------------------------------
// Funções auxiliares para desenhar a hud
//----------------------------------------------------------------------------------

void drawPlayerHud(Car *player, int x) {
    drawSpeedometer(player, x + 128, SCREEN_HEIGHT - 2 * 64);
    drawLaps(player, x + 32, 32);
    drawLapTime(player, x + 32, 96);
}

void drawTextWithShadow(char *text, float x, float y, int size, Color color) {
    DrawText(text, x + 1, y + 1, size, BLACK);
    DrawText(text, x, y, size, color);
}

void drawPlayerInMinimap(Car *player) {
    float x = trackHud.width * player->pos.x / trackBackground.width + minimapPos.x;
    float y = trackHud.height * player->pos.y / trackBackground.height + minimapPos.y;

    if (player->ghost) {
        DrawCircleLines(x, y, 3.5f, BLACK);
        DrawCircle(x, y, 3, WHITE);
    } else {
        DrawCircleLines(x, y, 6.5f, BLACK);
        DrawCircle(x, y, 6, player->color);
    }
}

void drawSpeedometer(Car *player, float x, float y) {
    snprintf(textBuffer, sizeof(textBuffer), "%.1f km/h",
             3600 * 0.75f * player->vel * 60 / trackBackground.width);
    Color textColor = ColorLerp(WHITE, RED, player->vel / player->maxVelocity);
    drawTextWithShadow(textBuffer, x, y, 64, textColor);
}

void drawLaps(Car *player, float x, float y) {
    if (player->lap > -1) {
        if (state.mode == SINGLEPLAYER) {
            snprintf(textBuffer, sizeof(textBuffer), "Volta %d", player->lap + 1);
        } else if (player->lap < MAX_LAPS) {
            snprintf(textBuffer, sizeof(textBuffer), "Volta %d/%d", player->lap + 1, MAX_LAPS);
        }
        drawTextWithShadow(textBuffer, x, y, 64, WHITE);
    }
}

void drawLapTime(Car *player, float x, float y) {
    if (player->lap > -1) {
        double time = GetTime() - player->startLapTime;
        int    mins = time / 60;
        float  secs = time - (mins * 60);
        snprintf(textBuffer, sizeof(textBuffer), "%d:%05.2fs", mins, secs);
        drawTextWithShadow(textBuffer, x, y, 48, WHITE);
    }
}

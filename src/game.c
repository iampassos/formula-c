#include "game.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

// --- Variáveis públicas ---
Texture2D   SPEEDOMETER;
Texture2D   trackBackground;
Texture2D   trackHud;
LinkedList *cars;

Camera2D *camera1;
Camera2D *camera2;

char textBuffer[1000];

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
    if (IsKeyDown(KEY_Q) || (winner && GetTime() - winner->startLapTime > 3.5f)) {
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

    Image temp = LoadImage(SPEEDOMETER_PATH);
    ImageResize(&temp, SCREEN_WIDTH / 6, SCREEN_WIDTH / 6);
    SPEEDOMETER = LoadTextureFromImage(temp);
    UnloadImage(temp);

    temp = LoadImage(state.debug ? map.maskPath : map.minimapPath);
    ImageResize(&temp, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
    trackHud = LoadTextureFromImage(temp);
    UnloadImage(temp);

    Track_setMask(map.maskPath);
    Track_setCheckpoints(map.checkpoints, map.checkpointSize);

    Camera_Background_setSize(trackBackground.width, trackBackground.height);
}

static void mapCleanup() {
    Track_Unload();
    LinkedList_clear(cars);
    UnloadTexture(trackBackground);
    UnloadTexture(SPEEDOMETER);
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
    drawLaps(player, x + 32, 32);
    drawLapTime(player, x + 32, 96);
    drawSpeedometer(player, x + 192, SCREEN_HEIGHT - 192);

    if (state.debug) {
        drawPlayerDebug(player, x + 32, 300);
    }
}

void drawTextWithShadow(char *text, float x, float y, int size, Color color, Font font) {
    DrawTextEx(font, text, (Vector2) {x + 1, y + 1}, size, 1.0f, BLACK);
    DrawTextEx(font, text, (Vector2) {x, y}, size, 1.0f, color);
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
    DrawTexture(SPEEDOMETER, x - SPEEDOMETER.width / 2, y - SPEEDOMETER.height / 2,
                (Color) {255, 255, 255, HUD_OPACITY});

    float   angle  = (fabs(player->vel) / player->maxVelocity) * (PI * 1.5) + (PI * 0.75);
    Vector2 endPos = {cosf(angle) * 100 + x, sinf(angle) * 100 + y};

    DrawLineEx((Vector2) {x, y}, endPos, 8, RED);
    DrawCircle(x, y, 3, RED);

    snprintf(textBuffer, sizeof(textBuffer), "%.0f",
             3600 * 0.75f * fabs(player->vel) * 60 / trackBackground.width);
    Color textColor = ColorLerp(GREEN, RED, player->vel / player->maxVelocity);

    drawTextWithShadow(textBuffer, x - MeasureTextEx(FONTS[1], textBuffer, 48, 1.0f).x / 2, y + 24,
                       48, textColor, FONTS[1]);

    drawTextWithShadow("KM/H", x - MeasureTextEx(FONTS[0], "KM/H", 16, 1.0f).x / 2, y + 72, 16,
                       WHITE, FONTS[0]);
}

void drawLaps(Car *player, float x, float y) {
    if (player->lap > -1) {
        if (state.mode == SINGLEPLAYER) {
            snprintf(textBuffer, sizeof(textBuffer), "Volta %d", player->lap + 1);
        } else if (player->lap < MAX_LAPS) {
            snprintf(textBuffer, sizeof(textBuffer), "Volta %d/%d", player->lap + 1, MAX_LAPS);
        }
        drawTextWithShadow(textBuffer, x, y, 64, WHITE, FONTS[0]);
    }
}

void drawLapTime(Car *player, float x, float y) {
    if (player->lap > -1) {
        double time = GetTime() - player->startLapTime;
        int    mins = time / 60;
        float  secs = time - (mins * 60);
        snprintf(textBuffer, sizeof(textBuffer), "%d:%05.2fs", mins, secs);
        drawTextWithShadow(textBuffer, x, y, 48, WHITE, FONTS[0]);
    }
}

//----------------------------------------------------------------------------------
// Debug
//----------------------------------------------------------------------------------

void drawPlayerDebug(Car *player, int x, int y) {
    char textBuffer[1000];
    snprintf(textBuffer, sizeof(textBuffer),
             "Current car debug\n"
             "ID: %d\n"
             "Lap: %d\n"
             "Start Lap Time: %.2f\n"
             "Current Lap Time: %.2f\n"
             "Best Lap Time: %.2f\n"
             "Checkpoint: %d\n"
             "Position: (%.1f, %.1f)\n"
             "Velocity: %.2f\n"
             "Max velocity: %.2f\n"
             "Acceleration: %.2f\n"
             "Size: %dx%d\n"
             "Angle: %.2f\n"
             "Angular Speed: %.2f\n"
             "Min Turn Speed: %.2f\n"
             "Brake Force: %.2f\n"
             "Drag Force: %.2f\n"
             "Reverse Force: %.2f",
             player->id, player->lap, player->startLapTime, GetTime() - player->startLapTime,
             player->bestLapTime, player->checkpoint, player->pos.x, player->pos.y, player->vel,
             player->maxVelocity, player->acc, player->width, player->height, player->angle,
             player->angularSpeed, player->minTurnSpeed, player->breakForce, player->dragForce,
             player->reverseForce);

    Vector2 size = MeasureTextEx(FONTS[0], textBuffer, 20, 1.0f);
    DrawRectangle(x, y, size.x + 10, size.y + 10, (Color) {196, 196, 196, 200});
    DrawTextEx(FONTS[0], textBuffer, (Vector2) {x, y}, 20, 1.0f, BLACK);
}

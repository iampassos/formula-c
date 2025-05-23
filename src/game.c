#include "game.h"
#include "raylib.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Variáveis públicas ---
Texture2D   trackHud;
LinkedList *cars;

Camera2D *camera1;
Camera2D *camera2;

int hudPlayerListWidth = 330;

Vector2 minimapPos;

// --- Variáveis internas ---

static Texture2D  trackBackground;
static Texture2D  SPEEDOMETER;
static Texture2D  logoNoBg;
static ArrayList *referenceLap = NULL;
static double     last         = 0;

// --- Funções internas ---

static void  drawHud();
static void  loadMap(Map map);
static void  mapCleanup();
static void  updateCarRanking();
static void  updateCarReference(Car *car);
static float cmp(Car *a, Car *b);

//----------------------------------------------------------------------------------
// Carregamento do jogo
//----------------------------------------------------------------------------------

void Game_setup() {
    cars         = LinkedList_create();
    referenceLap = ArrayList_create();
    Image temp   = LoadImage(LOGO_BG_IMAGE_PATH);
    ImageResize(&temp, 128, 128);
    logoNoBg = LoadTextureFromImage(temp);
    UnloadImage(temp);
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
    UnloadTexture(logoNoBg);
    ArrayList_free(referenceLap);
    LinkedList_free(cars);
}

//----------------------------------------------------------------------------------
// Atualização dos componentes do jogo (carros, camera...)
//----------------------------------------------------------------------------------

void Game_update() {
    if (IsKeyDown(KEY_Q) || state.status == ENDED) {
        state.screen = MENU;
        mapCleanup();
        return;
    }

    updateCarRanking();

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
// Funções complementares para o update
//----------------------------------------------------------------------------------

static void updateCarRanking() {
    double actualTime = GetTime();
    if (ArrayList_length(referenceLap) > 0 && actualTime - last > 0.5f) {
        LinkedList_forEach(cars, updateCarReference);
        LinkedList_sort(cars, cmp);
        last = actualTime;
    }
}

static void updateCarReference(Car *car) {
    if (car->lap == -1 && car->ghost == 0) {
        car->refFrame = 0;
        return;
    }

    float lowest = FLT_MAX;
    int   low_i  = 0;

    for (int i = 0; i < ArrayList_length(referenceLap); i++) {
        float dist = Vector2_dist(ArrayList_get(referenceLap, i).pos, car->pos);

        if (lowest > dist) {
            lowest = dist;
            low_i  = i;
        }
    }

    car->refFrame =
        state.mode == SINGLEPLAYER ? low_i : ArrayList_length(referenceLap) * car->lap + low_i;
}

static float cmp(Car *a, Car *b) {
    return b->refFrame - a->refFrame;
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
    state.maxLaps   = map.maxLaps;
    state.raceTime  = GetTime();
    trackBackground = LoadTexture(state.debug ? map.maskPath : map.backgroundPath);
    MAP_WIDTH = trackBackground.width;
    MAP_HEIGHT = trackBackground.height;

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

    char referencePath[200];
    strcpy(referencePath, REFERENCE_DATA_PATH);
    strcat(referencePath, map.name);
    strcat(referencePath, "_reference.bin");

    ArrayList_clear(referenceLap);
    FILE *file = fopen(referencePath, "rb");
    if (file != NULL) {
        CarFrame buffer;
        while (fread(&buffer, sizeof(CarFrame), 1, file) == 1) {
            ArrayList_push(referenceLap, buffer);
        }
        fclose(file);
    }
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
    drawSpeedometer(player, x + 192, SCREEN_HEIGHT - 192);

    drawGameLogo(x + 32, 32);

    DrawRectangle(x + 32, 166, hudPlayerListWidth, 48, (Color) {51, 51, 51, 255});
    drawLaps(player, x + 32, 170);
    drawLapTime(player, x + 32, 174);

    drawPlayerList(player, x + 32, 220);

    if (state.debug) {
        drawPlayerDebug(player, x + 32, 300);
    }
}

void drawGameLogo(float x, float y) {
    DrawRectangle(x, y, hudPlayerListWidth, 128, (Color) {51, 51, 51, 255});
    DrawTexture(logoNoBg, x + hudPlayerListWidth / 2 - 64, y, WHITE);
}

void drawLapTime(Car *player, float x, float y) {
    Color color = WHITE;

    if (flagBestLap) {
        stringifyTime(strBuffer, bestLapTime, 0);
        color = PURPLE;
    } else {
        stringifyTime(strBuffer, player->lap == -1 ? 0 : GetTime() - player->startLapTime, 0);

        if (state.mode == SINGLEPLAYER) {
            Car *ghost = LinkedList_getCarById(cars, 99);
            color      = ghost->refFrame - player->refFrame > 0 ? RED : GREEN;
        }
    }

    drawCenteredText(strBuffer, x + hudPlayerListWidth / 2.0f + 4, y, hudPlayerListWidth / 2.0f, 12,
                     24, color, FONTS[0]);
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

    snprintf(strBuffer, sizeof(strBuffer), "%.0f",
             3600 * 0.75f * fabs(player->vel) * 60 / trackBackground.width);
    Color textColor = ColorLerp(GREEN, RED, player->vel / player->maxVelocity);

    drawTextWithShadow(strBuffer, x - MeasureTextEx(FONTS[1], strBuffer, 48, 1.0f).x / 2, y + 24,
                       48, textColor, FONTS[1]);

    drawTextWithShadow("KM/H", x - MeasureTextEx(FONTS[0], "KM/H", 16, 1.0f).x / 2, y + 72, 16,
                       WHITE, FONTS[0]);
}

void drawLaps(Car *player, float x, float y) {
    if (state.mode == SINGLEPLAYER) {
        snprintf(strBuffer, sizeof(strBuffer), "Volta %d", player->lap + 1);
    } else if (player->lap < state.maxLaps) {
        snprintf(strBuffer, sizeof(strBuffer), "Volta %d/%d", player->lap + 1, state.maxLaps);
    }

    drawCenteredText(strBuffer, x + 4, y, hudPlayerListWidth / 2.0f, 14, 28, WHITE, FONTS[0]);
}

void drawPlayerList(Car *player, float x, float y) {
    stringifyTime(strBuffer, 599.999f, 1);
    Vector2 referenceSize = MeasureTextEx(FONTS[0], strBuffer, 20, 1.0f);

    int   idx  = 0;
    Node *prev = cars->head;
    Node *curr = cars->head;
    while (curr != NULL) {
        DrawRectangle(x, y + 32 * idx, hudPlayerListWidth, 32, (Color) {51, 51, 51, 255});

        int     isOwn = curr->car->id == player->id;
        Vector2 size  = MeasureTextEx(FONTS[isOwn], curr->car->name, 20, 1.0f);
        float   yx    = y + (idx * size.y);

        snprintf(strBuffer, sizeof(strBuffer), "%d", idx + 1);
        drawCenteredText(strBuffer, x, yx, 36, 20, 20, WHITE, FONTS[isOwn]);

        DrawRectangle(x + 36, yx + 12.5f, 2, 15, curr->car->color);

        DrawTextEx(FONTS[isOwn], curr->car->name, (Vector2) {x + 44, yx + 10}, 20, 1.0f, WHITE);

        if (curr == cars->head || (curr->car->ghost && curr->car->pos.x == -1000)) {
            strcpy(strBuffer, "-:--.---");
        } else {
            stringifyTime(strBuffer, (prev->car->refFrame - curr->car->refFrame) / 60.0f, 1);
        }

        drawCenteredText(strBuffer, x + hudPlayerListWidth - referenceSize.x, yx, referenceSize.x,
                         20, 20, WHITE, FONTS[isOwn]);

        prev = curr;
        curr = curr->next;
        idx++;
    }
}

//----------------------------------------------------------------------------------
// Debug
//----------------------------------------------------------------------------------

void drawPlayerDebug(Car *player, int x, int y) {
    char strBuffer[1000];
    snprintf(strBuffer, sizeof(strBuffer),
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
             "Reverse Force: %.2f\n"
             "Reference Frame i: %d",
             player->id, player->lap, player->startLapTime, GetTime() - player->startLapTime,
             player->bestLapTime, player->checkpoint, player->pos.x, player->pos.y, player->vel,
             player->maxVelocity, player->acc, player->width, player->height, fmod(player->angle, 2*PI),
             player->angularSpeed, player->minTurnSpeed, player->breakForce, player->dragForce,
             player->reverseForce, player->refFrame);

    Vector2 size = MeasureTextEx(FONTS[0], strBuffer, 20, 1.0f);
    DrawRectangle(x, y, size.x + 10, size.y + 10, (Color) {196, 196, 196, 200});
    DrawTextEx(FONTS[0], strBuffer, (Vector2) {x, y}, 20, 1.0f, BLACK);
}

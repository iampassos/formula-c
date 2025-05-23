#include "game.h"
#include "common.h"
#include "linked_list.h"
#include "raylib.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

// --- Variáveis públicas ---
Texture2D   trackHud;
LinkedList *cars;

Camera2D *camera1;
Camera2D *camera2;

int hudPlayerListWidth = 330;

Vector2 minimapPos;

Car   *bestLapTimePlayer = NULL;
double bestLapTime       = 0;
double bestLapLastTick   = 0;

// --- Variáveis internas ---

static Texture2D  trackBackground;
static Texture2D  SPEEDOMETER;
static Texture2D  logoNoBg;
static ArrayList *referenceLap = NULL;
static double     last         = 0;

static double msgStart;
static int    msgActive;
static int    msgCount;

// --- Funções internas ---

static void drawHud();
static void loadMap(Map map);
static void mapCleanup();
static void updateBestLap();
static void updateCarRanking();
static void updateCarReference(Car *car);

//----------------------------------------------------------------------------------
// Carregamento do jogo
//----------------------------------------------------------------------------------

void Game_setup() {
    cars         = LinkedList_create();
    referenceLap = ArrayList_create();
    Image temp   = LoadImage(LOGO_BG_IMAGE_PATH);
    ImageResize(&temp, 256, 256);
    logoNoBg = LoadTextureFromImage(temp);
    UnloadImage(temp);
}

void Game_load() {
    state.screen = GAME;
    Map map      = MAPS[state.map];
    loadMap(map);

    bestLapTime       = 0;
    bestLapTimePlayer = NULL;
    msgStart          = 0;
    msgActive         = 0;
    msgCount          = 0;

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

    updateBestLap();
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

static void checkBestLap(Car *player) {
    if (player->lap < 1 || !player->changeLapFlag) {
        return;
    }

    if (player->bestLapTime < bestLapTime || bestLapTime == 0) {
        bestLapTimePlayer = player;
        bestLapLastTick   = GetTime();
        bestLapTime       = player->bestLapTime;
    }
}

static void updateBestLap() {
    if (bestLapTimePlayer && GetTime() - bestLapLastTick >= 3.0f) {
        bestLapTimePlayer->changeLapFlag = false;
        bestLapTimePlayer                = NULL;
    }

    LinkedList_forEach(cars, checkBestLap);
}

static float cmp(Car *a, Car *b) {
    return b->refFrame - a->refFrame;
}

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
    MAP_WIDTH       = trackBackground.width;
    MAP_HEIGHT      = trackBackground.height;

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

void drawBestLapMessage(float x, float y) {
    if (bestLapTimePlayer) {
        float actualTime = GetTime();
        if (actualTime - msgStart >= 0.3f) {
            msgActive = !msgActive;
            msgStart  = actualTime;

            if (msgActive) {
                msgCount++;
            }

            if (msgCount > 10) {
                msgActive = 0;
                msgCount  = 0;
            }
        }

        if (msgActive) {
            Rectangle rect = {x, y, SCREEN_WIDTH, 0};

            if (state.mode == SPLITSCREEN) {
                rect.width  = SCREEN_WIDTH / 2.0f;
                rect.height = SCREEN_HEIGHT / 4.0f;
            }

            drawTextCenteredInRect("Melhor Volta", rect, 64, PURPLE, FONTS[1]);
        }
    }
}

void drawPlayerHud(Car *player, int x) {
    drawSpeedometer(player, x + 192, SCREEN_HEIGHT - 192);

    drawGameLogo(x + 32, 32);

    DrawRectangle(x + 32, 166, hudPlayerListWidth, 48, (Color) {51, 51, 51, HUD_OPACITY});
    drawLaps(player, x + 32, 166);
    drawLapTime(player, x + 32, 166);

    drawPlayerList(player, x + 32, 220);
    drawBestLapMessage(x, SCREEN_HEIGHT / 4.0f);

    if (state.debug) {
        drawPlayerDebug(player, x + 32, 300);
    }
}

void drawGameLogo(float x, float y) {
    DrawRectangle(x, y, hudPlayerListWidth, 128, (Color) {51, 51, 51, HUD_OPACITY});
    DrawTexture(logoNoBg, x + (hudPlayerListWidth - 256) / 2.0f, y - (252 - 128) / 2.0f,
                (Color) {255, 255, 255, HUD_OPACITY});
}

void drawLapTime(Car *player, float x, float y) {
    Color color = WHITE;

    if (player == bestLapTimePlayer) {
        stringifyTime(strBuffer, bestLapTimePlayer->bestLapTime, 0);
        color = PURPLE;
    } else {
        stringifyTime(strBuffer, player->lap == -1 ? 0 : GetTime() - player->startLapTime, 0);

        if (state.mode == SINGLEPLAYER) {
            Car *ghost = LinkedList_getCarById(cars, 99);
            color =
                ghost->ghostActive ? ghost->refFrame - player->refFrame > 0 ? RED : GREEN : WHITE;
        }
    }

    drawTextCenteredInRect(
        strBuffer, (Rectangle) {x + hudPlayerListWidth / 2.0f, y, hudPlayerListWidth / 2.0f, 48},
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

    drawTextCenteredInRect(strBuffer, (Rectangle) {x, y, hudPlayerListWidth / 2.0f, 48}, 24, WHITE,
                           FONTS[0]);
}

void drawPlayerList(Car *player, float x, float y) {
    stringifyTime(strBuffer, 599.999f, 1);
    Vector2 lapTimeSize = MeasureTextEx(FONTS[0], strBuffer, 20, 1.0f);

    int height  = 36;
    int padding = 9;

    int   idx  = 0;
    Node *prev = cars->head;
    Node *curr = cars->head;

    while (curr != NULL) {
        if (curr->car->ghost && !curr->car->ghostActive) {
            prev = curr;
            curr = curr->next;
            continue;
        }

        bool  isOwn = curr->car->id == player->id;
        float yx    = y + idx * height;

        Color bgColor = bestLapTimePlayer == curr->car ? PURPLE : (Color) {51, 51, 51, HUD_OPACITY};
        Rectangle rect = {x, yx, hudPlayerListWidth, height};
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, bgColor);

        float fontSize = 20;

        snprintf(strBuffer, sizeof(strBuffer), "%d", idx + 1);
        DrawTextEx(FONTS[isOwn], strBuffer, (Vector2) {x + padding, yx + (height - fontSize) / 2},
                   fontSize, 1.0f, WHITE);

        DrawRectangle(x + 36, yx + padding + 2.5f, 2, height / 2.5f, curr->car->color);

        DrawTextEx(FONTS[isOwn], curr->car->name, (Vector2) {x + 44, yx + (height - fontSize) / 2},
                   fontSize, 1.0f, WHITE);

        if (idx == 0 || (idx == 0 && curr->next == NULL)) {
            strcpy(strBuffer, "-:--.---");
        } else {
            stringifyTime(strBuffer, (prev->car->refFrame - curr->car->refFrame) / 60.0f, 1);
        }

        drawTextCenteredInRect(
            strBuffer,
            (Rectangle) {rect.x + rect.width - lapTimeSize.x, yx, lapTimeSize.x + 1, rect.height},
            fontSize, WHITE, FONTS[isOwn]);

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
             "Drag Force: %.3f\n"
             "Reverse Force: %.2f\n"
             "Reference Frame i: %d\n"
             "Game best lap: %.3f",
             player->id, player->lap, player->startLapTime, GetTime() - player->startLapTime,
             player->bestLapTime, player->checkpoint, player->pos.x, player->pos.y, player->vel,
             player->maxVelocity, player->acc, player->width, player->height,
             fmod(player->angle, 2 * PI), player->angularSpeed, player->minTurnSpeed,
             player->breakForce, player->dragForce, player->reverseForce, player->refFrame,
             bestLapTime);

    Vector2 size = MeasureTextEx(FONTS[0], strBuffer, 20, 1.0f);
    DrawRectangle(x, y, size.x + 10, size.y + 10, (Color) {196, 196, 196, 200});
    DrawTextEx(FONTS[0], strBuffer, (Vector2) {x, y}, 20, 1.0f, BLACK);
}

#include "game.h"
#include "camera.h"
#include "common.h"
#include "controller.h"
#include "linked_list.h"
#include "raylib.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

// --- Variáveis públicas ---

bool done   = false;
bool loaded = false;
char load_msg[100];

Image minimapImage;
Image trackImage;
Image debugMinimapImage;
Image debugTrackImage;
Image speedometerImage;
Image logoNoBgImage;
Image carsImage[3];

Texture2D minimapTexture;
Texture2D trackTexture;
Texture2D debugMinimapTexture;
Texture2D debugTrackTexture;
Texture2D speedometerTexture;
Texture2D logoNoBgTexture;

LinkedList *cars;

Camera2D *camera1;
Camera2D *camera2;

int hudPlayerListWidth = 330;

Vector2 minimapPos;

Car *bestLapTimePlayer;

// --- Variáveis internas ---

static Color backgroundImageColor;

static double bestLapLastTick = 0;

static ArrayList *referenceLap = NULL;
static double     last         = 0;

static double msgStart;
static int    msgActive;
static int    msgCount;

// --- Funções internas ---

static void drawHud();
static void mapCleanup();
static void updateBestLapCar(Car *player);
static void updateCarRanking();
static void updateCarReference(Car *car);

//----------------------------------------------------------------------------------
// Carregamento do jogo
//----------------------------------------------------------------------------------

void Game_setup() {
    cars = LinkedList_create();
}

void Game_load() {
    done   = false;
    loaded = false;
    strcpy(load_msg, "");

    state.screen   = GAME;
    Map *map       = malloc(sizeof(Map));
    *map           = MAPS[state.map];
    state.maxLaps  = map->maxLaps;
    state.raceTime = GetTime();

    msgStart  = 0;
    msgActive = 0;
    msgCount  = 0;

    pthread_t thread;

    switch (state.mode) {
    case SINGLEPLAYER:
        pthread_create(&thread, NULL, &loadSingleplayer, map);
        break;
    case SPLITSCREEN:
        pthread_create(&thread, NULL, &loadSplitscreen, map);
        break;
    }
}

static void loadCarTexture(Car *car) {
    car->texture = LoadTextureFromImage(car->image);
}

static void loadTextures() {
    trackTexture = LoadTextureFromImage(trackImage);
    UnloadImage(trackImage);

    debugTrackTexture = LoadTextureFromImage(debugTrackImage);
    UnloadImage(debugTrackImage);

    minimapTexture = LoadTextureFromImage(minimapImage);
    UnloadImage(minimapImage);

    minimapPos.x = SCREEN_WIDTH - minimapTexture.width;
    minimapPos.y = 10;

    debugMinimapTexture = LoadTextureFromImage(debugMinimapImage);
    UnloadImage(debugMinimapImage);

    speedometerTexture = LoadTextureFromImage(speedometerImage);
    UnloadImage(speedometerImage);

    logoNoBgTexture = LoadTextureFromImage(logoNoBgImage);
    UnloadImage(logoNoBgImage);

    LinkedList_forEach(cars, &loadCarTexture);

    done = true;
}

static void loadImages(Map *map) {
    strcpy(load_msg, "Carregando mapa...");
    trackImage = LoadImage(map->backgroundPath);

    MAP_WIDTH            = trackImage.width;
    MAP_HEIGHT           = trackImage.height;
    backgroundImageColor = map->backgroundColor;

    strcpy(load_msg, "Carregando mapa debug...");
    debugTrackImage = LoadImage(map->maskPath);

    strcpy(load_msg, "Carregando minimapa...");
    minimapImage = LoadImage(map->minimapPath);
    ImageResize(&minimapImage, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);

    strcpy(load_msg, "Carregando minimapa de debug...");
    debugMinimapImage = LoadImage(map->maskPath);
    ImageResize(&debugMinimapImage, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);

    strcpy(load_msg, "Carregando velocimetro...");
    speedometerImage = LoadImage(SPEEDOMETER_PATH);
    ImageResize(&speedometerImage, SCREEN_WIDTH / 6, SCREEN_WIDTH / 6);

    strcpy(load_msg, "Carregando logo...");
    logoNoBgImage = LoadImage(LOGO_BG_IMAGE_PATH);
    ImageResize(&logoNoBgImage, 256, 256);

    strcpy(load_msg, "Carregando imagem dos carros...");
    for (int i = 0; i < 3; i++) {
        carsImage[i] = LoadImage(CAR_IMAGES_PATH[i]);
    }
}

void loadAssets(Map *map) {
    loadImages(map);

    strcpy(load_msg, "Carregando checkpoints...");
    Track_setMask(&debugTrackImage);
    Track_setCheckpoints(map->checkpoints, map->checkpointSize);

    strcpy(load_msg, "Carregando volta de referencia...");
    referenceLap = ArrayList_create();
    ArrayList_clear(referenceLap);
    FILE *file = fopen(TextFormat("%s/%s_reference.bin", REFERENCE_DATA_PATH, map->name), "rb");
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
    UnloadTexture(minimapTexture);
    UnloadTexture(trackTexture);
    UnloadTexture(debugMinimapTexture);
    UnloadTexture(debugTrackTexture);
    UnloadTexture(speedometerTexture);
    UnloadTexture(logoNoBgTexture);
    LinkedList_clear(cars);
    if (state.mode == SINGLEPLAYER) {
        cleanUpSingleplayer();
    } else {
        cleanUpSplitscreen();
    }
    Camera_free(camera1);
}

void Game_cleanup() {
    if (state.screen != GAME) {
        mapCleanup();
    }
    ArrayList_free(referenceLap);
    LinkedList_free(cars);
}

//----------------------------------------------------------------------------------
// Atualização dos componentes do jogo (carros, camera...)
//----------------------------------------------------------------------------------

void Game_update() {
    if (!done) {
        if (loaded) {
            loadTextures();
            state.status = STARTED;
        }
        return;
    }

    SDL_GameControllerUpdate();

    if (IsKeyPressed(KEY_Q) || state.status == ENDED ||
        Controller_allButtonInputs(controllers, controllers_n).menu) {
        state.screen = MENU;
        mapCleanup();
        return;
    }

    if (IsKeyPressed(KEY_F1)) {
        state.debug = !state.debug;
    }

    updateCarRanking();
    LinkedList_forEach(cars, updateBestLapCar);

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

static void updateBestLapCar(Car *player) {
    if (player->lap < 1) {
        return;
    }

    if (player->changeLapFlag && player->lastLapTime <= bestLapTimePlayer->bestLapTime) {
        bestLapTimePlayer = player;
        bestLapLastTick   = GetTime() + 3;
    }
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
    if (state.screen != GAME) {
        return;
    }

    if (!done) {
        ClearBackground(WHITE);

        drawTextCenteredInRect("CARREGANDO MAPA",
                               (Rectangle) {0, SCREEN_HEIGHT / 3.0f, SCREEN_WIDTH, 128}, 64, BLACK,
                               FONTS[1]);

        drawTextCenteredInRect(load_msg,
                               (Rectangle) {0, SCREEN_HEIGHT / 3.0f + 96, SCREEN_WIDTH, 64}, 32,
                               BLACK, FONTS[3]);
        return;
    }

    ClearBackground(backgroundImageColor);

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
// Draw map
//----------------------------------------------------------------------------------

void drawMap() {
    DrawTexture(state.debug ? debugTrackTexture : trackTexture, 0, 0, WHITE);
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

    DrawTexture(state.debug ? debugMinimapTexture : minimapTexture, minimapPos.x, minimapPos.y,
                (Color) {255, 255, 255, HUD_OPACITY});
    LinkedList_forEach(cars, drawPlayerInMinimap);

    if (state.debug) {
        DrawFPS(0, 0);
    }
}

//----------------------------------------------------------------------------------
// Funções auxiliares para desenhar a hud
//----------------------------------------------------------------------------------

void drawBestLapMessage(float x, float y) {
    float actualTime = GetTime();

    if (bestLapLastTick < actualTime) {
        return;
    }

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
    DrawTexture(logoNoBgTexture, x + (hudPlayerListWidth - 256) / 2.0f, y - (252 - 128) / 2.0f,
                (Color) {255, 255, 255, HUD_OPACITY});
}

void drawLapTime(Car *player, float x, float y) {
    Color color = WHITE;

    if (GetTime() < bestLapLastTick) {
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
        24, color, FONTS[3]);
}

void drawPlayerInMinimap(Car *player) {
    float x = minimapTexture.width * player->pos.x / trackTexture.width + minimapPos.x;
    float y = minimapTexture.height * player->pos.y / trackTexture.height + minimapPos.y;

    if (player->ghost) {
        DrawCircleLines(x, y, 3.5f, BLACK);
        DrawCircle(x, y, 3, WHITE);
    } else {
        DrawCircleLines(x, y, 6.5f, BLACK);
        DrawCircle(x, y, 6, player->color);
    }
}

void drawSpeedometer(Car *player, float x, float y) {
    DrawTexture(speedometerTexture, x - speedometerTexture.width / 2.0f,
                y - speedometerTexture.height / 2.0f, (Color) {255, 255, 255, HUD_OPACITY});

    float   angle  = (fabs(player->vel) / player->maxVelocity) * (PI * 1.5) + (PI * 0.75);
    Vector2 endPos = {cosf(angle) * 100 + x, sinf(angle) * 100 + y};

    DrawLineEx((Vector2) {x, y}, endPos, 8, RED);
    DrawCircle(x, y, 3, RED);

    snprintf(strBuffer, sizeof(strBuffer), "%.0f",
             3600 * 0.75f * fabs(player->vel) * 60 / trackTexture.width);
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
                           FONTS[3]);
}

void drawPlayerList(Car *player, float x, float y) {
    stringifyTime(strBuffer, 599.999f, 1);
    Vector2 lapTimeSize = MeasureTextEx(FONTS[3], strBuffer, 20, 1.0f);

    int height  = 36;
    int padding = 9;

    int   idx  = 0;
    Node *prev = cars->head;
    Node *curr = cars->head;

    double actualTime = GetTime();

    while (curr != NULL) {
        if (curr->car->ghost && !curr->car->ghostActive) {
            prev = curr;
            curr = curr->next;
            continue;
        }

        float yx = y + idx * height;

        Color     bgColor = bestLapTimePlayer == curr->car && bestLapLastTick > actualTime
                                ? PURPLE
                                : (Color) {51, 51, 51, HUD_OPACITY};
        Rectangle rect    = {x, yx, hudPlayerListWidth, height};
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, bgColor);

        float fontSize = 20;

        snprintf(strBuffer, sizeof(strBuffer), "%d", idx + 1);
        DrawTextEx(FONTS[3], strBuffer,
                   (Vector2) {x + padding, yx + (height - (fontSize - 2)) / 2.0f}, fontSize - 2,
                   1.0f, WHITE);

        DrawRectangle(x + 36, yx + padding + 2.5f, 2, height / 2.5f, curr->car->color);

        DrawTextEx(FONTS[0], curr->car->name, (Vector2) {x + 44, yx + (height - fontSize) / 2},
                   fontSize, 1.0f, WHITE);

        if (idx == 0 || (idx == 0 && curr->next == NULL)) {
            strcpy(strBuffer, "-:--.---");
        } else {
            stringifyTime(strBuffer, (prev->car->refFrame - curr->car->refFrame) / 60.0f, 1);
        }

        drawTextCenteredInRect(
            strBuffer,
            (Rectangle) {rect.x + rect.width - lapTimeSize.x, yx, lapTimeSize.x, rect.height},
            fontSize - 2, WHITE, FONTS[3]);

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
             "Lap Flag: %d\n"
             "Last Lap Time: %.2f\n"
             "GameBestLapTime: %.2f",
             player->id, player->lap, player->startLapTime, GetTime() - player->startLapTime,
             player->bestLapTime, player->checkpoint, player->pos.x, player->pos.y, player->vel,
             player->maxVelocity, player->acc, player->width, player->height,
             fmod(player->angle, 2 * PI), player->angularSpeed, player->minTurnSpeed,
             player->breakForce, player->dragForce, player->reverseForce, player->refFrame,
             player->changeLapFlag, player->lastLapTime, bestLapTimePlayer->bestLapTime);

    Vector2 size = MeasureTextEx(FONTS[0], strBuffer, 20, 1.0f);
    DrawRectangle(x, y, size.x + 10, size.y + 10, (Color) {196, 196, 196, 200});
    DrawTextEx(FONTS[0], strBuffer, (Vector2) {x, y}, 20, 1.0f, BLACK);
}

// client.c
#include "client.h"
#include "car.h"
#include "raylib.h"
#include "server.h"

// Areas da pista
#define MAIN_TRACK_AREA (TrackArea){(Color) {127, 127, 127}, 0.01}
#define LIGHT_ESCAPE_AREA (TrackArea){(Color) {255, 127, 39}, 0.04}
#define HARD_ESCAPE_AREA (TrackArea){(Color) {163, 73, 164}, 0.07}
#define OUTSIDE_TRACK_AREA (TrackArea){(Color) {255, 255, 255}, 0.2}

// Checkpoints
#define RACE_START_CHECKPOINT (Checkpoint){(Color) {0, 255, 0}, (Vector2) {4371, 2537}, 2.66}
#define FIRST_CHECKPOINT (Checkpoint){(Color) {0, 0, 255}, (Vector2) {6700, 8147}, 0}
#define SECOND_CHECKPOINT (Checkpoint){(Color) {255, 0, 0}, (Vector2) {11069, 2257}, 2.17}

static Camera2D *camera;
static Car      *player;
static Texture2D trackTexture;
static int       playerId = 1;


static int SCREEN_WIDTH;
static int SCREEN_HEIGHT;

void Client_Init() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Formula C");
    SCREEN_WIDTH = GetScreenWidth();   
    SCREEN_HEIGHT = GetScreenHeight();
    Image icon = LoadImage("resources/logo/formula_c-logo.png");
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);

    Track_setMask("resources/masks/interlagos_maskV2.png");
    Track_setAreas(MAIN_TRACK_AREA, LIGHT_ESCAPE_AREA, HARD_ESCAPE_AREA, OUTSIDE_TRACK_AREA);
    Track_setCheckpoints(RACE_START_CHECKPOINT, FIRST_CHECKPOINT, SECOND_CHECKPOINT);

    trackTexture = LoadTexture("resources/masks/interlagos_maskV2.png");

    Camera_Screen_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera_Background_setSize(trackTexture.width, trackTexture.height);

    player = Car_create((Vector2) {5400, 2000}, // pos
                        2.66,                   // angulo inicial do carro

                        0.3,  // aceleracao do carro
                        0.2,  // força da marcha ré
                        0.02, // força de frenagem

                        0.035, // aceleração angular (velocidade de rotação)
                        0.2,   // velocidade mínima para fazer curva

                        125, // largura
                        75,  // altura

                        "resources/cars/carroazul.png", // path da textura
                        playerId                        // id do carro
    );

    Server_addCar(*player);

    camera = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, 0, 0.5f);
}

Car_data_transfer Client_CarData() {
    return (Car_data_transfer) {player->id,          player->lap,      player->startLapTime,
                                player->bestLapTime, player->raceTime, player->checkpoint,
                                player->pos,         player->vel,      player->angle,
                                player->dragForce};
}

void Client_Update() {
    Car_data_transfer serverCarData = Server_GetCarDataById(player->id);
    player->id                      = serverCarData.id;
    player->lap                     = serverCarData.lap;
    player->startLapTime            = serverCarData.startLapTime;
    player->bestLapTime             = serverCarData.bestLapTime;
    player->raceTime                = serverCarData.raceTime;
    player->checkpoint              = serverCarData.checkpoint;
    player->pos                     = serverCarData.pos;
    player->vel                     = serverCarData.vel;
    player->angle                   = serverCarData.angle;
    player->dragForce               = serverCarData.dragForce;

    Car_move(player, KEY_W, KEY_S, KEY_D, KEY_A);
    Camera_updateTarget(camera, player);
    Car_data_transfer carData = Client_CarData();
    Server_UpdateCar(carData);
}

void Client_Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(*camera);

    DrawTexture(trackTexture, 0, 0, WHITE);
    Car_showInfo(player, player->pos.x - GetScreenWidth() / 2,
                 player->pos.y - GetScreenHeight() / 2, 50, BLACK);
    Server_forEachCar(Car_draw);

    EndMode2D();
    EndDrawing();
}

void Client_Cleanup() {
    UnloadTexture(trackTexture);
    Camera_free(camera);
    Track_Unload();
    CloseWindow();
}
// client.c
#include "client.h"
#include "server.h"
#include "raylib.h"
#include "car.h"

// Largura e altura da tela em pixels
#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

// Cores das partes da pista
#define TRACK_COLOR (Color){127, 127, 127}
#define LIGHT_ESCAPE_AREA_COLOR (Color){255, 127, 39}
#define HARD_ESCAPE_AREA_COLOR (Color){163, 73, 164}
#define OUTSIDE_TRACK_COLOR (Color){255, 255, 255}

// Cores dos checkpoints
#define RACE_START_COLOR (Color){0, 255, 0}
#define FIRST_CHECKPOINT_COLOR (Color){0, 0, 255}
#define SECOND_CHECKPOINT_COLOR (Color){255, 0, 0}

// Forças de atrito do carro com as diferentes partes da pista
#define TRACK_DRAG 0.01
#define LIGHT_ESCAPE_AREA_DRAG 0.04
#define HARD_ESCAPE_AREA_DRAG 0.07
#define OUTSIDE_TRACK_DRAG 0.2

static Camera2D* camera;
static Car* player;
static Texture2D trackTexture;
static int playerId = 1;

void Client_Init() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Formula C");
    Image icon = LoadImage("resources/logo/formula_c-logo.png");
    ImageResize(&icon, 32, 32);
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);

    Track_setMask("resources/masks/interlagos_mask.png");
    Track_setDrag(TRACK_DRAG, LIGHT_ESCAPE_AREA_DRAG, HARD_ESCAPE_AREA_DRAG, OUTSIDE_TRACK_DRAG);
    Track_setDragColor(TRACK_COLOR, LIGHT_ESCAPE_AREA_COLOR, HARD_ESCAPE_AREA_COLOR,
                   OUTSIDE_TRACK_COLOR);
    Track_setCheckpointColor(RACE_START_COLOR, FIRST_CHECKPOINT_COLOR,SECOND_CHECKPOINT_COLOR);
    
    trackTexture = LoadTexture("resources/masks/interlagos_mask.png");

    Camera_Screen_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera_Background_setSize(trackTexture.width, trackTexture.height);

    player = Car_create(
        (Vector2){5400, 2000},     // pos
        2.66,                      // angulo inicial do carro

        0.3,                       // aceleracao do carro
        0.2,                       // força da marcha ré
        0.02,                      // força de frenagem

        0.035,                     // aceleração angular (velocidade de rotação)
        0.2,                       // velocidade mínima para fazer curva

        100,                       // largura
        50,                        // altura

        "resources/cars/carroazul.png", // path da textura
        playerId                          // id do carro
    );

    Server_addCar(*player);

    camera = Camera_create(player->pos, (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}, 0, 0.5f);
}

Car_data_transfer Client_CarData(){
    return (Car_data_transfer){
        player->id,
        player->lap,
        player->startLapTime,
        player->bestLapTime,
        player->raceTime,
        player->checkpoint,
        player->pos,
        player->vel,
        player->angle,
        player->dragForce
    };
}

void Client_Update() {
    Car_data_transfer serverCarData = Server_GetCarDataById(player->id);
    player->id = serverCarData.id;
    player->lap = serverCarData.lap;
    player->startLapTime = serverCarData.startLapTime;
    player->bestLapTime = serverCarData.bestLapTime;
    player->raceTime = serverCarData.raceTime;
    player->checkpoint = serverCarData.checkpoint;
    player->pos = serverCarData.pos;
    player->vel = serverCarData.vel;
    player->angle = serverCarData.angle;
    player->dragForce = serverCarData.dragForce;

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
    Car_showInfo(player, player->pos.x - GetScreenWidth()/2, player->pos.y - GetScreenHeight()/2, 50, BLACK);
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
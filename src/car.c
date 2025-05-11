#include "car.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Areas da pista
static TrackArea TRACK_AREAS[10];
static int TRACK_AREA_SIZE;

// Cor que representa a área fora da pista
static Color OUTSIDE_AREA_COLOR;

// Checkpoints
static Checkpoint CHECKPOINTS[10];
static int CHECKPOINTS_SIZE;

// Pixels da imagem da pista
static int    IMAGE_WIDTH;
static int    IMAGE_HEIGHT;
static Color *TRACK_PIXELS;

void Track_setMask(char *track_mask_path) { // Definindo a imagem da máscara de pixels
    Image trackMask = LoadImage(track_mask_path);
    IMAGE_WIDTH     = trackMask.width;
    IMAGE_HEIGHT    = trackMask.height;
    TRACK_PIXELS    = LoadImageColors(trackMask);
    UnloadImage(trackMask);
}

void Track_setAreas(TrackArea areas[], int size) {
    TRACK_AREA_SIZE=size;
    for (int i = 0; i < size; i++){
        areas[i].dragForce = 1 - areas[i].dragForce;
        TRACK_AREAS[i] = areas[i];
    }
}

void Track_setOutsideColor(Color color) {
    OUTSIDE_AREA_COLOR = color;
}

void Track_setCheckpoints(Checkpoint checkpoints[], int size) { // Definindo as cores dos checkpoints
    CHECKPOINTS_SIZE=size;
    for (int i = 0; i < size; i++){
        CHECKPOINTS[i] = checkpoints[i];
    }
}

void Track_Unload() { // Função para descarregar as variáveis associadas a pista
    UnloadImageColors(TRACK_PIXELS);
}

static bool equalsColor(Color a, Color b) { // Verifica se uma cor é igual a outra
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

static int getCheckpoint(Color color) { // Retorna o número int associado aql checkpoint
    for (int i = 0; i < CHECKPOINTS_SIZE; i++) {
        if (equalsColor(color, CHECKPOINTS[i].color))
            return i;
    }
    return -1;
}

static bool isOutSideTrack(Color color) { // Verifica se está fora da pista
    return equalsColor(color, OUTSIDE_AREA_COLOR);
}

static void Car_updateDragForce(Car *car, Color floorColor) { // Atualiza a força de atrito
    for (int i = 0; i < TRACK_AREA_SIZE; i++) {
        if (equalsColor(floorColor, TRACK_AREAS[i].color)) {
            car->dragForce = TRACK_AREAS[i].dragForce;
        }
    }

    if (isOutSideTrack(floorColor) && car->lap >= 0) {
        car->vel       = 10;
        car->dragForce = 1;
        car->pos       = CHECKPOINTS[car->checkpoint].pos;
        car->angle     = CHECKPOINTS[car->checkpoint].angle;
    }
}

static int Car_checkCheckpoint(
    Car *car, Color floorColor) { // Verifica se passou por um checkpoint e atualiza tempos do carro
    int checkpoint = getCheckpoint(floorColor);
    if (checkpoint < 0)
        return -1;

    int nextExpected = (car->checkpoint + 1) % 3;
    if (checkpoint == nextExpected) {
        car->checkpoint = checkpoint;

        if (checkpoint == 0) { // completou a volta
            car->lap++;

            double now     = GetTime();
            double lapTime = now - car->startLapTime;

            if (car->lap == 0)
                car->raceTime = now;

            if ((car->bestLapTime < 0 || lapTime < car->bestLapTime) && car->lap > 0) {
                car->bestLapTime = lapTime;
            }

            car->startLapTime = now;
        }
    }

    return checkpoint;
}

static void Car_applyPhysics(Car *car) { // Atualiza a posição com base na velocidade e no ângulo
    car->vel *= car->dragForce;
    car->pos.x += cos(car->angle) * car->vel;
    car->pos.y += sin(car->angle) * car->vel;
}

static Color Car_getFloor(Car *car) { // Retorna a cor embaixo do carro
    int x = (int) (car->pos.x + cos(car->angle) * car->width * 0.8f);
    int y = (int) (car->pos.y + sin(car->angle) * car->height * 0.5f);
    if (x < 0 || x >= IMAGE_WIDTH || y < 0 || y >= IMAGE_HEIGHT)
        return (Color) {0, 0, 0};
    return TRACK_PIXELS[y * IMAGE_WIDTH + x];
}

static void Car_accelerate(Car *car) { // Acelera o carro
    car->vel += car->acc;
}

static bool Car_canTurn(
    Car *car) { // Verificar se está acima da velocidade mínima (em módulo) para fazer a curva
    return car->vel > car->minTurnSpeed || car->vel < -car->minTurnSpeed;
}

static void Car_turn(Car *car, float angle) { // Virar com um angulo
    if (Car_canTurn(car))
        car->angle += angle;
}

static void Car_turnLeft(Car *car) { // Virar para a esquerda
    Car_turn(car, -car->angularAcc);
}

static void Car_turnRight(Car *car) { // Virar para a direita
    Car_turn(car, car->angularAcc);
}

static void Car_break(Car *car) { // Freiar
    car->vel *= car->breakForce;
}

static void Car_reverse(Car *car) { // Marcha ré
    car->vel -= car->acc * car->reverseForce;
}

void Car_update(Car *car) {
    Color floorColor = Car_getFloor(car); // Pega a cor do chão embaixo do carro

    if (Car_checkCheckpoint(car, floorColor) == -1) { // Se não está passando por um checkpoint
        Car_updateDragForce(car, floorColor);         // Atualiza a força de atrito
    }

    Car_applyPhysics(car);
}

void Car_draw(Car *car) {
    Rectangle sourceRec = {0, 0, car->texture.width, car->texture.height}; // A imagem inteira
    Rectangle destRec   = {car->pos.x, car->pos.y, car->width,
                           car->height};                           // Tamanho e posição do carro
    Vector2   origin    = {car->width * 0.5f, car->height * 0.5f}; // Centro da imagem para rotação
    DrawTexturePro(car->texture, sourceRec, destRec, origin, car->angle * RAD2DEG, WHITE);
}

Car *Car_create(   // Função para criar um carro
    Vector2 pos,   // posição inicial
    float   angle, // orientação inicial

    float acc,             // aceleração
    float reverseForce,    // força de ré
    float breakCoeficient, // coeficiente de frenagem

    float angularAcc,   // aceleração angular
    float minTurnSpeed, // velocidade mínima para virar

    int width,  // largura do carro
    int height, // altura do carro

    const char *texturePath, // path da textura
    int         id           // identificador único
) {
    Car *car = (Car *) malloc(sizeof(Car));
    if (car == NULL)
        return NULL;
    car->pos          = pos;
    car->acc          = acc;
    car->texture      = LoadTexture(texturePath);
    car->width        = width;
    car->height       = height;
    car->angle        = angle;
    car->angularAcc   = angularAcc;
    car->minTurnSpeed = minTurnSpeed;
    car->breakForce   = 1 - breakCoeficient;
    car->reverseForce = reverseForce;
    car->dragForce    = 0;
    car->id           = id;
    car->lap          = -1;
    car->vel          = 0;
    car->startLapTime = GetTime();
    car->raceTime     = GetTime();
    car->bestLapTime  = -1;
    car->checkpoint   = 2;
    return car;
}

Car *Car_createEmpty() {
    Car *car = (Car *) calloc(1, sizeof(Car));
    if (car == NULL)
        return NULL;
    return car;
}

void Car_free(Car *car) {
    UnloadTexture(car->texture);
    free(car);
}

void Car_move(Car *car, int up, int down, int right,
              int left) { // Atualiza as propriedades do carro de acordo com o input do player
    if (IsKeyDown(up)) {
        Car_accelerate(car);
    }
    if (IsKeyDown(left)) {
        Car_turnLeft(car);
    }
    if (IsKeyDown(right)) {
        Car_turnRight(car);
    }

    if (IsKeyDown(down)) {
        if (car->vel <= car->minTurnSpeed) {
            Car_reverse(car);
        } else {
            Car_break(car);
        }
    }
}

void Car_showInfo(Car *car, int x, int y, int fontSize, Color fontColor) {
    char car_info[1000];
    snprintf(car_info, sizeof(car_info),
             "ID: %d\n"
             "Lap: %d\n"
             "Start Lap Time: %.2f\n"
             "Current Lap Time: %.2f\n"
             "Best Lap Time: %.2f\n"
             "Race Time: %.2f\n"
             "Checkpoint: %d\n"
             "Position: (%.1f, %.1f)\n"
             "Velocity: %.2f\n"
             "Acceleration: %.2f\n"
             "Size: %dx%d\n"
             "Angle: %.2f\n"
             "Angular Acceleration: %.2f\n"
             "Min Turn Speed: %.2f\n"
             "Brake Force: %.2f\n"
             "Drag Force: %.2f\n"
             "Reverse Force: %.2f",
             car->id, car->lap, car->startLapTime, GetTime() - car->startLapTime, car->bestLapTime,
             GetTime() - car->raceTime, car->checkpoint, car->pos.x, car->pos.y, car->vel, car->acc,
             car->width, car->height, car->angle, car->angularAcc, car->minTurnSpeed,
             car->breakForce, car->dragForce, car->reverseForce);
    DrawText(car_info, x, y, fontSize, fontColor);
}

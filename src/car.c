#include "car.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Forças de atrito do carro com as diferentes partes da pista
static float TRACK_DRAG;
static float LIGHT_ESCAPE_AREA_DRAG;
static float HARD_ESCAPE_AREA_DRAG;
static float OUTSIDE_TRACK_DRAG;

// Cores das diferentes partes da pista
static Color TRACK_COLOR;
static Color LIGHT_ESCAPE_AREA_COLOR;
static Color HARD_ESCAPE_AREA_COLOR;
static Color OUTSIDE_TRACK_COLOR;

// Cores dos checkpoints
static Color RACE_START_COLOR;
static Color FIRST_CHECKPOINT_COLOR;
static Color SECOND_CHECKPOINT_COLOR;

// Pixels da imagem da pista
static int IMAGE_WIDTH;
static int IMAGE_HEIGHT;
static Color *TRACK_PIXELS;

void Track_setDrag(float track_drag, float light_escape_area_drag, float hard_escape_area_drag,
                   float ouside_track_drag) { // Definindo a força de atrito com as partes da pista
    TRACK_DRAG             = 1 - track_drag;
    LIGHT_ESCAPE_AREA_DRAG = 1 - light_escape_area_drag;
    HARD_ESCAPE_AREA_DRAG  = 1 - hard_escape_area_drag;
    OUTSIDE_TRACK_DRAG     = 1 - ouside_track_drag;
}

void Track_setMask(char* track_mask_path) { // Definindo a imagem da máscara de pixels
    Image trackMask = LoadImage(track_mask_path);
    IMAGE_WIDTH = trackMask.width;
    IMAGE_HEIGHT = trackMask.height;
    TRACK_PIXELS = LoadImageColors(trackMask);
    UnloadImage(trackMask);
}

void Track_setDragColor(Color track, Color light_escape, Color hard_escape, Color outside) { // Definindo as cores de cada parte da pista e checkpoint
    TRACK_COLOR             = track;
    LIGHT_ESCAPE_AREA_COLOR = light_escape;
    HARD_ESCAPE_AREA_COLOR  = hard_escape;
    OUTSIDE_TRACK_COLOR     = outside;
}

void Track_setCheckpointColor(Color race_start, Color first_check, Color second_check){ // Definindo as cores dos checkpoints
    RACE_START_COLOR        = race_start;
    FIRST_CHECKPOINT_COLOR  = first_check;
    SECOND_CHECKPOINT_COLOR = second_check;
}

void Track_Unload() { // Função para descarregar as variáveis associadas a pista
    UnloadImageColors(TRACK_PIXELS);
}

static bool equalsColor(Color a, Color b) { // Verifica se uma cor é igual a outra
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

static int getCheckpoint(Color color) { // Retorna o número int associado aql checkpoint
    if (equalsColor(color, RACE_START_COLOR))
        return 0;
    if (equalsColor(color, FIRST_CHECKPOINT_COLOR))
        return 1;
    if (equalsColor(color, SECOND_CHECKPOINT_COLOR))
        return 2;
    return -1;
}

static bool isOnTrack(Color color) { // Verifica se está na pista
    return equalsColor(color, TRACK_COLOR);
}

static bool isOnLightEscapeArea(Color color) { // Verifica se está na área de escape de menor atrito
    return equalsColor(color, LIGHT_ESCAPE_AREA_COLOR);
}

static bool isOnHardEscapeArea(Color color) { // Verifica se está na área de escape de maior atrito
    return equalsColor(color, HARD_ESCAPE_AREA_COLOR);
}

static bool isOutSideTrack(Color color) { // Verifica se está fora da pista
    return equalsColor(color, OUTSIDE_TRACK_COLOR);
}

static void Car_updateDragForce(Car *car, Color floorColor) { // Atualiza a força de atrito
    if (isOnTrack(floorColor)) {
        car->dragForce = TRACK_DRAG;
    } else if (isOnLightEscapeArea(floorColor)) {
        car->dragForce = LIGHT_ESCAPE_AREA_DRAG;
    } else if (isOnHardEscapeArea(floorColor)) {
        car->dragForce = HARD_ESCAPE_AREA_DRAG;
    } else if (isOutSideTrack(floorColor)) {
        car->dragForce = OUTSIDE_TRACK_DRAG;
    }
}

static int Car_checkCheckpoint(Car *car, Color floorColor) { // Verifica se passou por um checkpoint e atualiza tempos do carro
    int checkpoint = getCheckpoint(floorColor);
    if (checkpoint < 0) return -1;

    int nextExpected = (car->checkpoint + 1) % 3;
    if (checkpoint == nextExpected) {
        car->checkpoint = checkpoint;

        if (checkpoint == 0) {  // completou a volta
            car->lap++;

            double now = GetTime();
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
    int x = (int)(car->pos.x + cos(car->angle) * car->width * 0.8f);
    int y = (int)(car->pos.y + sin(car->angle) * car->height * 0.5f);
    if (x < 0 || x >= IMAGE_WIDTH || y < 0 || y >= IMAGE_HEIGHT)
        return (Color) {0, 0, 0};
    return TRACK_PIXELS[y * IMAGE_WIDTH + x];
}

static void Car_accelerate(Car *car) { // Acelera o carro
    car->vel += car->acc;
}

static bool Car_canTurn(Car *car) { // Verificar se está acima da velocidade mínima (em módulo) para fazer a curva
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
        Car_updateDragForce(car, floorColor); // Atualiza a força de atrito
    }

    Car_applyPhysics(car);
}

void Car_draw(Car *car) {
    Rectangle sourceRec = {0, 0, car->texture.width, car->texture.height};  // A imagem inteira
    Rectangle destRec = {car->pos.x, car->pos.y, car->width, car->height};  // Tamanho e posição do carro
    Vector2 origin = {car->width * 0.5f, car->height * 0.5f};  // Centro da imagem para rotação
    DrawTexturePro(car->texture, sourceRec, destRec, origin, car->angle * RAD2DEG, WHITE);
}

Car *Car_create(             // Função para criar um carro
    Vector2 pos,             // posição inicial 
    float angle,             // orientação inicial 

    float acc,               // aceleração
    float reverseForce,      // força de ré
    float breakCoeficient,   // coeficiente de frenagem

    float angularAcc,        // aceleração angular
    float minTurnSpeed,      // velocidade mínima para virar

    int width,               // largura do carro
    int height,              // altura do carro

    const char *texturePath, // path da textura 
    int id                   // identificador único 
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

void Car_free(Car *car) {
    UnloadTexture(car->texture);
    free(car);
}

void Car_move(Car *car, int up, int down, int right, int left) { // Atualiza as propriedades do carro de acordo com o input do player
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
        car->id,
        car->lap,
        car->startLapTime,
        GetTime()-car->startLapTime,
        car->bestLapTime,
        GetTime()-car->raceTime,
        car->checkpoint,
        car->pos.x, car->pos.y,
        car->vel,
        car->acc,
        car->width, car->height,
        car->angle,
        car->angularAcc,
        car->minTurnSpeed,
        car->breakForce,
        car->dragForce,
        car->reverseForce
    );
    DrawText(car_info, x, y, fontSize, fontColor);
}

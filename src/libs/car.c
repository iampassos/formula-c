#include "car.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Checkpoints
static Checkpoint CHECKPOINTS[100];
static int        CHECKPOINTS_SIZE;

// Pixels da imagem da pista
static int    IMAGE_WIDTH;
static int    IMAGE_HEIGHT;
static Color *TRACK_PIXELS;

static float MIN_DIST_TO_DETECT;

// --- Funções públicas ---
void Track_setMask(char *track_mask_path); // Definindo a mascara de pixel para carros lerem

void Track_setCheckpoints(Checkpoint checkpoints[], int size); // Define as cores dos checkpoints

void Track_Unload(); // Libera a memória de recursos da mascara de pixels

Car *Car_create(Vector2 pos, float angle, CarConfig config, const char *texturePath, Color color,
                bool ghost, int id);

void Car_free(Car *car); // Libera a memória de um carro

void Car_update(Car *car); // Atualizar a posição do carro a cada frame
void Car_draw(Car *car);   // Desenhar o carro na tela

void Car_move(Car *car, int up, int down, int right,
              int left); // Atualiza o carro de acordo com os inputs do usuário

void Car_showInfo(Car *car, int x, int y, int fontSize,
                  Color fontColor); // Mostra as informações do carro no console (PARA DEBUG)

// --- Funções internas ---
static bool  ColorEquals(Color a, Color b);
static float vecDist(Vector2 a, Vector2 b);

static Color getFloorColor(Car *car);

static bool isValidCheckpoint(Car *car, int nextExpected, Color floorColor);
static void returnToLastCheckpoint(Car *car);
static void updateLapStatus(Car *car, Color floorColor);

static void applyDragForce(Car *car, Color floorColor);
static void applyMovementPhysics(Car *car);

static bool canTurn(Car *car);

static void turn(Car *car, float angle);
static void turnLeft(Car *car);
static void turnRight(Car *car);
static void breakSpeed(Car *car);
static void reverse(Car *car);
static void accelerate(Car *car);

//----------------------------------------------------------------------------------
// Carregamento da pista e checkpoints
//----------------------------------------------------------------------------------

void Track_setMask(char *track_mask_path) { // Definindo a imagem da máscara de pixels
    Image trackMask = LoadImage(track_mask_path);
    IMAGE_WIDTH     = trackMask.width;
    IMAGE_HEIGHT    = trackMask.height;
    TRACK_PIXELS    = LoadImageColors(trackMask);
    UnloadImage(trackMask);
    MIN_DIST_TO_DETECT = IMAGE_WIDTH / 30;
}

// Definindo as cores dos checkpoints
void Track_setCheckpoints(Checkpoint checkpoints[], int size) {
    CHECKPOINTS_SIZE = size;
    for (int i = 0; i < size; i++) {
        CHECKPOINTS[i] = checkpoints[i];
    }
}

void Track_Unload() { // Função para descarregar as variáveis associadas a pista
    UnloadImageColors(TRACK_PIXELS);
}

//----------------------------------------------------------------------------------
// Criar / liberar memória do carro
//----------------------------------------------------------------------------------

Car *Car_create(Vector2 pos, float angle, CarConfig config, const char *texturePath, Color color,
                bool ghost, int id) {
    Car *car = (Car *) malloc(sizeof(Car));
    if (car == NULL)
        return NULL;
    car->pos             = pos;
    car->texture         = LoadTexture(texturePath);
    car->angle           = angle;
    car->acc             = config.acc;
    car->width           = config.width;
    car->height          = config.height;
    car->angularSpeed    = config.angularSpeed;
    car->maxAngularSpeed = config.angularSpeed * 30;
    car->minAngularSpeed = config.angularSpeed * 7;
    car->breakForce      = config.breakForce;
    car->reverseForce    = config.reverseForce;
    car->color           = color;
    car->ghost           = ghost;
    car->maxVelocity     = TRACK_AREAS[0].dragForce / (1 - TRACK_AREAS[0].dragForce) * car->acc;
    car->minTurnSpeed    = car->maxVelocity / 50;
    car->dragForce       = 1;
    car->id              = id;
    car->lap             = -1;
    car->vel             = 0;
    car->startLapTime    = GetTime();
    car->raceTime        = GetTime();
    car->bestLapTime     = -1;
    car->checkpoint      = -1;
    car->sound           = LoadMusicStream(CAR_SOUND_PATH);
    PlayMusicStream(car->sound);
    SetMusicVolume(car->sound, CAR_VOLUME);
    return car;
}

void Car_free(Car *car) {
    UnloadTexture(car->texture);
    UnloadMusicStream(car->sound);
    free(car);
}

//----------------------------------------------------------------------------------
// Atualizar a posição e física do carro
//----------------------------------------------------------------------------------

void Car_update(Car *car) {
    if (car->ghost)
        return;
    SetMusicPitch(car->sound, 0.6 + car->vel / 13.0f);
    UpdateMusicStream(car->sound);
    Color floorColor = getFloorColor(car); // Pega a cor do chão embaixo do carro

    updateLapStatus(car, floorColor);
    applyDragForce(car, floorColor); // Atualiza a força de atrito
    applyMovementPhysics(car);

    if (ColorEquals(floorColor, OUTSIDE_TRACK_COLOR))
        returnToLastCheckpoint(car);
}

// Atualiza as propriedades do carro de acordo com o input do player
void Car_move(Car *car, int up, int down, int right, int left) {
    if (IsKeyDown(up)) {
        accelerate(car);
    }
    if (canTurn(car)) {
        if (IsKeyDown(left)) {
            turnLeft(car);
        }
        if (IsKeyDown(right)) {
            turnRight(car);
        }
    }

    if (IsKeyDown(down)) {
        if (car->vel < car->minTurnSpeed) {
            reverse(car);
        } else {
            breakSpeed(car);
        }
    }
}

//----------------------------------------------------------------------------------
// Desenhar o carro
//----------------------------------------------------------------------------------

void Car_draw(Car *car) {
    Rectangle sourceRec = {0, 0, car->texture.width, car->texture.height}; // A imagem inteira
    Rectangle destRec   = {car->pos.x, car->pos.y, car->width,
                           car->height};                           // Tamanho e posição do carro
    Vector2   origin    = {car->width * 0.5f, car->height * 0.5f}; // Centro da imagem para rotação
    DrawTexturePro(car->texture, sourceRec, destRec, origin, car->angle * RAD2DEG,
                   car->ghost ? Fade(car->color, 0.5f) : WHITE);
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
             "Max velocity: %.2f\n"
             "Acceleration: %.2f\n"
             "Size: %dx%d\n"
             "Angle: %.2f\n"
             "Angular Speed: %.2f\n"
             "Min Turn Speed: %.2f\n"
             "Brake Force: %.2f\n"
             "Drag Force: %.2f\n"
             "Reverse Force: %.2f\n",
             car->id, car->lap, car->startLapTime, GetTime() - car->startLapTime, car->bestLapTime,
             GetTime() - car->raceTime, car->checkpoint, car->pos.x, car->pos.y, car->vel,
             car->maxVelocity, car->acc, car->width, car->height, car->angle, car->angularSpeed,
             car->minTurnSpeed, car->breakForce, car->dragForce, car->reverseForce);
    DrawText(car_info, x, y, fontSize, fontColor);
}

//----------------------------------------------------------------------------------
// Funções extras das structs da raylib
//----------------------------------------------------------------------------------

static bool ColorEquals(Color a, Color b) { // Verifica se uma cor é igual a outra
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

static float vecDist(Vector2 a, Vector2 b) {
    float deltaX = b.x - a.x;
    float deltaY = b.y - a.y;
    return sqrtf(deltaX * deltaX + deltaY * deltaY);
}

//----------------------------------------------------------------------------------
// Sensor do carro
//----------------------------------------------------------------------------------

static Color getFloorColor(Car *car) { // Retorna a cor embaixo do carro
    int x = (int) (car->pos.x + cosf(car->angle) * car->width * 0.4f);
    int y = (int) (car->pos.y + sinf(car->angle) * car->width * 0.4f);
    if (x < 0 || x >= IMAGE_WIDTH || y < 0 || y >= IMAGE_HEIGHT)
        return (Color) {0, 0, 0};
    return TRACK_PIXELS[y * IMAGE_WIDTH + x];
}

//----------------------------------------------------------------------------------
// Funções que lidam com os checkpoints
//----------------------------------------------------------------------------------

static bool isValidCheckpoint(Car *car, int nextExpected, Color floorColor) {
    if (!ColorEquals(floorColor, CHECKPOINTS_COLOR))
        return false;

    return vecDist(car->pos, CHECKPOINTS[nextExpected].pos) < MIN_DIST_TO_DETECT;
}

static void returnToLastCheckpoint(Car *car) {
    car->vel       = 10;
    car->dragForce = 1;
    car->pos       = CHECKPOINTS[car->checkpoint].pos;
    car->angle     = CHECKPOINTS[car->checkpoint].angle;
}

//----------------------------------------------------------------------------------
// Função que lida com a volta do carro na pista
//----------------------------------------------------------------------------------

// Verifica se passou por um checkpoint e atualiza tempos do carro
static void updateLapStatus(Car *car, Color floorColor) {
    int nextExpected = (car->checkpoint + 1) % CHECKPOINTS_SIZE;

    if (!isValidCheckpoint(car, nextExpected, floorColor))
        return;

    car->checkpoint = nextExpected;

    if (nextExpected == 0) { // completou a volta
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

//----------------------------------------------------------------------------------
// Aplicando física no carro
//----------------------------------------------------------------------------------

static void applyDragForce(Car *car, Color floorColor) { // Atualiza a força de atrito
    for (int i = 0; i < TRACK_AREA_SIZE; i++) {
        if (ColorEquals(floorColor, TRACK_AREAS[i].color)) {
            car->dragForce = TRACK_AREAS[i].dragForce;
            return;
        }
    }
}

// Atualiza a posição com base na velocidade e no ângulo
static void applyMovementPhysics(Car *car) {
    car->vel *= car->dragForce;
    car->pos.x += cosf(car->angle) * car->vel;
    car->pos.y += sinf(car->angle) * car->vel;
}

//----------------------------------------------------------------------------------
// Funções de alteração das propriedades físicas do carro
//----------------------------------------------------------------------------------

static void accelerate(Car *car) { // Acelera o carro
    car->vel += car->acc;
}

// Verificar se está acima da velocidade mínima (em módulo) para fazer a curva
static bool canTurn(Car *car) {
    return fabs(car->vel) > car->minTurnSpeed;
}

static void turn(Car *car, float angle) {
    // Calcule um fator interpolado baseado na velocidade relativa
    float speedRatio = car->vel / car->maxVelocity;

    // Interpola linearmente entre maxSensitivity e minSensitivity
    float steeringSensitivity = car->maxAngularSpeed - (car->maxAngularSpeed - car->minAngularSpeed) * speedRatio;

    // Aplica o ângulo com sensibilidade ajustada
    car->angle += angle * steeringSensitivity;
}

static void turnLeft(Car *car) { // Virar para a esquerda
    turn(car, -car->angularSpeed);
}

static void turnRight(Car *car) { // Virar para a direita
    turn(car, car->angularSpeed);
}

static void breakSpeed(Car *car) { // Freiar
    car->vel *= car->breakForce;
}

static void reverse(Car *car) { // Marcha ré
    car->vel -= car->acc * car->reverseForce;
}

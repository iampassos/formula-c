#include "car.h"
#include "common.h"
#include "controller.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Checkpoints
static Checkpoint CHECKPOINTS[100];
static int        CHECKPOINTS_SIZE;

// Pixels da imagem da pista
static Color *TRACK_PIXELS;

static float MIN_DIST_TO_DETECT;

// --- Funções internas ---
static void updateFloorColor(Car *car);

static bool isValidCheckpoint(Car *car, int nextExpected, Color floorColor);
static void returnIfIsOutside(Car *car);
static void updateLapStatus(Car *car);

static void applyDragForce(Car *car);
static void applyMovementPhysics(Car *car);

static bool canTurn(Car *car);

static void turn(Car *car, float angle);
static void turnLeft(Car *car);
static void turnRight(Car *car);
static void breakSpeed(Car *car, float force);
static void reverse(Car *car);
static void accelerate(Car *car, float force);

//----------------------------------------------------------------------------------
// Carregamento da pista e checkpoints
//----------------------------------------------------------------------------------

// Definindo a imagem da máscara de pixels
void Track_setMask(Image *mask_image) {
    TRACK_PIXELS       = LoadImageColors(*mask_image);
    MIN_DIST_TO_DETECT = MAP_WIDTH / 30;
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

Car *Car_create(Vector2 pos, float angle, CarConfig config, Image *image, Color color, bool ghost,
                int id, char *name) {
    Car *car = (Car *) malloc(sizeof(Car));
    if (car == NULL) {
        return NULL;
    }

    car->pos             = pos;
    car->image           = *image;
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
    car->ghostActive     = false;
    car->changeLapFlag   = false;
    car->maxVelocity     = TRACK_AREAS[0].dragForce / (1 - TRACK_AREAS[0].dragForce) * car->acc;
    car->minTurnSpeed    = car->maxVelocity / 50;
    car->id              = id;
    car->lap             = -1;
    car->vel             = 0;
    car->startLapTime    = GetTime();
    car->bestLapTime     = INFINITY;
    car->lastLapTime     = INFINITY;
    car->checkpoint      = CHECKPOINTS_SIZE - 1; // COmeca no ultimo check point
    car->sound           = LoadMusicStream(CAR_SOUND_PATH);
    car->refFrame        = 0;
    strcpy(car->name, name);
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
    updateFloorColor(car);

    updateLapStatus(car);
    applyDragForce(car); // Atualiza a força de atrito
    applyMovementPhysics(car);
    returnIfIsOutside(car);
}

// Atualiza as propriedades do carro de acordo com o input do player
void Car_move(Car *car, int up, int down, int right, int left, SDL_GameController *controller) {
    if (car == NULL)
        return;

    SDL_GameControllerUpdate();
    ControllerInput input = Controller_input(controller);

    if (IsKeyDown(up) || fabs(input.rt) > 0.10f) {
        accelerate(car, controller ? input.rt : 1.0f);
    }

    if (canTurn(car)) {
        if (controller && fabs(input.leftAxis.x) > 0.1f) {
            turn(car, input.leftAxis.x * car->angularSpeed);
        } else {
            if (IsKeyDown(left)) {
                turnLeft(car);
            }
            if (IsKeyDown(right)) {
                turnRight(car);
            }
        }
    }

    if (IsKeyDown(down) || fabs(input.lt) > 0.10f) {
        if (car->vel < car->minTurnSpeed) {
            reverse(car);
        } else {
            breakSpeed(car, controller ? input.lt : 1.0f);
        }
    }
}

//----------------------------------------------------------------------------------
// Desenhar o carro
//----------------------------------------------------------------------------------

void Car_draw(Car *car) {
    if (car->ghost && !car->ghostActive)
        return;
    // Retangulo da imagem do carro
    Rectangle sourceRec = {0, 0, car->texture.width, car->texture.height};

    // Tamanho e posição do carro
    Rectangle destRec = {car->pos.x, car->pos.y, car->width, car->height};

    // Centro da imagem para rotação
    Vector2 origin = {car->width * 0.5f, car->height * 0.5f};

    DrawTexturePro(car->texture, sourceRec, destRec, origin, car->angle * RAD2DEG,
                   car->ghost ? (Color) {255, 255, 255, 127} : WHITE);
}

//----------------------------------------------------------------------------------
// Sensor do carro
//----------------------------------------------------------------------------------

// Atualiza o que o carro esta lendo
static void updateFloorColor(Car *car) {
    Color temp;
    int   x = (int) (car->pos.x + cosf(car->angle) * car->width * 0.4f);
    int   y = (int) (car->pos.y + sinf(car->angle) * car->width * 0.4f);

    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        temp = OUTSIDE_TRACK_COLOR;
    else
        temp = TRACK_PIXELS[y * MAP_WIDTH + x];

    car->floorColor = temp;
}

//----------------------------------------------------------------------------------
// Funções que lidam com os checkpoints
//----------------------------------------------------------------------------------

static bool isValidCheckpoint(Car *car, int nextExpected, Color floorColor) {
    if (!Color_equals(floorColor, CHECKPOINTS_COLOR))
        return false;

    return Vector2_dist(car->pos, CHECKPOINTS[nextExpected].pos) < MIN_DIST_TO_DETECT;
}

static void returnIfIsOutside(Car *car) {
    if (!Color_equals(car->floorColor, OUTSIDE_TRACK_COLOR))
        return;
    car->vel   = 10;
    car->pos   = CHECKPOINTS[car->checkpoint].pos;
    car->angle = CHECKPOINTS[car->checkpoint].angle;
}

//----------------------------------------------------------------------------------
// Função que lida com a volta do carro na pista
//----------------------------------------------------------------------------------

// Verifica se passou por um checkpoint e atualiza tempos do carro
static void updateLapStatus(Car *car) {
    car->changeLapFlag = false;
    int nextExpected   = (car->checkpoint + 1) % CHECKPOINTS_SIZE;

    if (!isValidCheckpoint(car, nextExpected, car->floorColor))
        return;

    car->checkpoint = nextExpected;

    if (nextExpected == 0) { // completou a volta
        car->changeLapFlag = true;
        car->lap++;

        double now = GetTime();
        if (car->lap > 0) {
            car->lastLapTime = now - car->startLapTime;

            if (car->lastLapTime < car->bestLapTime) {
                car->bestLapTime = car->lastLapTime;
            }
        }

        car->startLapTime = now;
    }
}

//----------------------------------------------------------------------------------
// Aplicando física no carro
//----------------------------------------------------------------------------------

static void applyDragForce(Car *car) { // Atualiza a força de atrito
    for (int i = 0; i < TRACK_AREA_SIZE; i++) {
        if (Color_equals(car->floorColor, TRACK_AREAS[i].color)) {
            car->dragForce = TRACK_AREAS[i].dragForce;
            return;
        }
    }
}

// Atualiza a posição com base na velocidade e no ângulo
static void applyMovementPhysics(Car *car) {
    car->vel *= powf(car->dragForce, deltaTime * 60.0f);
    car->pos.x += cosf(car->angle) * car->vel * deltaTime * 60.0f;
    car->pos.y += sinf(car->angle) * car->vel * deltaTime * 60.0f;
}

//----------------------------------------------------------------------------------
// Funções de alteração das propriedades físicas do carro
//----------------------------------------------------------------------------------

static void accelerate(Car *car, float force) { // Acelera o carro
    car->vel += car->acc * force * deltaTime * 60.0f;
}

static void breakSpeed(Car *car, float force) { // Freiar
    car->vel *= 1.0f - (1.0f - car->breakForce) * force * deltaTime * 60.0f;
}

// Verificar se está acima da velocidade mínima (em módulo) para fazer a curva
static bool canTurn(Car *car) {
    return fabs(car->vel) > car->minTurnSpeed;
}

static void turn(Car *car, float angle) {
    // Calcule um fator interpolado baseado na velocidade relativa
    float speedRatio = car->vel / car->maxVelocity;

    // Interpola linearmente entre maxSensitivity e minSensitivity
    float steeringSensitivity =
        car->maxAngularSpeed - (car->maxAngularSpeed - car->minAngularSpeed) * speedRatio;

    // Aplica o ângulo com sensibilidade ajustada
    car->angle += angle * steeringSensitivity * deltaTime * 60.0f;
}

static void turnLeft(Car *car) { // Virar para a esquerda
    turn(car, -car->angularSpeed);
}

static void turnRight(Car *car) { // Virar para a direita
    turn(car, car->angularSpeed);
}

static void reverse(Car *car) { // Marcha ré
    car->vel -= car->acc * car->reverseForce * deltaTime * 60.0f;
}

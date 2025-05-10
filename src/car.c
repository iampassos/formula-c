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
                   float ouside_track_drag) {
    TRACK_DRAG             = 1 - track_drag;
    LIGHT_ESCAPE_AREA_DRAG = 1 - light_escape_area_drag;
    HARD_ESCAPE_AREA_DRAG  = 1 - hard_escape_area_drag;
    OUTSIDE_TRACK_DRAG     = 1 - ouside_track_drag;
}

void Track_setMask(Image track_mask) {
    IMAGE_WIDTH = track_mask.width;
    IMAGE_HEIGHT = track_mask.height;
    TRACK_PIXELS = LoadImageColors(track_mask);
}

void Track_setColor(Color track, Color light_escape, Color hard_escape, Color outside,
                    Color race_start, Color first_check, Color second_check) {
    TRACK_COLOR             = track;
    LIGHT_ESCAPE_AREA_COLOR = light_escape;
    HARD_ESCAPE_AREA_COLOR  = hard_escape;
    OUTSIDE_TRACK_COLOR     = outside;
    RACE_START_COLOR        = race_start;
    FIRST_CHECKPOINT_COLOR  = first_check;
    SECOND_CHECKPOINT_COLOR = second_check;
}

void Track_Unload() {
    UnloadImageColors(TRACK_PIXELS);
}

Car *Car_create(Vector2 pos, float acc, int width, int height, Color color, float angle,
                float angularAcc, float minTurnSpeed, float breakCoeficient, float reverseForce,
                int id) {
    Car *car = (Car *) malloc(sizeof(Car));
    if (car == NULL)
        return NULL;
    car->pos          = pos;
    car->acc          = acc;
    car->width        = width;
    car->height       = height;
    car->color        = color;
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

static bool equalsColor(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

static int getCheckpoint(Color color) {
    if (equalsColor(color, RACE_START_COLOR))
        return 0;
    if (equalsColor(color, FIRST_CHECKPOINT_COLOR))
        return 1;
    if (equalsColor(color, SECOND_CHECKPOINT_COLOR))
        return 2;
    return -1;
}

static bool isOnTrack(Color color) {
    return equalsColor(color, TRACK_COLOR);
}

static bool isOnLightEscapeArea(Color color) {
    return equalsColor(color, LIGHT_ESCAPE_AREA_COLOR);
}

static bool isOnHardEscapeArea(Color color) {
    return equalsColor(color, HARD_ESCAPE_AREA_COLOR);
}

static bool isOutSideTrack(Color color) {
    return equalsColor(color, OUTSIDE_TRACK_COLOR);
}

static void Car_updateDragForce(Car *car, Color floorColor) {
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

static int Car_checkCheckpoint(Car *car, Color floorColor) {
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

static void Car_applyPhysics(Car *car) {
    car->vel *= car->dragForce;
    car->pos.x += cos(car->angle) * car->vel;
    car->pos.y += sin(car->angle) * car->vel;
}

static Color Car_getFloor(Car *car) {
    int x = (int)(car->pos.x + cos(car->angle) * car->width * 0.8f);
    int y = (int)(car->pos.y + sin(car->angle) * car->height * 0.5f);
    if (x < 0 || x >= IMAGE_WIDTH || y < 0 || y >= IMAGE_HEIGHT)
        return (Color) {0, 0, 0};
    return TRACK_PIXELS[y * IMAGE_WIDTH + x];
}

static void Car_accelerate(Car *car) {
    car->vel += car->acc;
}

static bool canTurn(Car *car) {
    return car->vel > car->minTurnSpeed || car->vel < -car->minTurnSpeed;
}

static void Car_turn(Car *car, float angle) {
    if (canTurn(car))
        car->angle += angle;
}

static void Car_turnLeft(Car *car) {
    Car_turn(car, -car->angularAcc);
}

static void Car_turnRight(Car *car) {
    Car_turn(car, car->angularAcc);
}

static void Car_break(Car *car) {
    car->vel *= car->breakForce;
}

static void Car_reverse(Car *car) {
    car->vel -= car->acc * car->reverseForce;
}

void Car_update(Car *car) {
    Color floorColor = Car_getFloor(car);

    if (Car_checkCheckpoint(car, floorColor) == -1) {
        Car_updateDragForce(car, floorColor);
    }

    Car_applyPhysics(car);
}

void Car_draw(Car *car) {
    Rectangle rect   = {car->pos.x, car->pos.y, car->width, car->height};
    Vector2   origin = {car->width * 0.2f, car->height * 0.5f};
    DrawRectanglePro(rect, origin, car->angle * RAD2DEG, car->color);
}


void Car_free(Car *car) {
    free(car);
    // Caso precise liberar mais memória
}

void Car_move(Car *car, int up, int down, int right, int left) {
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

#include "car.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float TRACK_DRAG;
float LIGHT_ESCAPE_AREA_DRAG;
float HARD_ESCAPE_AREA_DRAG;
float OUTSIDE_TRACK_DRAG;

Color TRACK_COLOR;
Color LIGHT_ESCAPE_AREA_COLOR;
Color HARD_ESCAPE_AREA_COLOR;
Color OUTSIDE_TRACK_COLOR;

Color RACE_START_COLOR;
Color FIRST_CHECKPOINT_COLOR;
Color SECOND_CHECKPOINT_COLOR;

Image  Car_trackMask;
Color *Car_trackPixels;

void Track_setDrag(float track_drag, float light_escape_area_drag, float hard_escape_area_drag,
                   float ouside_track_drag) {
    TRACK_DRAG             = 1 - track_drag;
    LIGHT_ESCAPE_AREA_DRAG = 1 - light_escape_area_drag;
    HARD_ESCAPE_AREA_DRAG  = 1 - hard_escape_area_drag;
    OUTSIDE_TRACK_DRAG     = 1 - ouside_track_drag;
}

void Track_setMask(Image trackMask, Color *trackPixels) {
    Car_trackMask   = trackMask;
    Car_trackPixels = trackPixels;
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
    UnloadImage(Car_trackMask);
    UnloadImageColors(Car_trackPixels);
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
    car->lapTime      = GetTime();
    car->raceTime     = GetTime();
    car->bestLapTime  = DBL_MAX;
    car->checkpoint   = 2;
    return car;
}

void Car_free(Car *car) {
    free(car);
    // Caso precise liberar mais memória
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
    if (checkpoint >= 0 && ((car->checkpoint + 1) % 3 == checkpoint)) {
        car->checkpoint = checkpoint;
        if (checkpoint == 0) {
            car->lap++;
            car->lapTime = GetTime() - car->lapTime;
            if (car->lapTime < car->bestLapTime)
                car->bestLapTime = car->lapTime;
            car->lapTime = GetTime();
        }
    }
    return checkpoint;
}

static void Car_applyPhysics(Car *car) {
    car->vel *= car->dragForce;
    car->pos.x += cos(car->angle) * car->vel;
    car->pos.y += sin(car->angle) * car->vel;
}

void Car_update(Car *car) {
    Color floorColor = Car_getFloor(car, Car_trackMask, Car_trackPixels);

    if (Car_checkCheckpoint(car, floorColor) == -1) {
        Car_updateDragForce(car, floorColor);
    }

    Car_applyPhysics(car);
}

void Car_accelerate(Car *car) {
    car->vel += car->acc;
}

void Car_draw(Car *car) {
    Rectangle rect   = {car->pos.x, car->pos.y, car->width, car->height};
    Vector2   origin = {car->width * 0.2f, car->height * 0.5f};
    DrawRectanglePro(rect, origin, car->angle * RAD2DEG, car->color);
}

static bool canTurn(Car *car) {
    return car->vel > car->minTurnSpeed || car->vel < -car->minTurnSpeed;
}

void Car_turn(Car *car, float angle) {
    if (canTurn(car))
        car->angle += angle;
}

void Car_turnLeft(Car *car) {
    Car_turn(car, -car->angularAcc);
}

void Car_turnRight(Car *car) {
    Car_turn(car, car->angularAcc);
}

void Car_break(Car *car) {
    car->vel *= car->breakForce;
}

void Car_reverse(Car *car) {
    car->vel -= car->acc * car->reverseForce;
}

Color Car_getFloor(Car *car, Image Car_trackMask, Color *Car_trackPixels) {
    int x = (int) car->pos.x + cos(car->angle) * car->width * 0.8f;
    int y = (int) car->pos.y + sin(car->angle) * car->height * 0.5f;
    if (x < 0 || x >= Car_trackMask.width || y < 0 || y >= Car_trackMask.height)
        return (Color) {0, 0, 0};
    return Car_trackPixels[y * Car_trackMask.width + x];
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

void Car_setPos(Car *car, Vector2 newPos) {
    car->pos = newPos;
}

void Car_setAngle(Car *car, float angle) {
    car->angle = angle;
}

void Car_info(Car *car) {
    printf("Carro id: %d\n"
           "lapTime: %.2f\n"
           "pos.x: %.2f, pos.y: %.2f\n"
           "vel: %.2f\n"
           "acc: %.2f\n"
           "width: %d\n"
           "height: %d\n"
           "color: (r: %d, g: %d, b: %d, a: %d)\n"
           "angle: %.2f\n"
           "angularAcc: %.2f\n"
           "minTurnSpeed: %.2f\n"
           "breakForce: %.2f\n"
           "dragForce: %.2f\n",
           car->id, car->lapTime, car->pos.x, car->pos.y, car->vel, car->acc, car->width,
           car->height, car->color.r, car->color.g, car->color.b, car->color.a, car->angle,
           car->angularAcc, car->minTurnSpeed, car->breakForce, car->dragForce);
}

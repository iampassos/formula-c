#include "car.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float TRACK_DRAG = 0.01;
float LIGHT_ESCAPE_AREA_DRAG = 0.05;
float HARD_ESCAPE_AREA_DRAG = 0.1;

void Car_setDrag(float track_drag, float light_escape_area_drag, float hard_escape_area_drag){
    TRACK_DRAG = track_drag;
    LIGHT_ESCAPE_AREA_DRAG = light_escape_area_drag;
    HARD_ESCAPE_AREA_DRAG = hard_escape_area_drag;
}

Car* Car_create(Vector2 pos,float vel, float acc, int width, int height, Color color, float angle, float angularAcc, float minTurnSpeed, float breakCoeficient, float dragCoeficient, int id) {
    Car* car = (Car*)malloc(sizeof(Car));
    if (car == NULL) return NULL;
    car->pos = pos;
    car->vel = vel;
    car->acc = acc;
    car->width = width;
    car->height = height;
    car->color = color;
    car->angle = angle;
    car->angularAcc = angularAcc;
    car->minTurnSpeed = minTurnSpeed;
    car->breakForce = 1 - breakCoeficient;
    car->dragForce = 1 - dragCoeficient;
    car->id = id;
    car->vel = 0;
    car->lapTime = -1.0f;
    return car;
}

void Car_free(Car* car){
    free(car);
}

static bool isOnTrack(Color color){
    return color.r == 127 && color.g == 127 && color.b == 127;
}

static static bool isOnLightEscapeArea(Color color){
    return color.r == 255 && color.g == 127 && color.b == 39;
}

static bool isOnHardEscapeArea(Color color){
    return color.r == 163 && color.g == 73 && color.b == 164;
}

void Car_update(Car *car){
    Color floorColor = Car_getFloor(car);
    if (isOnTrack(floorColor))
        car->dragForce = TRACK_DRAG;
    else if (isOnLightEscapeArea(floorColor))
        car->dragForce = LIGHT_ESCAPE_AREA_DRAG;
    else if (isOnHardEscapeArea(floorColor))
        car->dragForce = HARD_ESCAPE_AREA_DRAG;
    car->vel *= car->dragForce;
    car->pos.x += cos(car->angle) * car->vel;
    car->pos.y += sin(car->angle) * car->vel;
}

void Car_accelerate(Car *car){
    car->vel += car->acc; 
}

void Car_draw(Car *car){
    Rectangle rect = {car->pos.x, car->pos.y, car->width, car->height};
    Vector2 origin = {car->width * 0.2f, car->height * 0.5f};
    DrawRectanglePro(rect, origin, car->angle * RAD2DEG, car->color);
}

static bool canTurn(Car* car) {
    return car->vel > car->minTurnSpeed || car->vel < -car->minTurnSpeed;
}

void Car_turn(Car *car, float angle){
    if (canTurn(car))
        car->angle += angle;
}

void Car_turnLeft(Car *car){
    Car_turn(car, -car->angularAcc);
}

void Car_turnRight(Car *car){
    Car_turn(car, car->angularAcc);
}

void Car_break(Car *car){
    car->vel *= car->breakForce;
}

void Car_reverse(Car *car){
    car->vel -= car->acc * 0.2;
}

Color Car_getFloor(Car* car, Image trackMask, Color *trackPixels) {
    int x = (int)car->pos.x;
    int y = (int)car->pos.y;
    if (x < 0 || x >= trackMask.width || y < 0 || y >= trackMask.height)
        return (Color){0, 0, 0};
    return trackPixels[y * trackMask.width + x];
}

void Car_move(Car* car, int up, int down, int right, int left){
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

void Car_info(Car* car) {
    printf(
        "Carro id: %d\n"
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
        car->id,
        car->lapTime,
        car->pos.x, car->pos.y,
        car->vel,
        car->acc,
        car->width,
        car->height,
        car->color.r, car->color.g, car->color.b, car->color.a,
        car->angle,
        car->angularAcc,
        car->minTurnSpeed,
        car->breakForce,
        car->dragForce
    );
}
#include "car.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float TRACK_DRAG;
float LIGHT_ESCAPE_AREA_DRAG;
float HARD_ESCAPE_AREA_DRAG;
float OUTSIDE_TRACK_DRAG;

Color TRACK_COLOR;
Color LIGHT_ESCAPE_AREA_COLOR;
Color HARD_ESCAPE_AREA_COLOR;
Color OUTSIDE_TRACK_COLOR;

Image Car_trackMask;
Color *Car_trackPixels;

void Track_setDrag(float track_drag, float light_escape_area_drag, float hard_escape_area_drag, float ouside_track_drag){
    TRACK_DRAG = track_drag;
    LIGHT_ESCAPE_AREA_DRAG = light_escape_area_drag;
    HARD_ESCAPE_AREA_DRAG = hard_escape_area_drag;
    OUTSIDE_TRACK_DRAG = ouside_track_drag;
}

void Track_setMask(Image trackMask, Color *trackPixels){
    Car_trackMask = trackMask;
    Car_trackPixels = trackPixels;
}

void Track_setColor(Color track, Color light_escape, Color hard_escape, Color outside){
    TRACK_COLOR = track;
    LIGHT_ESCAPE_AREA_COLOR = light_escape;
    HARD_ESCAPE_AREA_COLOR = hard_escape;
    OUTSIDE_TRACK_COLOR = outside;
}

void Track_Unload(){
    UnloadImage(Car_trackMask);
    UnloadImageColors(Car_trackPixels);
}

Car* Car_create(Vector2 pos, float acc, int width, int height, Color color, float angle, float angularAcc, float minTurnSpeed, float breakCoeficient, int id) {
    Car* car = (Car*)malloc(sizeof(Car));
    if (car == NULL) return NULL;
    car->pos = pos;
    car->acc = acc;
    car->width = width;
    car->height = height;
    car->color = color;
    car->angle = angle;
    car->angularAcc = angularAcc;
    car->minTurnSpeed = minTurnSpeed;
    car->breakForce = 1 - breakCoeficient;
    car->dragForce = 0;
    car->id = id;
    car->vel = 0;
    car->lapTime = -1.0f;
    return car;
}

void Car_free(Car* car){
    free(car);
    // Caso precise liberar mais memória
}

static bool isOnTrack(Color color){
    return color.r == TRACK_COLOR.r && color.g == TRACK_COLOR.g && color.b == TRACK_COLOR.b;
}

static bool isOnLightEscapeArea(Color color){
    return color.r == LIGHT_ESCAPE_AREA_COLOR.r && color.g == LIGHT_ESCAPE_AREA_COLOR.g && color.b == LIGHT_ESCAPE_AREA_COLOR.b;
}

static bool isOnHardEscapeArea(Color color){
    return color.r == HARD_ESCAPE_AREA_COLOR.r && color.g == HARD_ESCAPE_AREA_COLOR.g && color.b == HARD_ESCAPE_AREA_COLOR.b;
}

static bool isOutSideTrack(Color color){
    return color.r == OUTSIDE_TRACK_COLOR.r && color.g == OUTSIDE_TRACK_COLOR.g && color.b == OUTSIDE_TRACK_COLOR.b;
}

void Car_update(Car *car){
    Color floorColor = Car_getFloor(car,Car_trackMask,Car_trackPixels);
    if (isOnTrack(floorColor))
        car->dragForce = TRACK_DRAG;
    else if (isOnLightEscapeArea(floorColor))
        car->dragForce = LIGHT_ESCAPE_AREA_DRAG;
    else if (isOnHardEscapeArea(floorColor))
        car->dragForce = HARD_ESCAPE_AREA_DRAG;
    else if (isOutSideTrack(floorColor))
        car->dragForce = OUTSIDE_TRACK_DRAG;
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

Color Car_getFloor(Car* car, Image Car_trackMask, Color *Car_trackPixels) {
    int x = (int)car->pos.x + cos(car->angle) * car->width * 0.8f;
    int y = (int)car->pos.y + sin(car->angle) * car->height * 0.5f;
    if (x < 0 || x >= Car_trackMask.width || y < 0 || y >= Car_trackMask.height)
        return (Color){0, 0, 0};
    return Car_trackPixels[y * Car_trackMask.width + x];
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

void Car_setPos(Car* car, Vector2 newPos){
    car->pos = newPos;
}

void Car_setAngle(Car* car, float angle){
    car->angle = angle;
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
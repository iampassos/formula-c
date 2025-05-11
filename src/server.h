// server.h
#ifndef SERVER_H
#define SERVER_H

#include "car.h"

typedef struct {
    int     id;
    int     lap;
    double  startLapTime;
    double  bestLapTime;
    double  raceTime;
    int     checkpoint;
    Vector2 pos;
    float   vel;
    float   angle;
    float   dragForce;
} Car_data_transfer;

typedef struct {
    int length;
    Car* data;
} Car_list_transfer;

void Server_Init();
void Server_addCar(Car car);

void Server_UpdateCar(Car_data_transfer carData);

void Server_exit();

Car Server_GetCarById(int id);

Car_data_transfer Server_GetCarDataById(int id);

Car_list_transfer Server_GetNearCars(int id);

void Server_forEachCar(void (*function)(Car *));

#endif

// server.c
#include "server.h"
#include "car.h"
#include "linked_list.h"
#include "raylib.h"
#include <stdlib.h>

static LinkedList *cars;

void Server_Init() {
    cars = LinkedList_create();
}

void Server_addCar(Car car) {
    Car *playerCar = Car_createEmpty();
    *playerCar     = car;
    LinkedList_addCar(cars, playerCar);
}

void Server_UpdateCar(Car_data_transfer carData) {
    Car *car          = LinkedList_getCarById(cars, carData.id);
    car->id           = carData.id;
    car->lap          = carData.lap;
    car->startLapTime = carData.startLapTime;
    car->bestLapTime  = carData.bestLapTime;
    car->raceTime     = carData.raceTime;
    car->checkpoint   = carData.checkpoint;
    car->pos          = carData.pos;
    car->vel          = carData.vel;
    car->angle        = carData.angle;
    car->dragForce    = carData.dragForce;
    Car_update(car);
}

void Server_exit() {
    LinkedList_free(cars);
}

Car Server_GetCarById(int id) {
    return *LinkedList_getCarById(cars, id);
}

Car_data_transfer Server_GetCarDataById(int id) {
    Car *car = LinkedList_getCarById(cars, id);
    return (Car_data_transfer) {car->id,       car->lap,        car->startLapTime, car->bestLapTime,
                                car->raceTime, car->checkpoint, car->pos,          car->vel,
                                car->angle,    car->dragForce};
}

Car_list_transfer Server_GetNearCars(int id){
    int size = LinkedList_size(cars);
    Car* arr = (Car*)malloc(sizeof(Car) * size);
    Node* cur = cars->head;
    for (int i = 0; i < size; i++){
        arr[i] = *cur->car;
        cur = cur->next;
    }
    return (Car_list_transfer){size, arr};
}

void Server_forEachCar(void (*function)(Car *)) {
    LinkedList_forEach(cars, function);
}
// server.c
#include "server.h"
#include "client.h"

static LinkedList* cars;

void Server_Init() {
    cars = LinkedList_create();
    Car* player = Car_create(
        (Vector2){5400, 2000},     // pos
        2.66,                      // angulo inicial do carro

        0.3,                       // aceleracao do carro
        0.2,                       // força da marcha ré
        0.02,                      // força de frenagem

        0.035,                     // aceleração angular (velocidade de rotação)
        0.2,                       // velocidade mínima para fazer curva

        100,                       // largura
        50,                        // altura

        "resources/cars/carroazul.png", // path da textura
        1                          // id do carro
    );
    LinkedList_addCar(cars, player);
}

void Server_UpdateCar(Car_data_transfer carData) {
    Car* car = LinkedList_getCarById(cars, carData.id);
    car->id = carData.id;
    car->lap = carData.lap;
    car->startLapTime = carData.startLapTime;
    car->bestLapTime = carData.bestLapTime;
    car->raceTime = carData.raceTime;
    car->checkpoint = carData.checkpoint;
    car->pos = carData.pos;
    car->vel = carData.vel;
    car->angle = carData.angle;
    car->dragForce = carData.dragForce;
    Car_update(car);
    //LinkedList_forEach(cars, Car_update);
}

void Server_exit(){
    LinkedList_free(cars);
}

Car Server_GetCarById(int id){
    return *LinkedList_getCarById(cars, id);
}

Car_data_transfer Server_GetCarDataById(int id){
    Car* car = LinkedList_getCarById(cars, id);
    return (Car_data_transfer){
        car->id,
        car->lap,
        car->startLapTime,
        car->bestLapTime,
        car->raceTime,
        car->checkpoint,
        car->pos,
        car->vel,
        car->angle,
        car->dragForce
    };
}

void Server_forEachCar(void (*function)(Car*)){
    LinkedList_forEach(cars, function);
}
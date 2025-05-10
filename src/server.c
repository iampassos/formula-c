// server.c
#include "server.h"

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

void Server_Update() {
    LinkedList_forEach(cars, Car_update);
}

void Server_exit(){
    LinkedList_free(cars);
}

Car* Server_GetCarById(int id){
    return LinkedList_getCarById(cars, id);
}

void Server_forEachCar(void (*function)(Car*)){
    LinkedList_forEach(cars, function);
}

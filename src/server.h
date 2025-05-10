// server.h
#ifndef SERVER_H
#define SERVER_H

#include "linked_list.h"
#include "car.h"

void Server_Init();
void Server_Update();

void Server_exit();

Car* Server_GetCarById(int id);

void Server_forEachCar(void (*function)(Car*));

#endif

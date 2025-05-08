#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

typedef struct {
    int id;
    float lapTime;
    Vector2 pos;
    float vel;
    float acc;
    int width;
    int height;
    Color color;
    float angle;
    float angularAcc;
} Car;

typedef struct Node {
    Car car;
    struct Node* next;
    struct Node* previous;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int length;
} LinkedList;

LinkedList* LinkedList_create(); // Cria uma lista duplamente encadeada
void LinkedList_free(LinkedList* list); // Libera a memória da lista encadeada

int LinkedList_addLast(LinkedList* list, Car car);
int LinkedList_addFirst(LinkedList* list, Car car);
Car LinkedList_removeLast(LinkedList* list);
Car LinkedList_removeFirst(LinkedList* list); 
Car LinkedList_removeId(LinkedList* list, int id); // Remove um carro pelo id

Car LinkedList_search(LinkedList* list, int id); // Procura um carro e retorna (Caso não haja retorna um carro com id = 0)

int LinkedList_size(LinkedList* list); // Retorna o tamanho da lista
void LinkedList_print(LinkedList* list); // Printa os carros da lista pelo ID

void LinkedList_sort(LinkedList* list); // Ordena pelo tempo da volta

#endif
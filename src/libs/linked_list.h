#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "car.h"

// Definindo uma função que possui apenas Car* car como variável
typedef void (*Car_function)(Car *);

// Função de comparação que será usada no LinkedList_sort()
typedef float (*Car_compare)(Car *, Car *);

typedef struct Node {
    Car         *car;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
} LinkedList;

// Cria uma lista encadeada
LinkedList *LinkedList_create();

// Libera a memória da lista encadeada
void LinkedList_free(LinkedList *list);

// Limpa todos os elementos da lista encadeada
void LinkedList_clear(LinkedList *list);

// Adiciona um carro no começo da lista encadeada
void LinkedList_addCar(LinkedList *list, Car *car);

// Procura um carro e retorna (Caso não haja retorna NULL)
Car *LinkedList_getCarById(LinkedList *list, int id);

// Ordena pelo critério definido pela função Car_compare
void LinkedList_sort(LinkedList *list, Car_compare function);

// Aplica uma função para todos os carros da lista
void LinkedList_forEach(LinkedList *list, Car_function function);

#endif

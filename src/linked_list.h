#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "car.h"

typedef struct Node {
    Car         *car;
    struct Node *next;
    struct Node *previous;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int   length;
} LinkedList;

LinkedList *LinkedList_create();               // Cria uma lista duplamente encadeada
void        LinkedList_free(LinkedList *list); // Libera a memória da lista encadeada

void LinkedList_clear(LinkedList *list);

int  LinkedList_addLast(LinkedList *list, Car *car);
int  LinkedList_addFirst(LinkedList *list, Car *car);
Car *LinkedList_removeLast(LinkedList *list);
Car *LinkedList_removeFirst(LinkedList *list);
Car *LinkedList_removeId(LinkedList *list, int id); // Remove um carro pelo id

Car *LinkedList_peakFirst(LinkedList *list); // Retorna o primeiro carro da lista
Car *LinkedList_peakLast(LinkedList *list);  // Retorna o último carro da lista

Car *LinkedList_search(LinkedList *list,
                       int         id); // Procura um carro e retorna (Caso não haja retorna NULL)

int  LinkedList_size(LinkedList *list);  // Retorna o tamanho da lista
void LinkedList_print(LinkedList *list); // Printa os carros da lista pelo ID

void LinkedList_sort(LinkedList *list); // Ordena pelo tempo da volta

void LinkedList_forEach(LinkedList *list,
                        void (*function)(Car *)); // Itera por cada carro e aplica uma função

#endif

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "car.h"

typedef void (*Car_function)(Car*); // Definindo uma função que possui apenas Car* car como variável
typedef float (*Car_compare)(Car*, Car*); // Função de comparação que será usada no LinkedList_sort()

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

void LinkedList_clear(LinkedList *list);    // Limpa todos os elementos da lista encadeada

int  LinkedList_addLast(LinkedList *list, Car *car); // Adiciona um carro no fim da lista encadeada
int  LinkedList_addFirst(LinkedList *list, Car *car); // Adiciona um carro no começo da lista encadeada
Car *LinkedList_removeLast(LinkedList *list); // Remove um carro no fim da lista encadeada
Car *LinkedList_removeFirst(LinkedList *list); // Remove um carro no fim da lista encadeada
Car *LinkedList_removeId(LinkedList *list, int id); // Remove um carro pelo id

Car *LinkedList_peakFirst(LinkedList *list); // Retorna o primeiro carro da lista
Car *LinkedList_peakLast(LinkedList *list);  // Retorna o último carro da lista

Car *LinkedList_search(LinkedList *list,
                       int         id); // Procura um carro e retorna (Caso não haja retorna NULL)

int  LinkedList_size(LinkedList *list);  // Retorna o tamanho da lista
void LinkedList_print(LinkedList *list); // Printa os carros da lista pelo ID

void LinkedList_sort(LinkedList *list, Car_compare function); // Ordena pelo critério definido pela função Car_compare

void LinkedList_forEach(LinkedList *list,
                        Car_function function); // Itera por cada carro e aplica uma função que possui apenas Car* car como variável

#endif
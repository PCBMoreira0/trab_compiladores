#pragma once

#include "libs/afn.h"

typedef struct Node {
    AFN_State *data;
    struct Node* next;
} Node;

// Cria um novo nó
Node* create_node(AFN_State *data);

// Insere no início da lista
void insert_front(Node** head, AFN_State *data);

// Remove o primeiro nó com o valor dado
void remove_node(Node** head, AFN_State *data);

// Imprime a lista
void print_list(Node* head);

// Libera a memória da lista
void free_list(Node* head);
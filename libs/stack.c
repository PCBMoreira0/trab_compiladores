#include <stdlib.h>
#include "stack.h"

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

struct Stack {
    Node* top;
};

Stack* stack_create() {
    Stack* s = (Stack*) malloc(sizeof(Stack));
    s->top = NULL;
    return s;
}

void stack_push(Stack* s, void* data) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->data = data;
    node->next = s->top;
    s->top = node;
}

void* stack_pop(Stack* s) {
    if (s->top == NULL) return NULL;

    Node* temp = s->top;
    void* data = temp->data;

    s->top = temp->next;
    free(temp);

    return data;
}

void* stack_peek(Stack* s) {
    if (s->top == NULL) return NULL;
    return s->top->data;
}

void stack_destroy(Stack* s) {
    while (s->top != NULL) {
        Node* temp = s->top;
        s->top = temp->next;
        free(temp);
    }
    free(s);
}
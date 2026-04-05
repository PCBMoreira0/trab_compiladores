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
    if (s) s->top = NULL;
    return s;
}

void stack_push(Stack* s, void* data) {
    if (!s) return;
    Node* node = (Node*) malloc(sizeof(Node));
    node->data = data;
    node->next = s->top;
    s->top = node;
}

void* stack_pop(Stack* s) {
    if (s == NULL || s->top == NULL) return NULL;

    Node* temp = s->top;
    void* data = temp->data;

    s->top = temp->next;
    free(temp);

    return data;
}

void* stack_peek(Stack* s) {
    if (s == NULL || s->top == NULL) return NULL;
    return s->top->data;
}

int stack_is_empty(Stack* s) {
    return (s == NULL || s->top == NULL);
}

void stack_destroy(Stack* s) {
    if (!s) return;
    while (s->top != NULL) {
        Node* temp = s->top;
        s->top = temp->next;
        free(temp);
    }
    free(s);
}
#pragma once

#include <stddef.h>

typedef struct Stack Stack;

Stack* stack_create();
void stack_destroy(Stack* s);

void stack_push(Stack* s, void* data);
void* stack_pop(Stack* s);
void* stack_peek(Stack* s);

int stack_is_empty(Stack* s);

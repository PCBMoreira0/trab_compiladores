#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct ValueStack ValueStack;

ValueStack* value_stack_create(size_t element_size);
void value_stack_push(ValueStack* s, const void* value_ptr);
bool value_stack_pop(ValueStack* s, void* out_data);
void* value_stack_peek(ValueStack* s);
int value_stack_is_empty(ValueStack* s);
void value_stack_destroy(ValueStack* s);

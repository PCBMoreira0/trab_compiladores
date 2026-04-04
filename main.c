#include <stdio.h>
#include "libs/afn.h"
#include "libs/stack.h"

int main() {
    char *er = "ab|";

    Stack *stack = stack_create();

    for(int i = 0; er[i] != '\0'; i++){
        switch (er[i])
        {
        case 'a' || 'b':
            AFN_State *symbol = afnCreateSymbol(er[i]);
            stack_push(stack, &symbol);
            break;
        
        case '|':
            AFN_State *b = *(AFN_State**)stack_pop(stack);
            AFN_State *a = *(AFN_State**)stack_pop(stack);
            AFN_State *unionAB = afnCreateUnion(a, b);
            stack_push(stack, &unionAB);
            break;
        default:
            break;
        }
    }

    afnPrint(*(AFN_State**)stack_pop(stack), 0);
    
    afnFree(*(AFN_State**)stack_pop(stack));

    return 0;
}
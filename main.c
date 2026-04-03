#include <stdio.h>
#include "libs/afn.h"

int main() {
    AFN_State *a = afnCreateSymbol('a');
    AFN_State *b = afnCreateSymbol('b');
    AFN_State *unionAB = afnCreateUnion(a, b);
    
    // for(Node *state = unionAB->list; state != NULL; state = state->list) {
    //     printf("Transition character: %c, Is end state: %d\n", state->transitionChar, state->isEndState);
    // }
    return 0;
}
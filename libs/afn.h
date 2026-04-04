#pragma once

typedef struct AFN_State AFN_State;

AFN_State *afnNewState(int isEndState);
AFN_State *afnAddTransition(AFN_State *from, AFN_State *to, char character);
AFN_State *afnCreateSymbol(char character);
AFN_State *afnCreateUnion(AFN_State *a, AFN_State *b);
AFN_State *afnCreateKleene(AFN_State *a);
void afnFree(AFN_State *state);
void afnPrint(AFN_State *state, int depth);
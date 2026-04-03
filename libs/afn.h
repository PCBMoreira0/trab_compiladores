#pragma once

typedef struct AFN_State AFN_State;

AFN_State *afnNewEmptyState(int isEndState);
AFN_State *afnNewState(char transitionChar, int isEndState);
AFN_State *afnAddTransition(AFN_State *from, AFN_State *to);
AFN_State *afnCreateSymbol(char character);
AFN_State *afnCreateUnion(AFN_State *a, AFN_State *b);
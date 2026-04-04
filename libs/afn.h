#pragma once

#define MAX_TRANSITIONS 10
#define EMPTY_STATE_CHAR '\0'
#define EPSILON_CHAR '\0'

typedef struct AFN_State AFN_State;

typedef struct Node {
    char transitionChar;
    AFN_State *to;
    struct Node *next;
} Node;

typedef struct AFN_State {
    int id;
    int isEndState;
    Node *transitions; 
} AFN_State;

typedef struct {
    AFN_State *start;
    AFN_State *end;
} AFN_Fragment;

AFN_State* afnNewState(int isEndState);
void afnPrint(AFN_State *start);
void afnFree(AFN_State *start);
void afnAddTransition(AFN_State *from, char transitionChar, AFN_State *to);
AFN_Fragment afnCreateSymbol(char character);
AFN_Fragment afnCreateUnion(AFN_Fragment a, AFN_Fragment b);
AFN_Fragment afnCreateConcat(AFN_Fragment a, AFN_Fragment b);
AFN_Fragment afnCreateKleene(AFN_Fragment a);
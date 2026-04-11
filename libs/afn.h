#pragma once

#include "linked_list.h"
#include <stdio.h>
#include "regular_expression.h"

#define MAX_TRANSITIONS 10
#define EMPTY_STATE_CHAR '\0'
#define EPSILON_CHAR '\0'

typedef struct AFN_State AFN_State;

typedef struct {
    char transitionChar;
    AFN_State *to;
} AFN_Transition;

struct AFN_State {
    int id;
    int isEndState;
    ERToken token;
    LinkedList *transitions;
};

typedef struct {
    AFN_State *start;
    AFN_State *end;
} AFN_Fragment;

typedef struct {
    int state_count;             
    int capacity;
    AFN_State **allocated_states;
} AFN_Context;

AFN_Context* afnCreateContext();
void afnFreeContext(AFN_Context *ctx);

AFN_State* afnNewState(AFN_Context *ctx, int isEndState);
void afnPrint(AFN_Context *ctx, AFN_State *start);
void afnFree(AFN_State *start);
void afnAddTransition(AFN_State *from, char transitionChar, AFN_State *to);
AFN_Fragment afnUnify(AFN_Context *ctx, AFN_Fragment *list, int listSize);

LinkedList* afnMove(AFN_Context *ctx, LinkedList *current_set, char symbol);
LinkedList* afnEpsilonClosure(AFN_Context *ctx, LinkedList *current_set);

AFN_Fragment afnCreateSymbol(AFN_Context *ctx, char character);
AFN_Fragment afnCreateUnion(AFN_Context *ctx, AFN_Fragment a, AFN_Fragment b);
AFN_Fragment afnCreateConcat(AFN_Context *ctx, AFN_Fragment a, AFN_Fragment b);
AFN_Fragment afnCreateKleene(AFN_Context *ctx, AFN_Fragment a);
int afnBuildFromER(AFN_Context *ctx, ERToken token, AFN_Fragment *out_fragment);
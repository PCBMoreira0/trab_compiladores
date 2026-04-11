#include <stdlib.h>
#include "afn.h"
#include "linked_list.h"

#include "value_stack.h"

#define CONCAT_OPERATOR '&'
#define UNION_OPERATOR '|'
#define KLEENE_OPERATOR '^'
#define PLUS_KLEENE_OPERATOR '+'

/* ==========================================
 * GERENCIAMENTO DE CONTEXTO
 * ========================================== */

AFN_Context* afnCreateContext() {
    AFN_Context *ctx = (AFN_Context *)malloc(sizeof(AFN_Context));
    if (!ctx) return NULL;
    
    ctx->state_count = 0;
    ctx->capacity = 16;
    ctx->allocated_states = (AFN_State **)malloc(ctx->capacity * sizeof(AFN_State*));
    
    return ctx;
}

void afnFreeContext(AFN_Context *ctx) {
    if (!ctx) return;

    for (int i = 0; i < ctx->state_count; i++) {
        AFN_State *s = ctx->allocated_states[i];
        freeList(s->transitions);
        
        free(s);
    }
    free(ctx->allocated_states);
    free(ctx);
}

/* ==========================================
 * CRIAÇÃO DE ESTADOS E TRANSIÇÕES
 * ========================================== */

AFN_State* afnNewState(AFN_Context *ctx, int isEndState) {
    AFN_State *newState = (AFN_State *)malloc(sizeof(AFN_State));
    if (!newState) return NULL;

    newState->id = ctx->state_count; 
    newState->isEndState = isEndState;
    newState->token = (ERToken){.value = "", .type = TOKEN_EMPTY, .priority = 0};
    newState->transitions = createList();

    if (ctx->state_count >= ctx->capacity) {
        int new_capacity = ctx->capacity * 2;
        AFN_State **temp = (AFN_State **)realloc(
            ctx->allocated_states, 
            new_capacity * sizeof(AFN_State*)
        );
        if (!temp) {
            free(newState); 
            return NULL;
        }
        ctx->allocated_states = temp;
        ctx->capacity = new_capacity;
    }

    ctx->allocated_states[ctx->state_count] = newState;
    ctx->state_count++;

    return newState;
}

void afnAddTransition(AFN_State *from, char transitionChar, AFN_State *to) {
    AFN_Transition trans;
    trans.transitionChar = transitionChar;
    trans.to = to;

    from->transitions = insertAtHead(from->transitions, &trans, sizeof(AFN_Transition));
}

/* ==========================================
 * CONSTRUTORES DO ALGORITMO DE THOMPSON
 * ========================================== */

AFN_Fragment afnCreateSymbol(AFN_Context *ctx, char character) {
    AFN_State *start = afnNewState(ctx, 0);
    AFN_State *end = afnNewState(ctx, 1);

    afnAddTransition(start, character, end);

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnCreateUnion(AFN_Context *ctx, AFN_Fragment a, AFN_Fragment b) {
    AFN_State *start = afnNewState(ctx, 0);
    AFN_State *end = afnNewState(ctx, 1);

    afnAddTransition(start, EPSILON_CHAR, a.start);
    afnAddTransition(start, EPSILON_CHAR, b.start);

    afnAddTransition(a.end, EPSILON_CHAR, end);
    afnAddTransition(b.end, EPSILON_CHAR, end);

    a.end->isEndState = 0;
    b.end->isEndState = 0;

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnCreateConcat(AFN_Context *ctx, AFN_Fragment a, AFN_Fragment b) {
    afnAddTransition(a.end, EPSILON_CHAR, b.start);
    a.end->isEndState = 0;

    AFN_Fragment frag = {a.start, b.end};
    return frag;
}

AFN_Fragment afnCreateKleene(AFN_Context *ctx, AFN_Fragment a) {
    AFN_State *start = afnNewState(ctx, 0);
    AFN_State *end = afnNewState(ctx, 1);

    afnAddTransition(start, EPSILON_CHAR, a.start);
    afnAddTransition(start, EPSILON_CHAR, end);
    afnAddTransition(a.end, EPSILON_CHAR, a.start);
    afnAddTransition(a.end, EPSILON_CHAR, end);
    a.end->isEndState = 0;

    AFN_Fragment frag = {start, end};
    return frag;
}

AFN_Fragment afnUnify(AFN_Context *ctx, AFN_Fragment *list, int listSize){
    AFN_State *start = afnNewState(ctx, 0);

    for (int i = 0; i < listSize; i++) {
        afnAddTransition(start, EPSILON_CHAR, list[i].start);
    }

    AFN_Fragment frag = {start, NULL};
    return frag;
}

/* ==========================================
 * POWER SET CONSTRUCTION (AFN -> AFD)
 * ========================================== */

LinkedList* afnMove(AFN_Context *ctx, LinkedList *current_set, char symbol){
    LinkedList *result_set = createList();
    int *added = (int *)calloc(ctx->state_count, sizeof(int));

    LinkedList *curr_state_node = current_set;
    while (curr_state_node) {
        AFN_State *s = *(AFN_State **)curr_state_node->data;
        
        LinkedList *trans_node = s->transitions;
        while (trans_node) {
            AFN_Transition *trans = (AFN_Transition *)trans_node->data;
            
            if (trans->transitionChar == symbol) {
                AFN_State *dest = trans->to;
                
                if (!added[dest->id]) {
                    added[dest->id] = 1; 
                    result_set = insertAtHead(result_set, &dest, sizeof(AFN_State*));
                }
            }
            trans_node = trans_node->next;
        }
        curr_state_node = curr_state_node->next;
    }
    
    free(added);
    return result_set;
}

LinkedList* afnEpsilonClosure(AFN_Context *ctx, LinkedList *current_set) {
    LinkedList *result_set = createList();
    int *added = (int *)calloc(ctx->state_count, sizeof(int));
    LinkedList *stack = createList();
    LinkedList *curr = current_set;

    while (curr) {
        AFN_State *s = *(AFN_State **)curr->data;
        
        if (!added[s->id]) {
            added[s->id] = 1;
            result_set = insertAtHead(result_set, &s, sizeof(AFN_State*));
            stack = insertAtHead(stack, &s, sizeof(AFN_State*));
        }
        curr = curr->next;
    }

    while (stack != NULL) {
        AFN_State *s = *(AFN_State **)stack->data;
        LinkedList *node_to_free = stack;
        stack = stack->next;
        free(node_to_free->data);
        free(node_to_free);
        LinkedList *trans_node = s->transitions;
        while (trans_node) {
            AFN_Transition *trans = (AFN_Transition *)trans_node->data;
            if (trans->transitionChar == EPSILON_CHAR) {
                AFN_State *dest = trans->to;
                
                if (!added[dest->id]) {
                    added[dest->id] = 1;
                    result_set = insertAtHead(result_set, &dest, sizeof(AFN_State*));
                    stack = insertAtHead(stack, &dest, sizeof(AFN_State*));
                }
            }
            trans_node = trans_node->next;
        }
    }

    free(added);
    return result_set;
}

/* ==========================================
 * UTILITÁRIOS (DEBUG)
 * ========================================== */

static void afnPrintHelper(AFN_State *state, int *visited) {
    if (!state || visited[state->id]) return;
    
    visited[state->id] = 1;
    printf("Estado %d %s:\n", state->id, state->isEndState ? "[FINAL]" : "");
    
    LinkedList *curr = state->transitions;
    if (!curr) {
        printf("  (sem transições)\n");
    }
    
    while (curr) {
        AFN_Transition *trans = (AFN_Transition *)curr->data;
        char c = (trans->transitionChar == EPSILON_CHAR) ? 'E' : trans->transitionChar;
        printf("  --(%c)--> Estado %d\n", c, trans->to->id);
        curr = curr->next;
    }
    
    curr = state->transitions;
    while (curr) {
        AFN_Transition *trans = (AFN_Transition *)curr->data;
        afnPrintHelper(trans->to, visited);
        curr = curr->next;
    }
}

void afnPrint(AFN_Context *ctx, AFN_State *start) {
    if (!start || !ctx) return;
    printf("--- ESTRUTURA DO AFN ---\n");

    int *visited = (int *)calloc(ctx->state_count, sizeof(int));
    afnPrintHelper(start, visited);
    free(visited);
    
    printf("------------------------\n\n");
}

int afnBuildFromER(AFN_Context *ctx, ERToken token, AFN_Fragment *out_fragment) {
    ValueStack *stack = value_stack_create(sizeof(AFN_Fragment));
    char *regex = token.value;
    for(int i = 0; regex[i] != '\0'; i++) {
        char c = regex[i];
        
        switch(c){
            case UNION_OPERATOR:
                AFN_Fragment frag_union1, frag_union2;
                value_stack_pop(stack, &frag_union2);
                value_stack_pop(stack, &frag_union1);
                AFN_Fragment union_afn = afnCreateUnion(ctx, frag_union2, frag_union1);
                value_stack_push(stack, &union_afn);
                break;
            case KLEENE_OPERATOR:
                AFN_Fragment frag_kleene;
                value_stack_pop(stack, &frag_kleene);
                AFN_Fragment kleene_afn = afnCreateKleene(ctx, frag_kleene);
                value_stack_push(stack, &kleene_afn);
                break;
            case CONCAT_OPERATOR:
                AFN_Fragment frag_concat1, frag_concat2;
                value_stack_pop(stack, &frag_concat2);
                value_stack_pop(stack, &frag_concat1);
                AFN_Fragment concat_afn = afnCreateConcat(ctx, frag_concat1, frag_concat2);
                value_stack_push(stack, &concat_afn);
                break;
            case PLUS_KLEENE_OPERATOR:
                AFN_Fragment frag_plus;
                value_stack_pop(stack, &frag_plus);
                AFN_Fragment kleene_plus_afn = afnCreateKleene(ctx, frag_plus);
                AFN_Fragment concat_plus_afn = afnCreateConcat(ctx, frag_plus, kleene_plus_afn);
                value_stack_push(stack, &concat_plus_afn);
                break;
            default:
                AFN_Fragment frag_symbol = afnCreateSymbol(ctx, c);
                value_stack_push(stack, &frag_symbol);
                break;
        }
    }
    
    AFN_Fragment result;
    value_stack_pop(stack, &result);
    int isEmpty = value_stack_is_empty(stack);
    value_stack_destroy(stack);
    if(!isEmpty) {
        return 0; // ER inválida, mais de um fragmento restante
    }
    
    result.end->token = token;
    *out_fragment = result;
    return 1; // Retornar o fragmento do AFN construído
}
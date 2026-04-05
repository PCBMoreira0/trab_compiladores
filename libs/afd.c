#include <stdlib.h>
#include <string.h>
#include "afd.h"
#include "linked_list.h"
#include "stack.h"

typedef struct {
    int dfa_state_id;
    LinkedList *nfa_states;
} WorkItem;

int run_dfa(Automato_AFD *dfa, const char *text) {
    int current_state = dfa->initial_state;
    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = text[i];
        current_state = dfa->transitions[current_state][c]; 
        if (current_state == STATE_DEAD) {
            return 0; 
        }
    }
    return dfa->is_final[current_state];
}

int* afdCreateSignature(AFN_Context *ctx, LinkedList *set) {
    int *signature = (int *)calloc(ctx->state_count, sizeof(int));
    if (!signature) return NULL;

    LinkedList *curr = set;
    while (curr) {
        AFN_State *s = *(AFN_State **)curr->data;
        signature[s->id] = 1;
        curr = curr->next;
    }

    return signature;
}

int afdFindSignatureIndex(AFN_Context *ctx, int *new_signature, int **known_signatures, int known_count) {
    size_t memory_size = ctx->state_count * sizeof(int);

    for (int i = 0; i < known_count; i++) {
        if (memcmp(new_signature, known_signatures[i], memory_size) == 0) {
            return i;
        }
    }

    return -1;
}

Automato_AFD* afdBuild(AFN_Context *ctx, AFN_State *start_nfa_state) {
    if (!ctx || !start_nfa_state) return NULL;

    Automato_AFD *dfa = (Automato_AFD *)malloc(sizeof(Automato_AFD));
    int capacity = 16;
    dfa->num_states = 0;
    dfa->initial_state = 0;
    dfa->is_final = (int *)calloc(capacity, sizeof(int));
    dfa->transitions = (int **)malloc(capacity * sizeof(int*));
    
    int **known_signatures = (int **)malloc(capacity * sizeof(int*));

    LinkedList *start_list = createList();
    start_list = insertAtHead(start_list, &start_nfa_state, sizeof(AFN_State*));
    
    LinkedList *s0_set = afnEpsilonClosure(ctx, start_list);
    freeList(start_list);

    int *s0_sig = afdCreateSignature(ctx, s0_set);
    known_signatures[0] = s0_sig;
    dfa->num_states = 1;
    
    dfa->transitions[0] = (int *)malloc(ALPHABET_SIZE * sizeof(int));
    for (int i = 0; i < ALPHABET_SIZE; i++) dfa->transitions[0][i] = STATE_DEAD;

    Stack *stack = stack_create();
    
    WorkItem *w0 = (WorkItem *)malloc(sizeof(WorkItem));
    w0->dfa_state_id = 0;
    w0->nfa_states = s0_set;
    stack_push(stack, w0);

    while (!stack_is_empty(stack)) {
        WorkItem *item = (WorkItem *)stack_pop(stack);
        int current_id = item->dfa_state_id;
        LinkedList *current_set = item->nfa_states;

        LinkedList *curr_nfa = current_set;
        while (curr_nfa) {
            AFN_State *s = *(AFN_State **)curr_nfa->data;
            if (s->isEndState) {
                dfa->is_final[current_id] = 1;
                break; 
            }
            curr_nfa = curr_nfa->next;
        }

        for (int c = 1; c < ALPHABET_SIZE; c++) {
            
            LinkedList *move_set = afnMove(ctx, current_set, c);
            if (!move_set) continue;

            LinkedList *closure_set = afnEpsilonClosure(ctx, move_set);
            freeList(move_set); 

            if (!closure_set) continue;

            int *sig = afdCreateSignature(ctx, closure_set);
            int target_id = afdFindSignatureIndex(ctx, sig, known_signatures, dfa->num_states);

            if (target_id == -1) {
                target_id = dfa->num_states++;
                
                if (dfa->num_states > capacity) {
                    capacity *= 2;
                    dfa->is_final = (int *)realloc(dfa->is_final, capacity * sizeof(int));
                    dfa->transitions = (int **)realloc(dfa->transitions, capacity * sizeof(int*));
                    known_signatures = (int **)realloc(known_signatures, capacity * sizeof(int*));
                }
                
                dfa->is_final[target_id] = 0;
                known_signatures[target_id] = sig;
                dfa->transitions[target_id] = (int *)malloc(ALPHABET_SIZE * sizeof(int));
                for (int i = 0; i < ALPHABET_SIZE; i++) dfa->transitions[target_id][i] = STATE_DEAD;

                WorkItem *w_new = (WorkItem *)malloc(sizeof(WorkItem));
                w_new->dfa_state_id = target_id;
                w_new->nfa_states = closure_set;
                stack_push(stack, w_new);
                
            } else {
                free(sig);
                freeList(closure_set);
            }
            dfa->transitions[current_id][c] = target_id;
        }

        freeList(current_set);
        free(item);
    }

    stack_destroy(stack);
    for (int i = 0; i < dfa->num_states; i++) {
        free(known_signatures[i]);
    }
    free(known_signatures);

    return dfa;
}

void afdFree(Automato_AFD *dfa) {
    if (!dfa) return;
    
    for (int i = 0; i < dfa->num_states; i++) {
        free(dfa->transitions[i]);
    }
    free(dfa->transitions);
    free(dfa->is_final);
    free(dfa);
}
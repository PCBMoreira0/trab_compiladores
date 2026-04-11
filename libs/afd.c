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


void afdPrint(Automato_AFD *dfa) {
    if (!dfa) return;

    printf("AFD:\n");
    printf("Estado Inicial: %d\n", dfa->initial_state);
    printf("Estados Finais: ");
    for (int i = 0; i < dfa->num_states; i++) {
        if (dfa->is_final[i]) {
            printf("%d ", i);
        }
    }
    printf("\nTransições:\n");
    for (int i = 0; i < dfa->num_states; i++) {
        for (int c = 0; c < ALPHABET_SIZE; c++) {
            if (dfa->transitions[i][c] != STATE_DEAD) {
                printf("  Estado %d --%c--> Estado %d\n", i, c, dfa->transitions[i][c]);
            }
        }
    }
}


Automato_AFD *afdMinimize(Automato_AFD *afd){
    // Inicialização
    int **table = malloc(sizeof(int *) * (afd->num_states + 1)); // + 1 para criar o estado morto
    for(int i = 0; i < afd->num_states + 1; i++){
        table[i] = calloc(afd->num_states + 1, sizeof(int));
    }

    // Marcando estados finais e não finais (estado morto nunca é final)
    int tableDeadIndex = afd->num_states;
    for(int i = 0; i < afd->num_states + 1; i++){
        for(int j = 0; j < afd->num_states + 1; j++){
            if(i == j || i > j) continue;

            if(i == tableDeadIndex && j == tableDeadIndex) continue;

            if(i == tableDeadIndex){
                if(afd->is_final[j]){
                    table[i][j] = 1;
                }
            }else if(j == tableDeadIndex){
                if(afd->is_final[i]){
                    table[i][j] = 1;
                }
            }else{
                if(afd->is_final[i] != afd->is_final[j]){
                    table[i][j] = 1;
                }
            }
        }
    }

    // Marcando estados distinguíveis adicionais
    int marked = 1;
    while(marked){
        marked = 0;
        for(int i = 0; i < afd->num_states; i++){
            for(int j = 0; j < afd->num_states; j++){
                if(i == j || i > j) continue;

                int isMarked = table[i][j];

                if(!isMarked){
                    for(int c = 0; c < ALPHABET_SIZE; c++){

                        int tState1 = afd->transitions[i][c];
                        int tState2 = afd->transitions[j][c];

                        if(tState1 == STATE_DEAD) tState1 = tableDeadIndex;
                        if(tState2 == STATE_DEAD) tState2 = tableDeadIndex;

                        int tStateIsMarked;
                        if(tState1 > tState2) tStateIsMarked = table[tState2][tState1];
                        else tStateIsMarked = table[tState1][tState2];

                        if(tStateIsMarked){
                            table[i][j] = 1;
                            marked = 1;
                        }
                    }
                }
            }
        }
    }

    // Combinando os estados restantes
    int *visited = calloc(afd->num_states, sizeof(int));
    int *combined = malloc(sizeof(int) * afd->num_states);

    int stateCount = 0;
    for(int i = 0; i < afd->num_states; i++){
        int marked = 0;
        for(int j = 0; j < afd->num_states; j++){
            if(i == j || i > j) continue;

            
            if(!table[i][j]){
                if(visited[i]){
                    combined[j] = combined[i];
                    visited[j] = 1;
                }
                else{
                    combined[i] = stateCount;
                    combined[j] = stateCount;
                    visited[i] = 1;
                    visited[j] = 1;
                    stateCount++;
                }
                marked = 1;
            }
        }

        if(!marked && !visited[i]){
            combined[i] = stateCount++;
            visited[i] = 1;
        }
    }

    free(visited);

    for(int i = 0; i < afd->num_states; i++){
        free(table[i]);
    }
    free(table);

    // Criando nova tabela de transição
    int **transitions = malloc(sizeof(int *) * afd->num_states);
    for(int i = 0; i < afd->num_states; i++){
        transitions[i] = malloc(ALPHABET_SIZE * sizeof(int));
    }

    for(int i = 0; i < afd->num_states; i++){
        for(int j = 0; j < ALPHABET_SIZE; j++){
            transitions[i][j] = STATE_DEAD;
        }
    }

    int *isFinal = calloc(stateCount, sizeof(int));

    for(int i = 0; i < afd->num_states; i++){
        for(int j = 0; j < ALPHABET_SIZE; j++){
            if(afd->transitions[i][j] == STATE_DEAD) continue;
            transitions[combined[i]][j] = combined[afd->transitions[i][j]];
        }

        isFinal[combined[i]] = afd->is_final[i];
    }

    Automato_AFD *newAfd = malloc(sizeof(Automato_AFD));
    newAfd->initial_state = combined[afd->initial_state];
    newAfd->num_states = stateCount;
    newAfd->transitions = transitions;
    newAfd->is_final = isFinal;

    free(combined);
    return newAfd;
}
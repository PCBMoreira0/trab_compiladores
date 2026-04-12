#include "stack.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>

int getOpPrecedence(char op){
    switch (op)
    {
    case '^':
    case '+':
        return 2;
        break;
    case '&':
        return 1;
        break;
    case '|':
        return 0;
        break;
    default:
        return -1;
        break;
    }
}

int isOperator(char ex){
    if(ex == '^' || ex == '&' || ex == '+' || ex == '|'){
        return 1;
    }
    else{
        return 0;
    }
}

char *shuntingYard(const char *expression){
    Stack *operatorStack = stack_create();
    Queue *outputQueue = queue_create();

    for(int i = 0; expression[i] != '\0'; i++){   
        if(expression[i] == '('){
            stack_push(operatorStack, &expression[i]);
        }
        else if(expression[i] == ')'){
            while(!stack_is_empty(operatorStack)){
                char *op = (char *) stack_pop(operatorStack);
                if(*op == '(') break;

                queue_enqueue(outputQueue, op);
            }
        }
        else if(isOperator(expression[i]))
        {
            if(!stack_is_empty(operatorStack)){
                char peek = *(char *) stack_peek(operatorStack);
                if(peek == '('){
                    stack_push(operatorStack, &expression[i]);
                    continue;
                }
                while(getOpPrecedence(expression[i]) <= getOpPrecedence(peek)){
                    char *op = (char *) stack_pop(operatorStack);
                    queue_enqueue(outputQueue, op);  
                    if(!stack_is_empty(operatorStack)){
                        peek = *(char *) stack_peek(operatorStack);
                    }
                    else{
                        break;
                    }
                }
            }

            stack_push(operatorStack, &expression[i]);
        }
        else{
            queue_enqueue(outputQueue, &expression[i]);
        }
    }

    while(!stack_is_empty(operatorStack)){
        char *op = (char *) stack_pop(operatorStack);
        queue_enqueue(outputQueue, op);
    }

    stack_destroy(operatorStack);
    
    char *outputString = malloc(sizeof(char) * (queue_size(outputQueue) + 1));
    int count = 0;
    while(!queue_is_empty(outputQueue)){
        char c = *(char *) queue_dequeue(outputQueue);
        outputString[count++] = c;
    }
    outputString[count] = '\0';

    queue_destroy(outputQueue);

    return outputString;
}

char* expandIntervals(const char* er) {
    size_t capacidade = strlen(er) * 10 + 10; 
    char* resultado = (char*)malloc(capacidade);
    if (!resultado) return NULL;

    int j = 0;
    for (int i = 0; er[i] != '\0'; i++) {
        if (er[i] == '[' && 
            er[i+1] != '\0' && 
            er[i+2] == '-' && 
            er[i+3] != '\0' && 
            er[i+4] == ']') {
            
            char inicio = er[i+1];
            char fim = er[i+3];

            resultado[j++] = '(';
            
            for (char c = inicio; c <= fim; c++) {
                resultado[j++] = c;
                if (c < fim) {
                    resultado[j++] = '|';
                }
            }
            
            resultado[j++] = ')';
            i += 4; 
        } else {
            resultado[j++] = er[i];
        }
    }
    
    resultado[j] = '\0';
    return resultado;
}

char *ERpreProcess(char *expression){
    char *output = malloc(sizeof(char) * strlen(expression) * 2);
    output[0] = expression[0];
    char lastChar = expression[0];
    int i, j;
    for(i = 1, j = 1; expression[i] != '\0'; i++, j++){
        if(!isOperator(expression[i]) && expression[i] != '(' && expression[i] != ')'){
            if(lastChar == '^' || lastChar == '+' || lastChar == ')'){
                output[j++] = '&';
            }
            else if(!isOperator(lastChar) && lastChar != '(' && lastChar != ')'){
                output[j++] = '&';
            }
        }   
        else if(expression[i] == '('){
            if((!isOperator(lastChar) && lastChar != '(' && lastChar != ')') || lastChar == ')' || lastChar == '+' || lastChar == '^'){
                output[j++] = '&';
            }
        }

        lastChar = expression[i];
        output[j] = expression[i];
    }

    output[j] = '\0';

    char *final_output = shuntingYard(output);
    free(output);
    return final_output;
}
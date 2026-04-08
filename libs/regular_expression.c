#include "stack.h"
#include "queue.h"
#include <stdlib.h>

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

char *shuntingYard(const char *expression, int *outputSize){
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
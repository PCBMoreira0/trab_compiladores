#include "stack.h"
#include "queue.h"
#include "regular_expression.h"
#include <stdlib.h>
#include <string.h>

int getOpPrecedence(char op){
    switch (op)
    {
    case '*':
    case '?':
    case '^':
    case '+':
        return 2;
    case '&':
        return 1;
    case '|':
        return 0;
    default:
        return -1;
    }
}

int isOperator(char ex){
    if(ex == '*' || ex == '?' || ex == '^' || ex == '&' || ex == '+' || ex == '|'){
        return 1;
    }
    return 0;
}

char *shuntingYard(const char *expression){
    Stack *operatorStack = stack_create();
    Queue *outputQueue = queue_create();

    for(int i = 0; expression[i] != '\0'; i++){   
        if(expression[i] == '('){
            stack_push(operatorStack, (void*)&expression[i]);
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
                    stack_push(operatorStack, (void*)&expression[i]);
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
            stack_push(operatorStack, (void*)&expression[i]);
        }
        else{
            queue_enqueue(outputQueue, (void*)&expression[i]);
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

int requiresConcatRight(unsigned char c) {
    if (c > 127) return 1; 
    return (!isOperator(c) && c != '(' && c != ')') || c == ')' || c == '*' || c == '+' || c == '?' || c == '^';
}

int requiresConcatLeft(unsigned char c) {
    if (c > 127) return 1; 
    return (!isOperator(c) && c != '(' && c != ')') || c == '(';
}

char *ERpreProcess(char *expression){
    if (!expression || expression[0] == '\0') return NULL;

    char *output = malloc(sizeof(char) * strlen(expression) * 2 + 1);
    int j = 0;
    
    for(int i = 0; expression[i] != '\0'; i++){
        unsigned char curr = expression[i];
        
        if (curr == '\\' && expression[i+1] != '\0') {
            char next = expression[i+1];
            
            if (next == 'n') { curr = '\n'; i++; }
            else if (next == 't') { curr = '\t'; i++; }
            else if (next == 'r') { curr = '\r'; i++; }
            else {
                curr = next | 0x80; 
                i++; 
            }
        }
        
        if (j > 0) {
            unsigned char prev = output[j-1];
            if (requiresConcatRight(prev) && requiresConcatLeft(curr)) {
                output[j++] = '&';
            }
        }
        
        output[j++] = curr;
    }
    
    output[j] = '\0';

    char *final_output = shuntingYard(output);
    free(output);
    
    return final_output;
}

ERTokenType string_to_token_type(const char *str) {
    if (strcmp(str, "TOKEN_LPAREN") == 0) return TOKEN_LPAREN;
    if (strcmp(str, "TOKEN_RPAREN") == 0) return TOKEN_RPAREN;
    if (strcmp(str, "TOKEN_LBRACKET") == 0) return TOKEN_LBRACKET;
    if (strcmp(str, "TOKEN_RBRACKET") == 0) return TOKEN_RBRACKET;
    if (strcmp(str, "TOKEN_DOT") == 0) return TOKEN_DOT;
    if (strcmp(str, "TOKEN_KW_EXPRESSION") == 0) return TOKEN_KW_EXPRESSION;
    if (strcmp(str, "TOKEN_KW_MODULE_STAR") == 0) return TOKEN_KW_MODULE_STAR;
    if (strcmp(str, "TOKEN_KW_MODULE") == 0) return TOKEN_KW_MODULE;
    if (strcmp(str, "TOKEN_KW_PLAIN_MOD_BEGIN") == 0) return TOKEN_KW_PLAIN_MOD_BEGIN;
    if (strcmp(str, "TOKEN_KW_BEGIN_FOR_SYNTAX") == 0) return TOKEN_KW_BEGIN_FOR_SYNTAX;
    if (strcmp(str, "TOKEN_KW_BEGIN0") == 0) return TOKEN_KW_BEGIN0;
    if (strcmp(str, "TOKEN_KW_BEGIN") == 0) return TOKEN_KW_BEGIN;
    if (strcmp(str, "TOKEN_KW_PROVIDE") == 0) return TOKEN_KW_PROVIDE;
    if (strcmp(str, "TOKEN_KW_DECLARE") == 0) return TOKEN_KW_DECLARE;
    if (strcmp(str, "TOKEN_KW_DEFINE_VALUES") == 0) return TOKEN_KW_DEFINE_VALUES;
    if (strcmp(str, "TOKEN_KW_DEFINE_SYNTAXES") == 0) return TOKEN_KW_DEFINE_SYNTAXES;
    if (strcmp(str, "TOKEN_KW_REQUIRE") == 0) return TOKEN_KW_REQUIRE;
    if (strcmp(str, "TOKEN_KW_PLAIN_LAMBDA") == 0) return TOKEN_KW_PLAIN_LAMBDA;
    if (strcmp(str, "TOKEN_KW_CASE_LAMBDA") == 0) return TOKEN_KW_CASE_LAMBDA;
    if (strcmp(str, "TOKEN_KW_IF") == 0) return TOKEN_KW_IF;
    if (strcmp(str, "TOKEN_KW_LETREC_VALUES") == 0) return TOKEN_KW_LETREC_VALUES;
    if (strcmp(str, "TOKEN_KW_LET_VALUES") == 0) return TOKEN_KW_LET_VALUES;
    if (strcmp(str, "TOKEN_KW_SET") == 0) return TOKEN_KW_SET;
    if (strcmp(str, "TOKEN_KW_QUOTE_SYNTAX") == 0) return TOKEN_KW_QUOTE_SYNTAX;
    if (strcmp(str, "TOKEN_KW_QUOTE") == 0) return TOKEN_KW_QUOTE;
    if (strcmp(str, "TOKEN_KW_LOCAL") == 0) return TOKEN_KW_LOCAL;
    if (strcmp(str, "TOKEN_KW_WITH_CONT_MARK") == 0) return TOKEN_KW_WITH_CONT_MARK;
    if (strcmp(str, "TOKEN_KW_PLAIN_APP") == 0) return TOKEN_KW_PLAIN_APP;
    if (strcmp(str, "TOKEN_KW_TOP") == 0) return TOKEN_KW_TOP;
    if (strcmp(str, "TOKEN_KW_VAR_REF") == 0) return TOKEN_KW_VAR_REF;
    if (strcmp(str, "TOKEN_FALSE") == 0) return TOKEN_FALSE;
    if (strcmp(str, "TOKEN_TRUE") == 0) return TOKEN_TRUE;
    if (strcmp(str, "TOKEN_ID") == 0) return TOKEN_ID;
    if (strcmp(str, "TOKEN_STRING") == 0) return TOKEN_STRING;
    if (strcmp(str, "TOKEN_FLOAT") == 0) return TOKEN_FLOAT;
    if (strcmp(str, "TOKEN_INT") == 0) return TOKEN_INT;
    if (strcmp(str, "TOKEN_WHITESPACE") == 0) return TOKEN_WHITESPACE;

    return TOKEN_EMPTY;
}
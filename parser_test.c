#include <stdio.h>
#include "parser/parser.h"
#include "libs/lex_parsing.h"
// Resolve o warning: passing argument 2 of 'print_tree' from incompatible pointer type (expected const char * int) but argument is of type const char* ERTokenType
// enum e int vai ter o mesmo valor mas para o compilador são tipos diferentes quando usados em ponteiros de função.
static const char *token_name_int(int t){
    return get_token_name((ERTokenType)t);
}

int main()
{
    TokenList *tokens = load_token_list_from_file("output.tks");
    char *errors[100];
    size_t error_count;
    TreeNode *root = parse(tokens, errors, &error_count);

    if (error_count > 0)
    {
        printf("Erros de Sintaxe encontrados:\n");
        for (size_t i = 0; i < error_count; i++)
        {
            printf("%s", errors[i]);
        }
    }
    else
    {

        printf("Árvore Sintática (AST):\n");
        print_tree(root, token_name_int);
    }

    free_tree(root);
    return 0;
}
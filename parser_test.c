#include <stdio.h>
#include "parser/parser.h"
#include "libs/lex_parsing.h"

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
        print_tree(root, get_token_name);
    }

    free_tree(root);
    return 0;
}
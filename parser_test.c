#include <stdio.h>
#include "parser/parser.h"

const char *token_type_to_string(int typeInt)
{
    if (typeInt < 1000)
    {
        ERTokenType type = (ERTokenType)typeInt;
        switch (type)
        {
        case TOKEN_EMPTY:
            return "TOKEN_EMPTY";
        case TOKEN_LPAREN:
            return "TOKEN_LPAREN";
        case TOKEN_RPAREN:
            return "TOKEN_RPAREN";
        case TOKEN_LBRACKET:
            return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:
            return "TOKEN_RBRACKET";
        case TOKEN_DOT:
            return "TOKEN_DOT";

        case TOKEN_KW_EXPRESSION:
            return "TOKEN_KW_EXPRESSION";
        case TOKEN_KW_MODULE_STAR:
            return "TOKEN_KW_MODULE_STAR";
        case TOKEN_KW_MODULE:
            return "TOKEN_KW_MODULE";
        case TOKEN_KW_PLAIN_MOD_BEGIN:
            return "TOKEN_KW_PLAIN_MOD_BEGIN";
        case TOKEN_KW_BEGIN_FOR_SYNTAX:
            return "TOKEN_KW_BEGIN_FOR_SYNTAX";
        case TOKEN_KW_BEGIN0:
            return "TOKEN_KW_BEGIN0";
        case TOKEN_KW_BEGIN:
            return "TOKEN_KW_BEGIN";

        case TOKEN_KW_PROVIDE:
            return "TOKEN_KW_PROVIDE";
        case TOKEN_KW_DECLARE:
            return "TOKEN_KW_DECLARE";
        case TOKEN_KW_DEFINE_VALUES:
            return "TOKEN_KW_DEFINE_VALUES";
        case TOKEN_KW_DEFINE_SYNTAXES:
            return "TOKEN_KW_DEFINE_SYNTAXES";
        case TOKEN_KW_REQUIRE:
            return "TOKEN_KW_REQUIRE";

        case TOKEN_KW_PLAIN_LAMBDA:
            return "TOKEN_KW_PLAIN_LAMBDA";
        case TOKEN_KW_CASE_LAMBDA:
            return "TOKEN_KW_CASE_LAMBDA";
        case TOKEN_KW_IF:
            return "TOKEN_KW_IF";
        case TOKEN_KW_LETREC_VALUES:
            return "TOKEN_KW_LETREC_VALUES";
        case TOKEN_KW_LET_VALUES:
            return "TOKEN_KW_LET_VALUES";
        case TOKEN_KW_SET:
            return "TOKEN_KW_SET";

        case TOKEN_KW_QUOTE_SYNTAX:
            return "TOKEN_KW_QUOTE_SYNTAX";
        case TOKEN_KW_QUOTE:
            return "TOKEN_KW_QUOTE";
        case TOKEN_KW_LOCAL:
            return "TOKEN_KW_LOCAL";
        case TOKEN_KW_WITH_CONT_MARK:
            return "TOKEN_KW_WITH_CONT_MARK";

        case TOKEN_KW_PLAIN_APP:
            return "TOKEN_KW_PLAIN_APP";
        case TOKEN_KW_TOP:
            return "TOKEN_KW_TOP";
        case TOKEN_KW_VAR_REF:
            return "TOKEN_KW_VAR_REF";

        case TOKEN_FALSE:
            return "TOKEN_FALSE";
        case TOKEN_TRUE:
            return "TOKEN_TRUE";

        case TOKEN_ID:
            return "TOKEN_ID";
        case TOKEN_STRING:
            return "TOKEN_STRING";
        case TOKEN_FLOAT:
            return "TOKEN_FLOAT";
        case TOKEN_INT:
            return "TOKEN_INT";
        case TOKEN_WHITESPACE:
            return "TOKEN_WHITESPACE";
        case TOKEN_EOF:
            return "TOKEN_EOF";
        default:
            return "UNKNOWN_TOKEN";
        }
    }
    else
    {
        switch ((AST_Node_Type) typeInt)
        {

        // --- AST nodes ---
        case NODE_TOP_LEVEL:
            return "NODE_TOP_LEVEL";
        case NODE_EXPRESSION:
            return "NODE_EXPRESSION";
        case NODE_MODULE:
            return "NODE_MODULE";
        case NODE_MODULE_STAR:
            return "NODE_MODULE_STAR";
        case NODE_PLAIN_MODULE_BEGIN:
            return "NODE_PLAIN_MODULE_BEGIN";
        case NODE_BEGIN:
            return "NODE_BEGIN";
        case NODE_BEGIN_FOR_SYNTAX:
            return "NODE_BEGIN_FOR_SYNTAX";
        case NODE_PROVIDE:
            return "NODE_PROVIDE";
        case NODE_DECLARE:
            return "NODE_DECLARE";
        case NODE_DEFINE_VALUES:
            return "NODE_DEFINE_VALUES";
        case NODE_DEFINE_SYNTAXES:
            return "NODE_DEFINE_SYNTAXES";
        case NODE_REQUIRE:
            return "NODE_REQUIRE";
        case NODE_PLAIN_LAMBDA:
            return "NODE_PLAIN_LAMBDA";
        case NODE_CASE_LAMBDA:
            return "NODE_CASE_LAMBDA";
        case NODE_IF:
            return "NODE_IF";
        case NODE_BEGIN0:
            return "NODE_BEGIN0";
        case NODE_LET_VALUES:
            return "NODE_LET_VALUES";
        case NODE_LETREC_VALUES:
            return "NODE_LETREC_VALUES";
        case NODE_SET:
            return "NODE_SET";
        case NODE_QUOTE:
            return "NODE_QUOTE";
        case NODE_QUOTE_SYNTAX:
            return "NODE_QUOTE_SYNTAX";
        case NODE_WITH_CONT_MARK:
            return "NODE_WITH_CONT_MARK";
        case NODE_PLAIN_APP:
            return "NODE_PLAIN_APP";
        case NODE_TOP:
            return "NODE_TOP";
        case NODE_VAR_REF:
            return "NODE_VAR_REF";
        case NODE_ID:
            return "NODE_ID";
        case NODE_LITERAL:
            return "NODE_LITERAL";
        case NODE_FORMALS:
            return "NODE_FORMALS";
        case NODE_BINDING:
            return "NODE_BINDING";

        default:
            return "UNKNOWN_NODE";
        }
    }
}

int main()
{
    ERTokenType tokens[32] = {
        TOKEN_LPAREN,    // (
        TOKEN_KW_MODULE, // module
        TOKEN_ID,        // main
        TOKEN_STRING,    // "main.rkt"

        TOKEN_LPAREN,             // (
        TOKEN_KW_PLAIN_MOD_BEGIN, // #%plain-module-begin

        TOKEN_LPAREN,           // (
        TOKEN_KW_DEFINE_VALUES, // define-values

        TOKEN_LPAREN, // (
        TOKEN_ID,     // x
        TOKEN_RPAREN, // )

        TOKEN_LPAREN,   // (
        TOKEN_KW_QUOTE, // quote
        TOKEN_INT,      // 10
        TOKEN_RPAREN,   // )

        TOKEN_RPAREN, // )

        TOKEN_LPAREN,       // (
        TOKEN_KW_PLAIN_APP, // #%plain-app
        TOKEN_ID,           // +
        TOKEN_ID,           // x

        TOKEN_LPAREN,   // (
        TOKEN_KW_QUOTE, // quote
        TOKEN_INT,      // 5
        TOKEN_RPAREN,   // )

        TOKEN_RPAREN, // )
        TOKEN_RPAREN, // )
        TOKEN_RPAREN, // )
        TOKEN_EOF     // EOF
    };

    TreeNode *root = parse("output.tks");
    printf("Árvore Sintática (AST):\n");
    print_tree(root, token_type_to_string);

    free_tree(root);
    return 0;
}
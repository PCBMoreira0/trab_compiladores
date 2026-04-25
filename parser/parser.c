#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <string.h>

typedef struct ParserContext
{
    ERTokenType currentType;
    int currentIndex;
    TokenList *tokens;
    char **errorList;
    int errorCount;
} ParserContext;

void error(ParserContext *context, Token *token, const char *message)
{
    if (context->errorCount >= 100) return;
    char *buffer = malloc(256);
    sprintf(buffer, "Erro de Sintaxe na linha %d coluna %d, lexema '%s': %s\n", token->line, token->column, token->lexeme, message);
    context->errorList[context->errorCount++] = buffer;
}

ERTokenType nextToken(ParserContext *context)
{
    return context->currentType = context->tokens->tokens[++context->currentIndex].type;
}

ERTokenType peekToken(ParserContext *context)
{
    return context->tokens->tokens[context->currentIndex + 1].type;
}

void match(ParserContext *context, ERTokenType expected)
{
    if (context->currentType == expected)
    {
        nextToken(context);
    }
    else
    {
        error(context, &context->tokens->tokens[context->currentIndex], "Token inesperado");
        nextToken(context);
    }
}

// Protótipos
TreeNode *top_level_form(ParserContext *context);
TreeNode *module_level_form(ParserContext *context);
TreeNode *submodule_form(ParserContext *context);
TreeNode *general_top_level_form(ParserContext *context);
TreeNode *expr(ParserContext *context);
TreeNode *formals(ParserContext *context);

// Helper para module-path e datum (simplificados como nós genéricos aqui)
TreeNode *module_path(ParserContext *context)
{
    TreeNode *node = create_node(NODE_LITERAL);
    nextToken(context); // Consome o path (simplificação)
    return node;
}

TreeNode *datum(ParserContext *context)
{
    TreeNode *node = create_node(NODE_LITERAL);
    nextToken(context); // Consome o datum (simplificação)
    return node;
}

// ==========================================
// Regras da BNF
// ==========================================

TreeNode *top_level_form(ParserContext *context)
{
    if (context->currentType == TOKEN_LPAREN)
    {
        ERTokenType lookahead = peekToken(context);

        switch (lookahead)
        {
        case TOKEN_KW_EXPRESSION:
        {
            TreeNode *node = create_node(NODE_EXPRESSION);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_EXPRESSION);
            add_child(node, expr(context));
            match(context, TOKEN_RPAREN);
            return node;
        }
        case TOKEN_KW_MODULE:
        {
            TreeNode *node = create_node(NODE_MODULE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_MODULE);
            add_child(node, create_node(NODE_ID));
            match(context, TOKEN_ID);
            add_child(node, module_path(context));

            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_PLAIN_MOD_BEGIN);
            TreeNode *body = create_node(NODE_PLAIN_MODULE_BEGIN);
            while (context->currentType != TOKEN_RPAREN)
            {
                add_child(body, module_level_form(context));
            }
            match(context, TOKEN_RPAREN);
            add_child(node, body);
            match(context, TOKEN_RPAREN);
            return node;
        }
        case TOKEN_KW_BEGIN:
        {
            TreeNode *node = create_node(NODE_BEGIN);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_BEGIN);
            while (context->currentType != TOKEN_RPAREN)
            {
                add_child(node, top_level_form(context));
            }
            match(context, TOKEN_RPAREN);
            return node;
        }
        case TOKEN_KW_BEGIN_FOR_SYNTAX:
        {
            TreeNode *node = create_node(NODE_BEGIN_FOR_SYNTAX);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_BEGIN_FOR_SYNTAX);
            while (context->currentType != TOKEN_RPAREN)
            {
                add_child(node, top_level_form(context));
            }
            match(context, TOKEN_RPAREN);
            return node;
        }
        default:
            break;
        }
    }
    return general_top_level_form(context);
}

TreeNode *module_level_form(ParserContext *context)
{
    if (context->currentType == TOKEN_LPAREN)
    {
        ERTokenType lookahead = peekToken(context);
        switch (lookahead)
        {
        case TOKEN_KW_PROVIDE:
        {
            TreeNode *node = create_node(NODE_PROVIDE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_PROVIDE);
            while (context->currentType != TOKEN_RPAREN)
                nextToken(context);
            match(context, TOKEN_RPAREN);
            return node;
        }
        case TOKEN_KW_BEGIN_FOR_SYNTAX:
        {
            TreeNode *node = create_node(NODE_BEGIN_FOR_SYNTAX);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_BEGIN_FOR_SYNTAX);
            while (context->currentType != TOKEN_RPAREN)
            {
                add_child(node, module_level_form(context));
            }
            match(context, TOKEN_RPAREN);
            return node;
        }
        case TOKEN_KW_DECLARE:
        {
            TreeNode *node = create_node(NODE_DECLARE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_DECLARE);
            while (context->currentType != TOKEN_RPAREN)
                nextToken(context);
            match(context, TOKEN_RPAREN);
            return node;
        }
        case TOKEN_KW_MODULE:
        case TOKEN_KW_MODULE_STAR:
            return submodule_form(context);
        default:
            break;
        }
    }
    return general_top_level_form(context);
}

TreeNode *submodule_form(ParserContext *context)
{
    TreeNode *node;
    match(context, TOKEN_LPAREN);

    if (context->currentType == TOKEN_KW_MODULE)
    {
        node = create_node(NODE_MODULE);
        match(context, TOKEN_KW_MODULE);
    }
    else
    {
        node = create_node(NODE_MODULE_STAR);
        match(context, TOKEN_KW_MODULE_STAR);
    }

    add_child(node, create_node(NODE_ID));
    match(context, TOKEN_ID);

    if (context->currentType == TOKEN_FALSE)
    {
        add_child(node, create_node(NODE_LITERAL));
        match(context, TOKEN_FALSE);
    }
    else
    {
        add_child(node, module_path(context));
    }

    match(context, TOKEN_LPAREN);
    match(context, TOKEN_KW_PLAIN_MOD_BEGIN);
    TreeNode *body = create_node(NODE_PLAIN_MODULE_BEGIN);
    while (context->currentType != TOKEN_RPAREN)
    {
        add_child(body, module_level_form(context));
    }

    match(context, TOKEN_RPAREN);
    add_child(node, body);
    match(context, TOKEN_RPAREN);
    return node;
}

TreeNode *general_top_level_form(ParserContext *context)
{
    if (context->currentType == TOKEN_LPAREN)
    {
        ERTokenType lookahead = peekToken(context);

        if (lookahead == TOKEN_KW_DEFINE_VALUES || lookahead == TOKEN_KW_DEFINE_SYNTAXES)
        {
            TreeNode *node = create_node(lookahead == TOKEN_KW_DEFINE_VALUES ? NODE_DEFINE_VALUES : NODE_DEFINE_SYNTAXES);
            match(context, TOKEN_LPAREN);
            nextToken(context);

            TreeNode *ids = create_node(NODE_FORMALS);
            match(context, TOKEN_LPAREN);
            while (context->currentType == TOKEN_ID)
            {
                add_child(ids, create_node(NODE_ID));
                match(context, TOKEN_ID);
            }
            match(context, TOKEN_RPAREN);

            add_child(node, ids);
            add_child(node, expr(context));
            match(context, TOKEN_RPAREN);
            return node;
        }
        else if (lookahead == TOKEN_KW_REQUIRE)
        {
            TreeNode *node = create_node(NODE_REQUIRE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_REQUIRE);
            while (context->currentType != TOKEN_RPAREN)
                nextToken(context);
            match(context, TOKEN_RPAREN);
            return node;
        }
    }
    return expr(context);
}

TreeNode *formals(ParserContext *context)
{
    TreeNode *node = create_node(NODE_FORMALS);
    if (context->currentType == TOKEN_ID)
    {
        add_child(node, create_node(NODE_ID));
        match(context, TOKEN_ID);
    }
    else if (context->currentType == TOKEN_LPAREN)
    {
        match(context, TOKEN_LPAREN);
        while (context->currentType == TOKEN_ID)
        {
            add_child(node, create_node(NODE_ID));
            match(context, TOKEN_ID);
        }
        if (context->currentType == TOKEN_DOT)
        {
            match(context, TOKEN_DOT);
            add_child(node, create_node(NODE_ID));
            match(context, TOKEN_ID);
        }
        match(context, TOKEN_RPAREN);
    }
    return node;
}

TreeNode *expr(ParserContext *context)
{
    if (context->currentType == TOKEN_ID)
    {
        TreeNode *node = create_node(NODE_ID);
        match(context, TOKEN_ID);
        return node;
    }

    // Tratamento genérico para literais do Lexer que podem cair como expr
    if (context->currentType == TOKEN_INT || context->currentType == TOKEN_FLOAT ||
        context->currentType == TOKEN_STRING || context->currentType == TOKEN_TRUE ||
        context->currentType == TOKEN_FALSE)
    {
        TreeNode *node = create_node(NODE_LITERAL);
        nextToken(context);
        return node;
    }

    match(context, TOKEN_LPAREN);
    ERTokenType keyword = context->currentType;
    TreeNode *node = NULL;

    switch (keyword)
    {
    case TOKEN_KW_PLAIN_LAMBDA:
        node = create_node(NODE_PLAIN_LAMBDA);
        match(context, TOKEN_KW_PLAIN_LAMBDA);
        add_child(node, formals(context));
        do
        {
            add_child(node, expr(context));
        } while (context->currentType != TOKEN_RPAREN);
        break;

    case TOKEN_KW_CASE_LAMBDA:
        node = create_node(NODE_CASE_LAMBDA);
        match(context, TOKEN_KW_CASE_LAMBDA);
        while (context->currentType == TOKEN_LPAREN)
        {
            TreeNode *clause = create_node(NODE_PLAIN_LAMBDA); // Usamos lambda interno para representar a clausula
            match(context, TOKEN_LPAREN);
            add_child(clause, formals(context));
            do
            {
                add_child(clause, expr(context));
            } while (context->currentType != TOKEN_RPAREN);
            match(context, TOKEN_RPAREN);
            add_child(node, clause);
        }
        break;

    case TOKEN_KW_IF:
        node = create_node(NODE_IF);
        match(context, TOKEN_KW_IF);
        add_child(node, expr(context)); // Cond
        add_child(node, expr(context)); // Then
        add_child(node, expr(context)); // Else
        break;

    case TOKEN_KW_BEGIN:
    case TOKEN_KW_BEGIN0:
        node = create_node(keyword == TOKEN_KW_BEGIN ? NODE_BEGIN : NODE_BEGIN0);
        match(context, keyword);
        do
        {
            add_child(node, expr(context));
        } while (context->currentType != TOKEN_RPAREN);
        break;

    case TOKEN_KW_LET_VALUES:
    case TOKEN_KW_LETREC_VALUES:
        node = create_node(keyword == TOKEN_KW_LET_VALUES ? NODE_LET_VALUES : NODE_LETREC_VALUES);
        match(context, keyword);

        // Parsing dos bindings: ([(id ...) expr] ...)
        match(context, TOKEN_LPAREN);
        while (context->currentType == TOKEN_LBRACKET || context->currentType == TOKEN_LPAREN)
        {
            TreeNode *binding = create_node(NODE_BINDING);
            ERTokenType open_bracket = context->currentType;
            match(context, open_bracket);

            TreeNode *ids = create_node(NODE_FORMALS);
            match(context, TOKEN_LPAREN);
            while (context->currentType == TOKEN_ID)
            {
                add_child(ids, create_node(NODE_ID));
                match(context, TOKEN_ID);
            }
            match(context, TOKEN_RPAREN);

            add_child(binding, ids);
            add_child(binding, expr(context));

            match(context, open_bracket == TOKEN_LBRACKET ? TOKEN_RBRACKET : TOKEN_RPAREN);
            add_child(node, binding);
        }
        match(context, TOKEN_RPAREN);

        // expr ...+
        do
        {
            add_child(node, expr(context));
        } while (context->currentType != TOKEN_RPAREN);
        break;

    case TOKEN_KW_SET:
        node = create_node(NODE_SET);
        match(context, TOKEN_KW_SET);
        add_child(node, create_node(NODE_ID));
        match(context, TOKEN_ID);
        add_child(node, expr(context));
        break;

    case TOKEN_KW_QUOTE:
        node = create_node(NODE_QUOTE);
        match(context, TOKEN_KW_QUOTE);
        add_child(node, datum(context));
        break;

    case TOKEN_KW_QUOTE_SYNTAX:
        node = create_node(NODE_QUOTE_SYNTAX);
        match(context, TOKEN_KW_QUOTE_SYNTAX);
        add_child(node, datum(context));
        if (context->currentType == TOKEN_KW_LOCAL)
        {
            match(context, TOKEN_KW_LOCAL);
        }
        break;

    case TOKEN_KW_WITH_CONT_MARK:
        node = create_node(NODE_WITH_CONT_MARK);
        match(context, TOKEN_KW_WITH_CONT_MARK);
        add_child(node, expr(context));
        add_child(node, expr(context));
        add_child(node, expr(context));
        break;

    case TOKEN_KW_TOP:
        node = create_node(NODE_TOP);
        match(context, TOKEN_KW_TOP);
        match(context, TOKEN_DOT);
        add_child(node, create_node(NODE_ID));
        match(context, TOKEN_ID);
        break;

    case TOKEN_KW_VAR_REF:
        node = create_node(NODE_VAR_REF);
        match(context, TOKEN_KW_VAR_REF);
        if (context->currentType == TOKEN_ID)
        {
            add_child(node, create_node(NODE_ID));
            match(context, TOKEN_ID);
        }
        else if (context->currentType == TOKEN_LPAREN)
        {
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_TOP);
            match(context, TOKEN_DOT);
            TreeNode *top_node = create_node(NODE_TOP);
            add_child(top_node, create_node(NODE_ID));
            match(context, TOKEN_ID);
            add_child(node, top_node);
            match(context, TOKEN_RPAREN);
        }
        break;

    case TOKEN_KW_PLAIN_APP:
        node = create_node(NODE_PLAIN_APP);
        match(context, TOKEN_KW_PLAIN_APP);
        do
        {
            add_child(node, expr(context));
        } while (context->currentType != TOKEN_RPAREN);
        break;

    default:
        // Aplicação normal (quando não tem palavra-chave do Racket, ex: (foo 1 2))
        node = create_node(NODE_PLAIN_APP); // Trata como aplicação
        // Como consumimos o '(' mas não era palavra chave, a currentType ainda é a cabeça da função.
        while (context->currentType != TOKEN_RPAREN)
        {
            add_child(node, expr(context));
        }
        break;
    }

    match(context, TOKEN_RPAREN);
    return node;
}

TreeNode *parse(TokenList *tokens, char **out_errors, size_t *out_size)
{   
    char *errorList[100]; // Suporte para até 100 erros (simplificação)
    ParserContext context = {tokens->tokens[0].type, 0, tokens, errorList, 0};
    TreeNode *root = top_level_form(&context);

    if (context.currentType != TOKEN_EOF)
    {
        error(&context, &context.tokens->tokens[context.currentIndex],
              "Tokens extras apos o final do programa");
    }

    *out_size = context.errorCount;
    for (int i = 0; i < context.errorCount; i++)
    {
        out_errors[i] = context.errorList[i];
    }
    return root;
}
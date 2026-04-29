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
    if (token->type == TOKEN_EOF)
    {
        sprintf(buffer, "Erro de Sintaxe na linha %d coluna %d, no fim do arquivo: %s\n", token->line, token->column, message);
    }
    else
    {
        sprintf(buffer, "Erro de Sintaxe na linha %d coluna %d, em '%s': %s\n", token->line, token->column, token->lexeme, message);
    }
    context->errorList[context->errorCount++] = buffer;
}

ERTokenType nextToken(ParserContext *context)
{
    if (context->currentType == TOKEN_EOF) return TOKEN_EOF;
    return context->currentType = context->tokens->tokens[++context->currentIndex].type;
}

ERTokenType peekToken(ParserContext *context)
{
    if (context->currentType == TOKEN_EOF) return TOKEN_EOF;
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
        error(context, &context->tokens->tokens[context->currentIndex], "construção sintática inválida");
        nextToken(context);
    }
}

void expect(ParserContext *context, ERTokenType expected, const char *message)
{
    if (context->currentType == expected)
    {
        nextToken(context);
    }
    else
    {
        error(context, &context->tokens->tokens[context->currentIndex], message);
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
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após expressão '#%expression'");
            return node;
        }
        case TOKEN_KW_MODULE:
        {
            TreeNode *node = create_node(NODE_MODULE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_MODULE);
            add_child(node, create_node(NODE_ID));
            expect(context, TOKEN_ID, "esperado nome do módulo após 'module'");
            add_child(node, module_path(context));

            expect(context, TOKEN_LPAREN, "esperado abertura de parênteses para o corpo do módulo");
            expect(context, TOKEN_KW_PLAIN_MOD_BEGIN, "esperado '#%plain-module-begin' no início do corpo do módulo");
            TreeNode *body = create_node(NODE_PLAIN_MODULE_BEGIN);
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
            {
                add_child(body, module_level_form(context));
            }
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após corpo de '#%plain-module-begin'");
            add_child(node, body);
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após declaração do módulo");
            return node;
        }
        case TOKEN_KW_BEGIN:
        {
            TreeNode *node = create_node(NODE_BEGIN);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_BEGIN);
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
            {
                add_child(node, top_level_form(context));
            }
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após bloco 'begin'");
            return node;
        }
        case TOKEN_KW_BEGIN_FOR_SYNTAX:
        {
            TreeNode *node = create_node(NODE_BEGIN_FOR_SYNTAX);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_BEGIN_FOR_SYNTAX);
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
            {
                add_child(node, top_level_form(context));
            }
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após bloco 'begin-for-syntax'");
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
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
                nextToken(context);
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após cláusula 'provide'");
            return node;
        }
        case TOKEN_KW_BEGIN_FOR_SYNTAX:
        {
            TreeNode *node = create_node(NODE_BEGIN_FOR_SYNTAX);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_BEGIN_FOR_SYNTAX);
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
            {
                add_child(node, module_level_form(context));
            }
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após bloco 'begin-for-syntax'");
            return node;
        }
        case TOKEN_KW_DECLARE:
        {
            TreeNode *node = create_node(NODE_DECLARE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_DECLARE);
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
                nextToken(context);
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após cláusula 'declare'");
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
    expect(context, TOKEN_ID, "esperado nome do submódulo");

    if (context->currentType == TOKEN_FALSE)
    {
        add_child(node, create_node(NODE_LITERAL));
        match(context, TOKEN_FALSE);
    }
    else
    {
        add_child(node, module_path(context));
    }

    expect(context, TOKEN_LPAREN, "esperado abertura de parênteses para o corpo do submódulo");
    expect(context, TOKEN_KW_PLAIN_MOD_BEGIN, "esperado '#%plain-module-begin' no início do corpo do submódulo");
    TreeNode *body = create_node(NODE_PLAIN_MODULE_BEGIN);
    while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
    {
        add_child(body, module_level_form(context));
    }

    expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após corpo de '#%plain-module-begin'");
    add_child(node, body);
    expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após declaração do submódulo");
    return node;
}

TreeNode *general_top_level_form(ParserContext *context)
{
    if (context->currentType == TOKEN_LPAREN)
    {
        ERTokenType lookahead = peekToken(context);

        if (lookahead == TOKEN_KW_DEFINE_VALUES || lookahead == TOKEN_KW_DEFINE_SYNTAXES)
        {
            int is_values = (lookahead == TOKEN_KW_DEFINE_VALUES);
            TreeNode *node = create_node(is_values ? NODE_DEFINE_VALUES : NODE_DEFINE_SYNTAXES);
            const char *form_name = is_values ? "define-values" : "define-syntaxes";
            match(context, TOKEN_LPAREN);
            nextToken(context);

            TreeNode *ids = create_node(NODE_FORMALS);
            expect(context, TOKEN_LPAREN,
                   is_values
                       ? "esperado abertura de parênteses para a lista de variáveis em 'define-values'"
                       : "esperado abertura de parênteses para a lista de variáveis em 'define-syntaxes'");
            while (context->currentType == TOKEN_ID)
            {
                add_child(ids, create_node(NODE_ID));
                match(context, TOKEN_ID);
            }
            expect(context, TOKEN_RPAREN,
                   is_values
                       ? "esperado fechamento de parênteses após a lista de variáveis de 'define-values'"
                       : "esperado fechamento de parênteses após a lista de variáveis de 'define-syntaxes'");

            add_child(node, ids);
            add_child(node, expr(context));
            expect(context, TOKEN_RPAREN,
                   is_values
                       ? "esperado fechamento de parênteses após expressão de 'define-values'"
                       : "esperado fechamento de parênteses após expressão de 'define-syntaxes'");
            (void)form_name;
            return node;
        }
        else if (lookahead == TOKEN_KW_REQUIRE)
        {
            TreeNode *node = create_node(NODE_REQUIRE);
            match(context, TOKEN_LPAREN);
            match(context, TOKEN_KW_REQUIRE);
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
                nextToken(context);
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após cláusula 'require'");
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
            expect(context, TOKEN_ID, "esperado nome de variável após '.' na lista de parâmetros");
        }
        expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após lista de parâmetros");
    }
    else
    {
        error(context, &context->tokens->tokens[context->currentIndex],
              "esperado nome de variável ou abertura de parênteses para a lista de parâmetros");
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

    if (context->currentType != TOKEN_LPAREN)
    {
        error(context, &context->tokens->tokens[context->currentIndex],
              "esperado início de expressão (variável, número, string ou abertura de parênteses)");
        nextToken(context);
        return create_node(NODE_LITERAL);
    }
    match(context, TOKEN_LPAREN);
    ERTokenType keyword = context->currentType;
    TreeNode *node = NULL;
    const char *close_msg = "esperado fechamento de parênteses ao final da expressão";

    switch (keyword)
    {
    case TOKEN_KW_PLAIN_LAMBDA:
        node = create_node(NODE_PLAIN_LAMBDA);
        match(context, TOKEN_KW_PLAIN_LAMBDA);
        add_child(node, formals(context));
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado corpo da função após a lista de parâmetros de '#%plain-lambda'");
        }
        else
        {
            do
            {
                add_child(node, expr(context));
            } while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF);
        }
        close_msg = "esperado fechamento de parênteses após corpo de '#%plain-lambda'";
        break;

    case TOKEN_KW_CASE_LAMBDA:
        node = create_node(NODE_CASE_LAMBDA);
        match(context, TOKEN_KW_CASE_LAMBDA);
        while (context->currentType == TOKEN_LPAREN)
        {
            TreeNode *clause = create_node(NODE_PLAIN_LAMBDA);
            match(context, TOKEN_LPAREN);
            add_child(clause, formals(context));
            if (context->currentType == TOKEN_RPAREN)
            {
                error(context, &context->tokens->tokens[context->currentIndex],
                      "esperado corpo da cláusula em 'case-lambda'");
            }
            else
            {
                do
                {
                    add_child(clause, expr(context));
                } while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF);
            }
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses ao final da cláusula de 'case-lambda'");
            add_child(node, clause);
        }
        close_msg = "esperado fechamento de parênteses após expressão 'case-lambda'";
        break;

    case TOKEN_KW_IF:
        node = create_node(NODE_IF);
        match(context, TOKEN_KW_IF);
        add_child(node, expr(context)); // Cond
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado ramo verdadeiro (then) na expressão 'if'");
        }
        else
        {
            add_child(node, expr(context));
        }
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado ramo falso (else) na expressão 'if'");
        }
        else
        {
            add_child(node, expr(context));
        }
        close_msg = "esperado fechamento de parênteses após expressão 'if'";
        break;

    case TOKEN_KW_BEGIN:
    case TOKEN_KW_BEGIN0:
        node = create_node(keyword == TOKEN_KW_BEGIN ? NODE_BEGIN : NODE_BEGIN0);
        match(context, keyword);
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  keyword == TOKEN_KW_BEGIN
                      ? "esperado ao menos uma expressão dentro de 'begin'"
                      : "esperado ao menos uma expressão dentro de 'begin0'");
        }
        else
        {
            do
            {
                add_child(node, expr(context));
            } while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF);
        }
        close_msg = keyword == TOKEN_KW_BEGIN
                        ? "esperado fechamento de parênteses após bloco 'begin'"
                        : "esperado fechamento de parênteses após bloco 'begin0'";
        break;

    case TOKEN_KW_LET_VALUES:
    case TOKEN_KW_LETREC_VALUES:
    {
        int is_let = (keyword == TOKEN_KW_LET_VALUES);
        const char *form_name = is_let ? "let-values" : "letrec-values";
        node = create_node(is_let ? NODE_LET_VALUES : NODE_LETREC_VALUES);
        match(context, keyword);

        // Parsing dos bindings: ([(id ...) expr] ...)
        if (is_let)
            expect(context, TOKEN_LPAREN, "esperado abertura de parênteses para a lista de ligações de 'let-values'");
        else
            expect(context, TOKEN_LPAREN, "esperado abertura de parênteses para a lista de ligações de 'letrec-values'");
        while (context->currentType == TOKEN_LBRACKET || context->currentType == TOKEN_LPAREN)
        {
            TreeNode *binding = create_node(NODE_BINDING);
            ERTokenType open_bracket = context->currentType;
            match(context, open_bracket);

            TreeNode *ids = create_node(NODE_FORMALS);
            expect(context, TOKEN_LPAREN, "esperado abertura de parênteses para a lista de variáveis da ligação");
            while (context->currentType == TOKEN_ID)
            {
                add_child(ids, create_node(NODE_ID));
                match(context, TOKEN_ID);
            }
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após a lista de variáveis da ligação");

            add_child(binding, ids);
            add_child(binding, expr(context));

            expect(context,
                   open_bracket == TOKEN_LBRACKET ? TOKEN_RBRACKET : TOKEN_RPAREN,
                   open_bracket == TOKEN_LBRACKET
                       ? "esperado fechamento de colchetes após a expressão da ligação"
                       : "esperado fechamento de parênteses após a expressão da ligação");
            add_child(node, binding);
        }
        if (is_let)
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após a lista de ligações de 'let-values'");
        else
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após a lista de ligações de 'letrec-values'");

        // expr ...+
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  is_let
                      ? "esperado ao menos uma expressão no corpo de 'let-values'"
                      : "esperado ao menos uma expressão no corpo de 'letrec-values'");
        }
        else
        {
            do
            {
                add_child(node, expr(context));
            } while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF);
        }
        close_msg = is_let
                        ? "esperado fechamento de parênteses após corpo de 'let-values'"
                        : "esperado fechamento de parênteses após corpo de 'letrec-values'";
        (void)form_name;
        break;
    }

    case TOKEN_KW_SET:
        node = create_node(NODE_SET);
        match(context, TOKEN_KW_SET);
        add_child(node, create_node(NODE_ID));
        expect(context, TOKEN_ID, "esperado nome de variável após 'set!'");
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado expressão para o novo valor em 'set!'");
        }
        else
        {
            add_child(node, expr(context));
        }
        close_msg = "esperado fechamento de parênteses após expressão 'set!'";
        break;

    case TOKEN_KW_QUOTE:
        node = create_node(NODE_QUOTE);
        match(context, TOKEN_KW_QUOTE);
        add_child(node, datum(context));
        close_msg = "esperado fechamento de parênteses após expressão 'quote'";
        break;

    case TOKEN_KW_QUOTE_SYNTAX:
        node = create_node(NODE_QUOTE_SYNTAX);
        match(context, TOKEN_KW_QUOTE_SYNTAX);
        add_child(node, datum(context));
        if (context->currentType == TOKEN_KW_LOCAL)
        {
            match(context, TOKEN_KW_LOCAL);
        }
        close_msg = "esperado fechamento de parênteses após expressão 'quote-syntax'";
        break;

    case TOKEN_KW_WITH_CONT_MARK:
        node = create_node(NODE_WITH_CONT_MARK);
        match(context, TOKEN_KW_WITH_CONT_MARK);
        add_child(node, expr(context));
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado expressão de valor em 'with-continuation-mark'");
        }
        else
        {
            add_child(node, expr(context));
        }
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado expressão de resultado em 'with-continuation-mark'");
        }
        else
        {
            add_child(node, expr(context));
        }
        close_msg = "esperado fechamento de parênteses após expressão 'with-continuation-mark'";
        break;

    case TOKEN_KW_TOP:
        node = create_node(NODE_TOP);
        match(context, TOKEN_KW_TOP);
        expect(context, TOKEN_DOT, "esperado '.' após '#%top'");
        add_child(node, create_node(NODE_ID));
        expect(context, TOKEN_ID, "esperado nome de variável após '#%top .'");
        close_msg = "esperado fechamento de parênteses após referência '#%top'";
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
            expect(context, TOKEN_KW_TOP, "esperado '#%top' dentro de '#%variable-reference'");
            expect(context, TOKEN_DOT, "esperado '.' após '#%top' em '#%variable-reference'");
            TreeNode *top_node = create_node(NODE_TOP);
            add_child(top_node, create_node(NODE_ID));
            expect(context, TOKEN_ID, "esperado nome de variável em '#%variable-reference'");
            add_child(node, top_node);
            expect(context, TOKEN_RPAREN, "esperado fechamento de parênteses após referência '#%top' interna");
        }
        else
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado nome de variável ou '(#%top . id)' após '#%variable-reference'");
        }
        close_msg = "esperado fechamento de parênteses após '#%variable-reference'";
        break;

    case TOKEN_KW_PLAIN_APP:
        node = create_node(NODE_PLAIN_APP);
        match(context, TOKEN_KW_PLAIN_APP);
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado a função a ser aplicada após '#%plain-app'");
        }
        else
        {
            do
            {
                add_child(node, expr(context));
            } while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF);
        }
        close_msg = "esperado fechamento de parênteses após chamada de função '#%plain-app'";
        break;

    default:
        // Aplicação normal (quando não tem palavra-chave do Racket, ex: (foo 1 2))
        node = create_node(NODE_PLAIN_APP);
        if (context->currentType == TOKEN_RPAREN)
        {
            error(context, &context->tokens->tokens[context->currentIndex],
                  "esperado uma expressão dentro dos parênteses");
        }
        else
        {
            while (context->currentType != TOKEN_RPAREN && context->currentType != TOKEN_EOF)
            {
                add_child(node, expr(context));
            }
        }
        close_msg = "esperado fechamento de parênteses ao final da chamada de função";
        break;
    }

    if (context->currentType == TOKEN_EOF)
    {
        error(context, &context->tokens->tokens[context->currentIndex],
              "fim de arquivo inesperado: há parênteses abertos sem fechamento correspondente");
    }
    else
    {
        expect(context, TOKEN_RPAREN, close_msg);
    }
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
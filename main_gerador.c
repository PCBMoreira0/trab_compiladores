#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/afn.h"
#include "libs/afd.h"
#include "libs/regular_expression.h"
#include "libs/lex_parsing.h"

#define MAX_TOKENS 100

int main() {
    ERToken ers[MAX_TOKENS];
    printf("[*] Lendo arquivo de definicao lexica 'racket.lex'...\n");
    int num_tokens = load_tokens_from_file("racket.lex", ers);
    
    if (num_tokens == 0) {
        printf("[-] Nenhum token carregado. Verifique o arquivo racket.lex. Saindo...\n");
        return 1;
    }
    printf("[+] %d tokens carregados com sucesso.\n\n", num_tokens);

    printf("[*] Construindo e unificando AFNs...\n");
    printf("[*] Construindo e unificando AFNs...\n");
    AFN_Fragment *fragments = malloc(num_tokens * sizeof(AFN_Fragment));
    AFN_Context *ctx = afnCreateContext();
    
    for(int i = 0; i < num_tokens; i++) {
        char *expanded = expandIntervals(ers[i].value);
        char *processed = ERpreProcess(expanded);

        // printf("[DEBUG] Token %d (%s) Regex Final: %s\n", i, get_token_name(ers[i].type), processed);
        ers[i].value = processed;
        
        AFN_Fragment f;
        afnBuildFromER(ctx, ers[i], &f);
        fragments[i] = f;
    }

    AFN_Fragment unified = afnUnify(ctx, fragments, num_tokens);
    free(fragments);

    printf("[*] Convertendo AFN unificado para AFD (Subset Construction)...\n");
    Automato_AFD *dfa = afdBuild(ctx, unified.start);
    if (!dfa) {
        printf("[-] Erro ao construir o AFD.\n");
        afnFreeContext(ctx);
        return 1;
    }
    printf("[+] AFD criado com %d estados.\n", dfa->num_states);

    printf("[*] Minimizando o AFD...\n");
    Automato_AFD *minimized_dfa = afdMinimize(dfa);
    printf("[+] AFD minimizado criado com %d estados.\n\n", minimized_dfa->num_states);

    printf("====================================================\n");
    printf(" TESTANDO STRINGS DO RACKET NO AFD MINIMIZADO\n");
    printf("====================================================\n");

    const char *test_strings[] = {
        // --- DELIMITADORES ---
        "(", ")", "[", "]", ".",
        
        // --- KEYWORDS (Testando as sobreposições) ---
        "module", "module*", 
        "begin", "begin0", "begin-for-syntax",
        "define-values", "define-syntaxes",
        "let-values", "letrec-values",
        "case-lambda", "if", "set!",
        "quote", "quote-syntax", 
        "with-continuation-mark",
        
        // --- KEYWORDS ESPECIAIS (#) ---
        "#%expression", "#%plain-module-begin", 
        "#%provide", "#%declare", "#%require", 
        "#%plain-lambda", "#%plain-app", 
        "#%top", "#%variable-reference", 
        "#f", "#t", "#:local",
        
        // --- IDENTIFICADORES COMPLEXOS ---
        "my-var",         // ID simples com hífen
        "camelCaseId",    // ID com letras maiúsculas
        "is-valid?",      // ID com interrogação (comum em Scheme/Racket)
        "+", "-", "*",    // Operadores matemáticos são IDs válidos em Racket
        "<=>",            // Símbolos agrupados
        "id_com_num123",  // ID com números no meio/fim
        
        // --- STRINGS ---
        "\"\"",                   // String vazia
        "\"um texto simples\"",   // String com espaços
        "\"string_com-simbolos!\"", // String com caracteres permitidos
        
        // --- NÚMEROS ---
        "0", "12345",     // Inteiros
        "0.0", "3.14159", // Ponto flutuante
        
        // --- WHITESPACE ---
        " ", "\t", "\n", "\r",
        
        // --- CASOS DE REJEIÇÃO (Devem dar UNKNOWN) ---
        "123id",          // Começa com número (inválido para ID na nossa regra)
        "invalido@",      // Caractere '@' não está no alfabeto de ID
        "\"fechamento_quebrado" // String sem a aspa final
    };
    
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    for (int i = 0; i < num_tests; i++) {
        ERTokenType accepted = run_dfa(minimized_dfa, test_strings[i]);
        
        printf("String: %-15s -> %s\n", 
               strcmp(test_strings[i], "") == 0 ? "\"\"" : test_strings[i], 
               get_token_name(accepted));
    }

    printf("\n[*] Liberando memoria...\n");
    
    FILE *f = fopen("afd_serializado.bin", "wb");
    AFDSerialize(minimized_dfa, f);
    fclose(f);
    afdFree(dfa);
    afdFree(minimized_dfa);
    afnFreeContext(ctx);
    
    printf("[+] Scanner testado com sucesso. Fim da execucao.\n");

    return 0;
}
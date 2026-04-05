#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/afn.h"
#include "libs/afd.h"

int main() {
    printf("====================================================\n");
    printf(" INICIANDO TESTE: REGEX (a|b)*abb\n");
    printf("====================================================\n\n");

    AFN_Context *ctx = afnCreateContext();
    if (!ctx) {
        printf("Erro ao criar o contexto do AFN.\n");
        return 1;
    }

    printf("[*] Construindo o AFN...\n");
    
    AFN_Fragment a = afnCreateSymbol(ctx, 'a');
    AFN_Fragment b = afnCreateSymbol(ctx, 'b');
    AFN_Fragment a_or_b = afnCreateUnion(ctx, a, b);

    AFN_Fragment kleene = afnCreateKleene(ctx, a_or_b);

    AFN_Fragment a2 = afnCreateSymbol(ctx, 'a');
    AFN_Fragment b2 = afnCreateSymbol(ctx, 'b');
    AFN_Fragment b3 = afnCreateSymbol(ctx, 'b');

    AFN_Fragment concat1 = afnCreateConcat(ctx, kleene, a2);
    AFN_Fragment concat2 = afnCreateConcat(ctx, concat1, b2);
    AFN_Fragment final_nfa = afnCreateConcat(ctx, concat2, b3);

    afnPrint(ctx, final_nfa.start);

    printf("\n[*] Convertendo AFN para AFD (Subset Construction)...\n");
    Automato_AFD *dfa = afdBuild(ctx, final_nfa.start);
    if (!dfa) {
        printf("Erro ao construir o AFD.\n");
        afnFreeContext(ctx);
        return 1;
    }
    printf("[+] AFD criado com sucesso com %d estados.\n", dfa->num_states);

    printf("\n====================================================\n");
    printf(" TESTANDO STRINGS NO AFD\n");
    printf("====================================================\n");

    const char *test_strings[] = {
        "abb",       // Aceita (mínimo)
        "aabb",      // Aceita
        "babb",      // Aceita
        "ababb",     // Aceita
        "bbbbabb",   // Aceita
        "a",         // Rejeita
        "ab",        // Rejeita
        "bba",       // Rejeita
        "abbb",      // Rejeita
        "abab",      // Rejeita
        ""           // Rejeita (vazio)
    };
    
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    for (int i = 0; i < num_tests; i++) {
        int accepted = run_dfa(dfa, test_strings[i]);
        printf("String: %-10s -> %s\n", 
            strcmp(test_strings[i], "") == 0 ? "\"\"" : test_strings[i], 
            accepted ? "[ACEITO]" : "[REJEITADO]");
    }

    printf("\n[*] Liberando memoria...\n");
    afdFree(dfa);
    afnFreeContext(ctx);
    printf("[+] Fim do teste.\n");

    return 0;
}
#include "libs/afn.h"
#include "libs/afd.h"
#include "libs/regular_expression.h"

int main() {
    char *er = "(a|b)*&a&b&b";

    char *er_posfixa = shuntingYard(er);

    AFN_Context *ctx = afnCreateContext();
    if (!ctx) {
        printf("Erro ao criar o contexto do AFN.\n");
        return 1;
    }

    AFN_Fragment fragment;
    if (!afnBuildFromER(ctx, er_posfixa, &fragment)) {
        printf("Erro ao construir o AFN a partir da ER.\n");
        return 1;
    }

    afnPrint(ctx, fragment.start);


    printf("\n[*] Convertendo AFN para AFD (Subset Construction)...\n");
    Automato_AFD *dfa = afdBuild(ctx, fragment.start);
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
#include "libs/afn.h"
#include "libs/afd.h"
#include "libs/regular_expression.h"

int main() {
    char *er = "if";
    char *er2 = "(a|b|i|f)^";
    char *er3 = "(0|1|2|3|4|5|6|7|8|9)^";
 
    char *er_pre = ERpreProcess(er);
    char *er_pre2 = ERpreProcess(er2);
    char *er_pre3 = ERpreProcess(er3);
    printf("%s\n", er_pre);
    printf("%s\n", er_pre2);
    printf("%s\n", er_pre3);
    AFN_Context *ctx = afnCreateContext();
    if (!ctx) {
        printf("Erro ao criar o contexto do AFN.\n");
        return 1;
    }

    ERToken token = {er_pre, TOKEN_IF, 1};
    ERToken token2 = {er_pre2, TOKEN_IDENTIFIER, 0};
    ERToken token3 = {er_pre3, TOKEN_NUMBER, 0};
    AFN_Fragment fragment;
    afnBuildFromER(ctx, token, &fragment);

    AFN_Fragment fragment2;
    afnBuildFromER(ctx, token2, &fragment2);
    

    AFN_Fragment fragment3;
    afnBuildFromER(ctx, token3, &fragment3);

    AFN_Fragment fragments[] = {fragment, fragment2, fragment3};
    AFN_Fragment unified = afnUnify(ctx, fragments, 3);

    afnPrint(ctx, unified.start);


    printf("\n[*] Convertendo AFN para AFD (Subset Construction)...\n");
    Automato_AFD *dfa = afdBuild(ctx, unified.start);
    if (!dfa) {
        printf("Erro ao construir o AFD.\n");
        afnFreeContext(ctx);
        return 1;
    }

    printf("[*] Antes da minimização:\n");
    afdPrint(dfa);

    printf("[+] AFD criado com sucesso com %d estados.\n", dfa->num_states);

    printf("\n[*] Minimizando o AFD...\n");
    Automato_AFD *minimized_dfa = afdMinimize(dfa);
    afdPrint(minimized_dfa);
    printf("[+] AFD minimizado criado com sucesso com %d estados.\n", minimized_dfa->num_states);

    printf("\n====================================================\n");
    printf(" TESTANDO STRINGS NO AFD\n");
    printf("====================================================\n");

    const char *test_strings[] = {
        // --- ACEITAS ---
        "a",             // Mínimo obrigatório (Bloco 2 apenas)
        "aaa",           // Bloco 2 com múltiplas repetições
        "ba",            // Bloco 1 (x) + Bloco 2 (a)
        "ifa",           // Bloco 1 (y+z) + Bloco 2 (a)
        "ifififa",         // Bloco 1 (vários y + z) + Bloco 2 (a)
        "bbbbif",          // Bloco 1 (x e depois yz) + Bloco 2 (a)
        "if",           // Bloco 2 (a) + Bloco 3 (bc)
        "i",
        "f",
        "1",
        "1235",
        "5043",
        "26",
    };
    
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    const char* TokenTypeNames[] = {
        "NONE",
        "TOKEN_IF",
        "TOKEN_ID",
        "TOKEN_NUM"
    };
    for (int i = 0; i < num_tests; i++) {
        int accepted = run_dfa(minimized_dfa, test_strings[i]);
        printf("String: %-10s -> %s\n", 
            strcmp(test_strings[i], "") == 0 ? "\"\"" : test_strings[i], 
            TokenTypeNames[accepted]);
    }

    printf("\n[*] Liberando memoria...\n");
    afdFree(dfa);
    afdFree(minimized_dfa);
    afnFreeContext(ctx);
    printf("[+] Fim do teste.\n");

    return 0;
}
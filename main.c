#include "libs/afn.h"
#include "libs/afd.h"
#include "libs/regular_expression.h"

int main() {
    char *er = "(x|y+z)^a+(bc|d)^";

    char *er_pre = ERpreProcess(er);
    printf("%s\n", er_pre);
    AFN_Context *ctx = afnCreateContext();
    if (!ctx) {
        printf("Erro ao criar o contexto do AFN.\n");
        return 1;
    }

    AFN_Fragment fragment;
    if (!afnBuildFromER(ctx, er_pre, &fragment)) {
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
        "xa",            // Bloco 1 (x) + Bloco 2 (a)
        "yza",           // Bloco 1 (y+z) + Bloco 2 (a)
        "yyyza",         // Bloco 1 (vários y + z) + Bloco 2 (a)
        "xyza",          // Bloco 1 (x e depois yz) + Bloco 2 (a)
        "abc",           // Bloco 2 (a) + Bloco 3 (bc)
        "ad",            // Bloco 2 (a) + Bloco 3 (d)
        "abcbcdd",       // Bloco 2 (a) + Bloco 3 (bc, bc, d, d)
        "xyyzaaabcbc",   // Complexa: Bloco 1 (x, yyz), Bloco 2 (aaa), Bloco 3 (bc, bc)

        // --- REJEITADAS ---
        "x",             // Rejeita: Falta o Bloco 2 (a+) que é obrigatório
        "bc",            // Rejeita: Falta o Bloco 2 (a+)
        "ya",            // Rejeita: Bloco 1 incompleto (y+ exige um z depois)
        "yzza",          // Rejeita: Bloco 1 inválido (z não tem operador +, apenas um é permitido)
        "ab",            // Rejeita: Bloco 3 incompleto (esperava bc, recebeu apenas b)
        "acb",           // Rejeita: Bloco 3 com ordem errada (esperava bc)
        "ax",            // Rejeita: 'x' só pode vir antes do 'a' (no Bloco 1)
        "y+za",          // Rejeita: O símbolo '+' é um operador, não um literal
        "",              // Rejeita: String vazia não satisfaz a obrigatoriedade de a+
        "xyz"            // Rejeita: Falta o caractere 'a'
    };
    
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    for (int i = 0; i < num_tests; i++) {
        int accepted = run_dfa(minimized_dfa, test_strings[i]);
        printf("String: %-10s -> %s\n", 
            strcmp(test_strings[i], "") == 0 ? "\"\"" : test_strings[i], 
            accepted ? "[ACEITO]" : "[REJEITADO]");
    }

    printf("\n[*] Liberando memoria...\n");
    afdFree(dfa);
    afdFree(minimized_dfa);
    afnFreeContext(ctx);
    printf("[+] Fim do teste.\n");

    return 0;
}
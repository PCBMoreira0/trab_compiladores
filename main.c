#include "libs/afn.h"
#include "libs/afd.h"
#include "libs/regular_expression.h"

int main() {

    // LISTA DE EXPRESSÕES REGULARES
    ERToken ers[] = {
        {"if", TOKEN_IF, 1},
        {"([a-z]|[A-Z])+", TOKEN_IDENTIFIER, 0},
        {"[0-9]^", TOKEN_INT, 0},
        {"([0-9])^(.[0-9]+|E([0-9]|(-[0-9])))^", TOKEN_FLOAT, 0}
    };
    
    AFN_Fragment fragments[4];
    AFN_Context *ctx = afnCreateContext();
    for(int i = 0; i < 4; i++){
        ers[i].value = ERpreProcess(expandIntervals(ers[i].value));
        AFN_Fragment f;
        afnBuildFromER(ctx, ers[i], &f);
        fragments[i] = f;
    }

    AFN_Fragment unified = afnUnify(ctx, fragments, 4);

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
    printf("[+] AFD minimizado criado com sucesso com %d estados.\n", minimized_dfa->num_states);

    printf("\n[*] Serializando o AFD...\n");
    
    FILE *f = fopen("afd_serializado.bin", "wb");
    AFDSerialize(minimized_dfa, f);
    afdPrint(minimized_dfa);
    fclose(f);

    afdFree(minimized_dfa);

    printf("\n[*] Desserializando o AFD...\n");

    f = fopen("afd_serializado.bin", "rb");
    minimized_dfa = AFDDeserialize(f);
    fclose(f);


    printf("\n====================================================\n");
    printf(" TESTANDO STRINGS NO AFD\n");
    printf("====================================================\n");

    const char *test_strings[] = {
        "a",            
        "aaa",          
        "ba",           
        "ifa",          
        "ifififa",      
        "bbbbif",       
        "if",
        "if",
        "if",
        "i",
        "f",
        "1",
        "1.23",
        "156.3253",
        ".234",
        "12.45E-2",
        "1.564E5",
        "1E-2",
        "xvjxbvdsboehb",
        "132454",
        "12",
        "3753",
    };
    
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    const char* TokenTypeNames[] = {
        "NONE",
        "TOKEN_IF",
        "TOKEN_ID",
        "TOKEN_INT",
        "TOKEN_FLOAT"
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
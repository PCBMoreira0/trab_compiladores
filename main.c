#include <stdio.h>
#include "libs/afn.h"

int main() {
    printf("Compilando Regex: a*\n\n");
    AFN_Context *ctx = afnCreateContext();
    if (!ctx) {
        printf("Erro fatal: Sem memoria para criar o contexto.\n");
        return 1;
    }
    AFN_Fragment frag_a = afnCreateSymbol(ctx, 'a');

    AFN_Fragment afn_final = afnCreateKleene(ctx, frag_a);
    afnPrint(ctx, afn_final.start);

    afnFreeContext(ctx);

    printf("Memoria liberada com sucesso!\n");
    return 0;
}
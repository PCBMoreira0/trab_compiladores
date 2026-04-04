#include <stdio.h>
#include "libs/afn.h"

int main() {
    printf("Compilando Regex: a*\n\n");
    AFN_Fragment frag_a = afnCreateSymbol('a');
    AFN_Fragment afn_final = afnCreateKleene(frag_a);
    afnPrint(afn_final.start);
    afnFree(afn_final.start);

    printf("Memoria liberada com sucesso!\n");
    return 0;
}
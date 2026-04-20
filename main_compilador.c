#include <stdio.h>
#include <stdlib.h>
#include "libs/afd.h"
#include "libs/scanner.h"

int main() {
    printf("[*] Iniciando o Compilador Racket...\n");

    printf("[*] Carregando a maquina de estados (afd_serializado.bin)...\n");
    FILE *f = fopen("afd_serializado.bin", "rb");
    if (!f) {
        printf("Erro: Arquivo afd_serializado.bin nao encontrado!\n");
        printf("Rode o gerador_scanner primeiro.\n");
        return 1;
    }
    Automato_AFD *dfa = AFDDeserialize(f);
    fclose(f);
    printf("[+] Scanner carregado com sucesso! (%d estados)\n", dfa->num_states);

    printf("[*] Analisando arquivo 'programa.rkt'...\n");
    TokenList *fita = tokenize_file(dfa, "programa.rkt");

    if (fita) {
        print_token_list(fita);
        save_token_list_to_file(fita, "output.tks");
        
        // Etapa de parsing (ainda temos q implementar)

        free_token_list(fita);
    }

    afdFree(dfa);
    return 0;
}
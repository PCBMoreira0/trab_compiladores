#include <stdio.h>
#include <stdlib.h>
#include "libs/afd.h"
#include "libs/scanner.h"

int main(int argc, char **argv) {
     if (argc < 2){
        printf("O primeiro argumento deve ser o arquivo racket que deseja testar!\n");
        return 1;
    }
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
    printf("[*] Analisando arquivo %s...\n", argv[1]);
    TokenList *fita = tokenize_file(dfa, argv[1]);

    if (fita) {
        print_token_list(fita);
        save_token_list_to_file(fita, "output.tks");

        TokenList *loaded_fita = load_token_list_from_file("output.tks");
        printf("\n[*] Tokens carregados de 'output.tks':\n");
        print_token_list(loaded_fita);
        
        // Etapa de parsing (ainda temos q implementar)

        free_token_list(fita);
    }
    afdFree(dfa);
    return 0;
}
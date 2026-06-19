#include "ranking.h"
#include <stdio.h>

void carregar_recordes(SnakeGame* game) {
    FILE* arq = fopen("recordes.txt", "r");
    
    if (arq == NULL) {
        for (int i = 0; i < MAX_RECORDES; i++) {
            game->recordes[i] = 0;
        }
        return; 
    }
    
    for (int i = 0; i < MAX_RECORDES; i++) {
        if (fscanf(arq, "%d", &game->recordes[i]) != 1) {
            game->recordes[i] = 0;
        }
    }
    fclose(arq);
}

void salvar_recordes(SnakeGame* game) {
    FILE* arq = fopen("recordes.txt", "w");
    if (arq == NULL) return;
    
    for (int i = 0; i < MAX_RECORDES; i++) {
        fprintf(arq, "%d\n", game->recordes[i]);
    }
    fclose(arq);
}

void ordenar_recordes(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] < arr[j + 1]) { 
                int temp = arr[j];       
                arr[j] = arr[j + 1];     
                arr[j + 1] = temp;       
            }
        }
    }
}

int busca_binaria_recordes(int arr[], int n, int chave) {
    int inicio = 0;
    int fim = n - 1;
    
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2; 
        
        if (arr[meio] == chave) return meio; 
        
        if (arr[meio] < chave) {
            fim = meio - 1;   
        } else {
            inicio = meio + 1; 
        }
    }
    
    return -1; 
}
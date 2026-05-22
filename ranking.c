#include "ranking.h"
#include <stdio.h>

// Aqui eu abro o arquivo de texto para ler os recordes salvos quando o jogo inicia
void carregar_recordes(int recordes[]) {
    // Tento abrir o arquivo "recordes.txt" no modo de leitura ("r")
    FILE* arq = fopen("recordes.txt", "r");
    
    // Se o arquivo não existir (como na primeira vez que o jogo roda), 
    // eu preencho todas as posições do ranking com zero
    if (arq == NULL) {
        for (int i = 0; i < MAX_RECORDES; i++) {
            recordes[i] = 0;
        }
        return; // Encerro a função por aqui já que não há arquivo para ler
    }
    
    // Se o arquivo existir, eu uso este laço para ler os 5 números guardados 
    // dentro dele e os coloco direto no vetor de recordes do jogo
    for (int i = 0; i < MAX_RECORDES; i++) {
        if (fscanf(arq, "%d", &recordes[i]) != 1) {
            recordes[i] = 0; // Se houver algum erro ou linha vazia, garanto que fica 0
        }
    }
    
    // Sempre fecho o arquivo depois de usar para liberar a memória do sistema
    fclose(arq);
}

// Aqui eu salvo as pontuações atualizadas de volta no arquivo de texto
void salvar_recordes(int recordes[]) {
    // Abro ou crio o arquivo "recordes.txt" no modo de escrita ("w")
    FILE* arq = fopen("recordes.txt", "w");
    if (arq == NULL) return; // Se der algum erro ao criar o arquivo, saio da função
    
    // Escrevo os 5 recordes atuais no arquivo, colocando um número por linha (\n)
    for (int i = 0; i < MAX_RECORDES; i++) {
        fprintf(arq, "%d\n", recordes[i]);
    }
    
    // Fecho o arquivo para garantir que os dados sejam gravados fisicamente no HD
    fclose(arq);
}

// Meu algoritmo de ordenação (Bubble Sort) para organizar o ranking
void ordenar_recordes(int arr[], int n) {
    // Uso dois laços "for" para percorrer o vetor e comparar os números de dois em dois
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // Se o número da esquerda for MENOR que o número da direita, eu faço eles trocarem de lugar.
            // Faço isso para garantir a ordem DECRESCENTE (os maiores placares ficam no topo).
            if (arr[j] < arr[j + 1]) { 
                int temp = arr[j];       // Guardo o valor da esquerda em uma variável temporária
                arr[j] = arr[j + 1];     // Jogo o valor da direita para a esquerda
                arr[j + 1] = temp;       // Coloco o valor que estava guardado na direita
            }
        }
    }
}

// Meu algoritmo de busca (Busca Binária) para procurar um placar específico
int busca_binaria_recordes(int arr[], int n, int chave) {
    int inicio = 0;
    int fim = n - 1;
    
    // Enquanto o meu espaço de busca não se fechar por completo
    while (inicio <= fim) {
        // Calculo onde fica o meio exato do vetor atual
        int meio = inicio + (fim - inicio) / 2;
        
        // Se a pontuação que está no meio for igual ao meu placar atual, eu encontrei o que queria!
        if (arr[meio] == chave) return meio; // Retorno o índice da posição encontrada (0 a 4)
        
        // Como o meu vetor está organizado do maior para o menor (decrescente), inverto a lógica padrão:
        if (arr[meio] < chave) {
            fim = meio - 1;     // Se o meu placar é maior que o do meio, ele deve estar na metade esquerda
        } else {
            inicio = meio + 1;  // Se o meu placar é menor que o do meio, ele deve estar na metade direita
        }
    }
    
    return -1; // Se o laço terminar e eu não achar nada, retorno -1 indicando que é uma pontuação inédita
}
#include "ranking.h"
#include <stdio.h>

// abro o arquivo de texto para ler os recordes salvos quando o jogo inicia
void carregar_recordes(SnakeGame* game) {
    // abro o arquivo "recordes.txt" (leitura)
    FILE* arq = fopen("recordes.txt", "r");
    
    // se o arquivo não existir (como na primeira vez que o jogo roda)
    // preencho todas as posições do ranking com zero
    if (arq == NULL) {
        for (int i = 0; i < MAX_RECORDES; i++) {
            game->recordes[i] = 0;
        }
        return; // Encerro a função por aqui (se nao tiver arquivo pra ler)
    }
    
    // se o arquivo existir, eu uso este laço para ler os 5 números guardados
    for (int i = 0; i < MAX_RECORDES; i++) {
        if (fscanf(arq, "%d", &game->recordes[i]) != 1) {
            game->recordes[i] = 0; // se houver algum erro ou linha vazia, garanto que fique 0
        }
    }
    
    // sempre fecho o arquivo depois de usar para liberar a memória do sistema
    fclose(arq);
}

// aqui eu salvo as pontuações atualizadas de volta no arquivo de texto
void salvar_recordes(SnakeGame* game) {
    // abro ou crio o arquivo "recordes.txt" no modo de escrita ("w")
    FILE* arq = fopen("recordes.txt", "w");
    if (arq == NULL) return; // se der algum erro ao criar o arquivo, saio da função
    
    // escrevo os 5 recordes atuais no arquivo, colocando um número por linha (\n)
    for (int i = 0; i < MAX_RECORDES; i++) {
        fprintf(arq, "%d\n", game->recordes[i]);
    }
    
    // Fecho o arquivo para garantir que os dados sejam gravados
    fclose(arq);
}

// bubblesort para organizar o ranking
void ordenar_recordes(int arr[], int n) {
    // dois laços "for" para percorrer o vetor e comparar os números de dois em dois
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // Se o número da esquerda for MENOR que o número da direita, eu faço eles trocarem de lugar
            // para garantir a ordem DECRESCENTE (os maiores placares ficam em cima)
            if (arr[j] < arr[j + 1]) { 
                int temp = arr[j];       // guardo o valor da esquerda em uma variável temporária
                arr[j] = arr[j + 1];     // jogo o valor da direita para a esquerda
                arr[j + 1] = temp;       // coloco o valor que estava guardado na direita
            }
        }
    }
}

// busca binaria para procurar um placar específico (pra saber se empatou com alguma posição do ranking)
int busca_binaria_recordes(int arr[], int n, int chave) {
    int inicio = 0;
    int fim = n - 1;
    
    // enquanto o meu espaço de busca não se fechar por completo
    while (inicio <= fim) {
        // calculo onde fica o meio exato do vetor atual
        int meio = inicio + (fim - inicio) / 2;
        
        // se a pontuação que está no meio for igual ao meu placar atual
        if (arr[meio] == chave) return meio; // retorno o índice da posição encontrada (0 a 4)
        
        // como o vetor está organizado do maior para o menor (decrescente), inverto a lógica
        if (arr[meio] < chave) {
            fim = meio - 1;     // se o meu placar é maior que o do meio, ele deve estar na metade esquerda
        } else {
            inicio = meio + 1;  // se o meu placar é menor que o do meio, ele deve estar na metade direita
        }
    }
    
    return -1; // se o laço terminar e eu não achar nada, retorno -1 indicando que é uma pontuação inédita
}
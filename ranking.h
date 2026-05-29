#ifndef RANKING_H
#define RANKING_H

// Incluímos o jogo.h para o ranking.h saber o que é um SnakeGame
#include "jogo.h"

// As duas primeiras funções agora recebem a struct do jogo inteira!
void carregar_recordes(SnakeGame* game);
void salvar_recordes(SnakeGame* game);

// Essas continuam iguais
void ordenar_recordes(int arr[], int n);
int busca_binaria_recordes(int arr[], int n, int chave);

#endif
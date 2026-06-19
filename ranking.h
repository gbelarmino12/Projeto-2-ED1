#ifndef RANKING_H
#define RANKING_H

#include "jogo.h"

void carregar_recordes(SnakeGame* game);
void salvar_recordes(SnakeGame* game);
void ordenar_recordes(int arr[], int n);
int busca_binaria_recordes(int arr[], int n, int chave);

#endif
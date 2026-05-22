#ifndef RANKING_H
#define RANKING_H

// Defino que o meu ranking vai guardar apenas o TOP 5 melhores pontuações
#define MAX_RECORDES 5

// pra avisar ao compilador que essas 4 funções existem e podem ser usadas
void carregar_recordes(int recordes[]);
void salvar_recordes(int recordes[]);
void ordenar_recordes(int arr[], int n);
int busca_binaria_recordes(int arr[], int n, int chave);

#endif
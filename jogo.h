#ifndef JOGO_H
#define JOGO_H

// módulos principais e os addons do allegro 
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>       

// constantes lógicas
#define FALSE 0
#define TRUE  1
#define MAX_RECORDES 5 //

// enumeração para controlar o estado atual das telas do jogo
enum ESTADO { MENU, JOGANDO, GAMEOVER };

// struct para guardar qualquer coordenada X e Y na tela
typedef struct {
    int x, y;
} Ponto;

// struct que representa cada pedaço/bloco do corpo da cobra
typedef struct {
    Ponto posicao;
} SnakeSegment;

// struct principal que unifica e gerencia todo o estado do jogo
typedef struct {
    SnakeSegment* corpo_cobra; // ponteiro para criar o vetor dinâmico da cobrinha
    int tamanho_cobra;         // quantidade atual de blocos da cobra
    int max_tamanho_alocado;   // limite atual de memória alocada no bloco de dados

    Ponto comida;              // coordenadas X e Y da comida
    int dx, dy;                // vetor de direção
    int placar;                // pontuação obtida na partida atual
    char placar_texto[50];     // string usada para formatar e exibir o score na tela

    int recordes[MAX_RECORDES]; // vetor do TOP 5 recordes carregados do arquivo
    int encontrou_na_busca;     // guarda o índice da busca binária

    // ponteiros para gerenciar os elementos do allegro
    ALLEGRO_DISPLAY* display;
    ALLEGRO_TIMER* timer;          
    ALLEGRO_EVENT_QUEUE* fila_eventos; 
    ALLEGRO_FONT* fonte;           
} SnakeGame;

// constantes de configuração do jogo
#define FPS 9.0             
#define LARGURA_TELA 640      
#define ALTURA_TELA 480       
#define BLOCO 40             
#define TAMANHO_INICIAL_COBRA 3

// declaração das funções
SnakeGame* criar_jogo();
void destruir_jogo(SnakeGame* game);
void resetar_jogo(SnakeGame* game);
int mover_cobra(SnakeGame* game);
void desenhar_jogo(SnakeGame* game);
void atualizar_placar(SnakeGame* game);

#endif
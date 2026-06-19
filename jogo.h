#ifndef JOGO_H
#define JOGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>       
#include <allegro5/allegro_image.h> 

#define FALSE 0
#define TRUE  1
#define MAX_RECORDES 5 

#define FPS 9.0             
#define LARGURA_TELA 1280      
#define ALTURA_TELA 720       
#define BLOCO 40              
#define TAMANHO_INICIAL_COBRA 3 

enum ESTADO { MENU, JOGANDO, GAMEOVER };

typedef struct {
    int x, y;
} Ponto;

typedef struct {
    Ponto posicao;
} SnakeSegment;

typedef struct NodoObstaculo {
    Ponto posicao;
    struct NodoObstaculo* proximo;
} NodoObstaculo;

typedef struct {
    SnakeSegment* corpo_cobra; 
    int tamanho_cobra;         
    int max_tamanho_alocado;   

    NodoObstaculo* lista_caixas;

    Ponto comida;              
    int dx, dy;                
    int placar;                
    char placar_texto[50];     

    int recordes[MAX_RECORDES]; 
    int encontrou_na_busca;     

    ALLEGRO_DISPLAY* display;
    ALLEGRO_TIMER* timer;          
    ALLEGRO_EVENT_QUEUE* fila_eventos; 
    ALLEGRO_FONT* fonte;           

    ALLEGRO_BITMAP* spr_grama;
    ALLEGRO_BITMAP* spr_maca;
    ALLEGRO_BITMAP* spr_caixa;
    ALLEGRO_BITMAP* spr_cabeca_cima;
    ALLEGRO_BITMAP* spr_cabeca_baixo;
    ALLEGRO_BITMAP* spr_cabeca_esq;
    ALLEGRO_BITMAP* spr_cabeca_dir;
    ALLEGRO_BITMAP* spr_corpo_horiz;
    ALLEGRO_BITMAP* spr_corpo_vert;
    ALLEGRO_BITMAP* spr_corpo_ce; 
    ALLEGRO_BITMAP* spr_corpo_cd; 
    ALLEGRO_BITMAP* spr_corpo_be; 
    ALLEGRO_BITMAP* spr_corpo_bd; 
    ALLEGRO_BITMAP* spr_rabo_cima;
    ALLEGRO_BITMAP* spr_rabo_baixo;
    ALLEGRO_BITMAP* spr_rabo_esq;
    ALLEGRO_BITMAP* spr_rabo_dir;

} SnakeGame;

SnakeGame* criar_jogo();
void destruir_jogo(SnakeGame* game);
void resetar_jogo(SnakeGame* game);
int mover_cobra(SnakeGame* game);
void desenhar_jogo(SnakeGame* game);
void atualizar_placar(SnakeGame* game);
void adicionar_caixa(SnakeGame* game);
void liberar_caixas(SnakeGame* game);

#endif
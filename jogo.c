#include "jogo.h"
#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void liberar_caixas(SnakeGame* game) {
    NodoObstaculo* atual = game->lista_caixas;
    while (atual != NULL) {
        NodoObstaculo* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    game->lista_caixas = NULL;
}

void adicionar_caixa(SnakeGame* game) {
    NodoObstaculo* nova_caixa = (NodoObstaculo*)malloc(sizeof(NodoObstaculo));
    nova_caixa->posicao.x = (rand() % (LARGURA_TELA / BLOCO)) * BLOCO;
    nova_caixa->posicao.y = (rand() % (ALTURA_TELA / BLOCO)) * BLOCO;
    nova_caixa->proximo = game->lista_caixas;
    game->lista_caixas = nova_caixa;
}

SnakeGame* criar_jogo() {
    SnakeGame* game = (SnakeGame*)malloc(sizeof(SnakeGame));
    if (!game) return NULL; 

    if (!al_init()) return NULL;
    al_install_keyboard();       
    al_init_primitives_addon();  
    al_init_font_addon();        
    al_init_image_addon(); 

    game->display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    game->timer = al_create_timer(1.0 / FPS); 
    game->fila_eventos = al_create_event_queue();
    game->fonte = al_create_builtin_font(); 

    game->spr_grama = al_load_bitmap("assets/grass.png");
    game->spr_maca = al_load_bitmap("assets/apple.png");
    game->spr_caixa = al_load_bitmap("assets/box.png");
    game->spr_cabeca_cima = al_load_bitmap("assets/head_up.png");
    game->spr_cabeca_baixo = al_load_bitmap("assets/head_down.png");
    game->spr_cabeca_esq = al_load_bitmap("assets/head_left.png");
    game->spr_cabeca_dir = al_load_bitmap("assets/head_right.png");
    
    game->spr_corpo_horiz = al_load_bitmap("assets/body_horizontal.png");
    game->spr_corpo_vert = al_load_bitmap("assets/body_vertical.png");
    game->spr_corpo_ce = al_load_bitmap("assets/body_topleft.png");
    game->spr_corpo_cd = al_load_bitmap("assets/body_topright.png");
    game->spr_corpo_be = al_load_bitmap("assets/body_bottomleft.png");
    game->spr_corpo_bd = al_load_bitmap("assets/body_bottomright.png");

    game->spr_rabo_cima = al_load_bitmap("assets/tail_up.png");
    game->spr_rabo_baixo = al_load_bitmap("assets/tail_down.png");
    game->spr_rabo_esq = al_load_bitmap("assets/tail_left.png");
    game->spr_rabo_dir = al_load_bitmap("assets/tail_right.png");

    al_register_event_source(game->fila_eventos, al_get_display_event_source(game->display));
    al_register_event_source(game->fila_eventos, al_get_timer_event_source(game->timer));
    al_register_event_source(game->fila_eventos, al_get_keyboard_event_source());

    game->max_tamanho_alocado = TAMANHO_INICIAL_COBRA + 5;
    game->corpo_cobra = (SnakeSegment*)malloc(sizeof(SnakeSegment) * game->max_tamanho_alocado);
    game->lista_caixas = NULL;
    
    strcpy(game->placar_texto, "Score: 0");
    game->encontrou_na_busca = -1;

    carregar_recordes(game);
    resetar_jogo(game);

    return game;
}

void destruir_jogo(SnakeGame* game) {
    if (game) {
        if(game->corpo_cobra) free(game->corpo_cobra);
        
        liberar_caixas(game);
        
        if(game->spr_grama) al_destroy_bitmap(game->spr_grama);
        if(game->spr_maca) al_destroy_bitmap(game->spr_maca);
        if(game->spr_caixa) al_destroy_bitmap(game->spr_caixa);
        if(game->spr_cabeca_cima) al_destroy_bitmap(game->spr_cabeca_cima);
        if(game->spr_cabeca_baixo) al_destroy_bitmap(game->spr_cabeca_baixo);
        if(game->spr_cabeca_esq) al_destroy_bitmap(game->spr_cabeca_esq);
        if(game->spr_cabeca_dir) al_destroy_bitmap(game->spr_cabeca_dir);
        if(game->spr_corpo_horiz) al_destroy_bitmap(game->spr_corpo_horiz);
        if(game->spr_corpo_vert) al_destroy_bitmap(game->spr_corpo_vert);
        if(game->spr_corpo_ce) al_destroy_bitmap(game->spr_corpo_ce);
        if(game->spr_corpo_cd) al_destroy_bitmap(game->spr_corpo_cd);
        if(game->spr_corpo_be) al_destroy_bitmap(game->spr_corpo_be);
        if(game->spr_corpo_bd) al_destroy_bitmap(game->spr_corpo_bd);
        if(game->spr_rabo_cima) al_destroy_bitmap(game->spr_rabo_cima);
        if(game->spr_rabo_baixo) al_destroy_bitmap(game->spr_rabo_baixo);
        if(game->spr_rabo_esq) al_destroy_bitmap(game->spr_rabo_esq);
        if(game->spr_rabo_dir) al_destroy_bitmap(game->spr_rabo_dir);

        al_destroy_font(game->fonte);
        al_destroy_event_queue(game->fila_eventos);
        al_destroy_timer(game->timer);
        al_destroy_display(game->display);

        free(game);
    }
}

void resetar_jogo(SnakeGame* game) {
    game->tamanho_cobra = TAMANHO_INICIAL_COBRA;
    
    game->corpo_cobra[0].posicao = (Ponto){120, 120}; 
    game->corpo_cobra[1].posicao = (Ponto){80, 120};  
    game->corpo_cobra[2].posicao = (Ponto){40, 120};  

    game->dx = BLOCO; game->dy = 0;
    game->comida = (Ponto){320, 240};
    
    game->placar = 0;
    game->encontrou_na_busca = -1;
    
    liberar_caixas(game);
    atualizar_placar(game);
}

int mover_cobra(SnakeGame* game) {
    Ponto nova_cabeca = {game->corpo_cobra[0].posicao.x + game->dx, game->corpo_cobra[0].posicao.y + game->dy};

    if (nova_cabeca.x < 0 || nova_cabeca.x >= LARGURA_TELA || 
        nova_cabeca.y < 0 || nova_cabeca.y >= ALTURA_TELA) return FALSE;

    for (int i = 0; i < game->tamanho_cobra; i++) {
        if (nova_cabeca.x == game->corpo_cobra[i].posicao.x && nova_cabeca.y == game->corpo_cobra[i].posicao.y) return FALSE;
    }

    NodoObstaculo* atual_caixa = game->lista_caixas;
    while (atual_caixa != NULL) {
        if (nova_cabeca.x == atual_caixa->posicao.x && nova_cabeca.y == atual_caixa->posicao.y) {
            return FALSE; 
        }
        atual_caixa = atual_caixa->proximo;
    }

    if (nova_cabeca.x == game->comida.x && nova_cabeca.y == game->comida.y) {
        game->tamanho_cobra++;   
        game->placar += 10;      
        atualizar_placar(game);  

        if (game->tamanho_cobra >= game->max_tamanho_alocado) {
            game->max_tamanho_alocado += 10;
            game->corpo_cobra = (SnakeSegment*)realloc(game->corpo_cobra, sizeof(SnakeSegment) * game->max_tamanho_alocado);
        }

        if ((game->tamanho_cobra - TAMANHO_INICIAL_COBRA) % 5 == 0) {
            adicionar_caixa(game);
        }

        game->comida.x = (rand() % (LARGURA_TELA / BLOCO)) * BLOCO;
        game->comida.y = (rand() % (ALTURA_TELA / BLOCO)) * BLOCO;
    }

    for (int i = game->tamanho_cobra - 1; i > 0; i--) {
        game->corpo_cobra[i] = game->corpo_cobra[i - 1];
    }
    game->corpo_cobra[0].posicao = nova_cabeca;
    return TRUE; 
}

void atualizar_placar(SnakeGame* game) {
    sprintf(game->placar_texto, "Score: %d", game->placar);
}

void desenhar_jogo(SnakeGame* game) {
    al_clear_to_color(al_map_rgb(20, 60, 20));

    if (game->spr_grama) {
        int gw = al_get_bitmap_width(game->spr_grama);
        int gh = al_get_bitmap_height(game->spr_grama);
        for (int y = 0; y < ALTURA_TELA; y += gh) {
            for (int x = 0; x < LARGURA_TELA; x += gw) {
                al_draw_bitmap(game->spr_grama, x, y, 0);
            }
        }
    }

    al_draw_text(game->fonte, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, game->placar_texto);

    if (game->spr_maca) {
        al_draw_bitmap(game->spr_maca, game->comida.x, game->comida.y, 0);
    }

    NodoObstaculo* atual_caixa = game->lista_caixas;
    while (atual_caixa != NULL) {
        if (game->spr_caixa) {
            al_draw_bitmap(game->spr_caixa, atual_caixa->posicao.x, atual_caixa->posicao.y, 0);
        }
        atual_caixa = atual_caixa->proximo;
    }

    for (int i = 0; i < game->tamanho_cobra; i++) {
        int cx = game->corpo_cobra[i].posicao.x;
        int cy = game->corpo_cobra[i].posicao.y;

        if (i == 0) { 
            if (game->dx > 0 && game->spr_cabeca_dir) al_draw_bitmap(game->spr_cabeca_dir, cx, cy, 0);
            else if (game->dx < 0 && game->spr_cabeca_esq) al_draw_bitmap(game->spr_cabeca_esq, cx, cy, 0);
            else if (game->dy > 0 && game->spr_cabeca_baixo) al_draw_bitmap(game->spr_cabeca_baixo, cx, cy, 0);
            else if (game->dy < 0 && game->spr_cabeca_cima) al_draw_bitmap(game->spr_cabeca_cima, cx, cy, 0);
        } 
        else if (i == game->tamanho_cobra - 1) { 
            int px = game->corpo_cobra[i - 1].posicao.x;
            int py = game->corpo_cobra[i - 1].posicao.y;
            
            if (px > cx && game->spr_rabo_esq) al_draw_bitmap(game->spr_rabo_esq, cx, cy, 0);
            else if (px < cx && game->spr_rabo_dir) al_draw_bitmap(game->spr_rabo_dir, cx, cy, 0);
            else if (py > cy && game->spr_rabo_cima) al_draw_bitmap(game->spr_rabo_cima, cx, cy, 0);
            else if (py < cy && game->spr_rabo_baixo) al_draw_bitmap(game->spr_rabo_baixo, cx, cy, 0);
        } 
        else { 
            int px = game->corpo_cobra[i - 1].posicao.x; 
            int py = game->corpo_cobra[i - 1].posicao.y;
            int nx = game->corpo_cobra[i + 1].posicao.x; 
            int ny = game->corpo_cobra[i + 1].posicao.y;

            if (px == nx && game->spr_corpo_vert) {
                al_draw_bitmap(game->spr_corpo_vert, cx, cy, 0);
            } 
            else if (py == ny && game->spr_corpo_horiz) {
                al_draw_bitmap(game->spr_corpo_horiz, cx, cy, 0);
            } 
            else {
                int esq = (px < cx || nx < cx);
                int dir = (px > cx || nx > cx);
                int cima = (py < cy || ny < cy);
                int baixo = (py > cy || ny > cy);

                if (cima && esq && game->spr_corpo_ce) al_draw_bitmap(game->spr_corpo_ce, cx, cy, 0);
                else if (cima && dir && game->spr_corpo_cd) al_draw_bitmap(game->spr_corpo_cd, cx, cy, 0);
                else if (baixo && esq && game->spr_corpo_be) al_draw_bitmap(game->spr_corpo_be, cx, cy, 0);
                else if (baixo && dir && game->spr_corpo_bd) al_draw_bitmap(game->spr_corpo_bd, cx, cy, 0);
            }
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "jogo.h"
#include "ranking.h"

// função responsável por alocar a memoria e as configurações iniciais do jogo
 SnakeGame* criar_jogo() {
    
    // aloca a estrutura do snakegame na memoria
    SnakeGame* game = (SnakeGame*)malloc(sizeof(SnakeGame));
    if (!game) return NULL; // boas praticas. caso faltar memoria no pc

    // carrega os modulos: inicia o allegro e os pacotes nativos (fontes, primitivas graficas e teclado)
    if (!al_init()) return NULL;
    al_install_keyboard();       // ativa a leitura do teclado
    al_init_primitives_addon();  // ativa o desenho de formas (os quadrados da cobra)
    al_init_font_addon();        // ativa a escrita de textos

    // cria a janela do jogo, o relogio de FPS, a fila de eventos e carrega a fonte
    game->display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    game->timer = al_create_timer(1.0 / FPS); 
    game->fila_eventos = al_create_event_queue();
    game->fonte = al_create_builtin_font(); 

    // conecta os perifericos na fila de eventos para o jogo registrar
    al_register_event_source(game->fila_eventos, al_get_display_event_source(game->display));
    al_register_event_source(game->fila_eventos, al_get_timer_event_source(game->timer));
    al_register_event_source(game->fila_eventos, al_get_keyboard_event_source());

    // reserva de memoria para a cobra
    // aloca o tamanho inicial somado a uma margem de segurança (+5)
    game->max_tamanho_alocado = TAMANHO_INICIAL_COBRA + 5;
    
    // aloca o vetor na memória com base nessa capacidade calculada
    game->corpo_cobra = (SnakeSegment*)malloc(sizeof(SnakeSegment) * game->max_tamanho_alocado);
    
    // configurações iniciais do jogo (placar zerado e busca binaria resetada)
    strcpy(game->placar_texto, "Score: 0");
    game->encontrou_na_busca = -1;

    // chama a função de leitura do arquivo .txt para carregar os recordes salvos
    carregar_recordes(game);
    
    // posiciona a cobra e a maça para o início do jogo
    resetar_jogo(game);

    // retorna o ponteiro do jogo pronto para a main
    return game;
}

// rotina de destruição de objetos para evitar vazamento de memória
void destruir_jogo(SnakeGame* game) {
    if (game) {
        if(game->corpo_cobra) free(game->corpo_cobra);
        
        al_destroy_font(game->fonte);
        al_destroy_event_queue(game->fila_eventos);
        al_destroy_timer(game->timer);
        al_destroy_display(game->display);

        free(game);
    }
}

// reseta o estado das variáveis de jogo pra quando o jogador der restart
void resetar_jogo(SnakeGame* game) {
    game->tamanho_cobra = TAMANHO_INICIAL_COBRA;
    
    game->corpo_cobra[0].posicao = (Ponto){120, 120}; // cabeça
    game->corpo_cobra[1].posicao = (Ponto){80, 120};  // tronco
    game->corpo_cobra[2].posicao = (Ponto){40, 120};  // rabo

    game->dx = BLOCO; game->dy = 0;
    game->comida = (Ponto){320, 240};
    
    // zera a pontuação e reseta a flag da busca binaria para o novo round
    game->placar = 0;
    game->encontrou_na_busca = -1;
    atualizar_placar(game);
}

// mecânica de movimentaçao da cobra e checagem de colisões
int mover_cobra(SnakeGame* game) {
    // calcula para onde a cabeça vai no próximo frame somando a posição atual com a direção (dx, dy)
    Ponto nova_cabeca = {game->corpo_cobra[0].posicao.x + game->dx, game->corpo_cobra[0].posicao.y + game->dy};

    // se o x ou y da nova cabeça passar dos limites da janela retorna false (game over)
    if (nova_cabeca.x < 0 || nova_cabeca.x >= LARGURA_TELA || 
        nova_cabeca.y < 0 || nova_cabeca.y >= ALTURA_TELA) return FALSE;

    // percorre o corpo pra verificar se a cabeça da cobra nao colide
    for (int i = 0; i < game->tamanho_cobra; i++) {
        if (nova_cabeca.x == game->corpo_cobra[i].posicao.x && nova_cabeca.y == game->corpo_cobra[i].posicao.y) return FALSE;
    }
    // se as coordenadas da cabeça forem as mesmas da maça
    if (nova_cabeca.x == game->comida.x && nova_cabeca.y == game->comida.y) {
        game->tamanho_cobra++;   // aumenta o tamanho da cobra
        game->placar += 10;      // adiciona os pontos da maça
        atualizar_placar(game);  // atualiza o placar

        // se o tamanho da cobra estourar o bloco de memória atual, realoca dinamicamente 
        // mais espaço em tempo de execução usando o realloc para comportar o novo rabo.
        if (game->tamanho_cobra >= game->max_tamanho_alocado) {
            game->max_tamanho_alocado += 10;
            game->corpo_cobra = (SnakeSegment*)realloc(game->corpo_cobra, sizeof(SnakeSegment) * game->max_tamanho_alocado);
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

// Converte o número inteiro do placar em uma string legível para a Allegro desenhar
void atualizar_placar(SnakeGame* game) {
    sprintf(game->placar_texto, "Score: %d", game->placar);
}

// Renderização gráfica pura dos elementos ativos da partida
void desenhar_jogo(SnakeGame* game) {
    al_clear_to_color(al_map_rgb(20, 60, 20));

    al_draw_text(game->fonte, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, game->placar_texto);

    al_draw_filled_rectangle(game->comida.x + 2, game->comida.y + 2, 
                             game->comida.x + BLOCO - 2, game->comida.y + BLOCO - 2, 
                             al_map_rgb(220, 50, 50));

    for (int i = 0; i < game->tamanho_cobra; i++) {
        ALLEGRO_COLOR cor;
        if (i == 0) cor = al_map_rgb(100, 255, 100);
        else cor = al_map_rgb(50, 180, 50);
        
        al_draw_filled_rectangle(game->corpo_cobra[i].posicao.x + 1, game->corpo_cobra[i].posicao.y + 1, 
                                 game->corpo_cobra[i].posicao.x + BLOCO - 1, game->corpo_cobra[i].posicao.y + BLOCO - 1, 
                                 cor);
    }
}
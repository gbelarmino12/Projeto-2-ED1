#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "jogo.h"
#include "ranking.h"

// função principal // game loop // maquina de estados
int main() {
    SnakeGame* game = criar_jogo();
    if (!game) return -1;

    // maquina de estados: define que o jogo sempre abre na tela de MENU
    enum ESTADO estado_atual = MENU; 
    int sair = FALSE;       // flag que mantém o loop principal rodando
    int redesenhar = TRUE;  // flag pra evitar desenhar atoa
    char texto_ranking[30]; // buffer temporario pra formatar os textos de recorde

    srand(time(NULL));      // seed do gerador de numeros (pra maça)      
    al_start_timer(game->timer);

    // loop do jogo
    while (!sair) {
        ALLEGRO_EVENT evento;

        // pausa o jogo e espera um evento
        al_wait_for_event(game->fila_eventos, &evento);
        
        // evento 1: timer contou um frame
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            // a cobra so se mexe se estiver no estado JOGANDO
            if (estado_atual == JOGANDO) {

                // se a função de movimento retornar false, a cobra bateu
                if (mover_cobra(game) == FALSE) {
                    estado_atual = GAMEOVER; // muda o estado pra tela de morte
                    
                    // disparo da busca binaria. busca o score do round no vetor
                    game->encontrou_na_busca = busca_binaria_recordes(game->recordes, MAX_RECORDES, game->placar);

                    // disparo da atualização
                    // se o score for maior que o ultimo do top 5 entra no vetor
                    if (game->placar > game->recordes[MAX_RECORDES - 1]) {
                        game->recordes[MAX_RECORDES - 1] = game->placar;

                        // chama a função bubblesort pra ordernar do maior pro menor
                        ordenar_recordes(game->recordes, MAX_RECORDES);

                        // salva a alteração no arquivo
                        salvar_recordes(game);
                    }
                }
            }
            // sinaliza que a logica foi atualizada e a tela precisa ser redesenhada
            redesenhar = TRUE; 
        } 

        // interação do usuario
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {

            // se estiver no menu ou na tela de morte o enter inicia outra partida
            if (estado_atual == MENU || estado_atual == GAMEOVER) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    resetar_jogo(game);     
                    estado_atual = JOGANDO; 
                }
            } 

            // se estiver jogando, as setas mudam a direção do vetor (dx, dy)
            // os ifs evitam que a cobra va pra tras em cima do proprio corpo
            else {
                switch (evento.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:    if (game->dy == 0) { game->dx = 0; game->dy = -BLOCO; } break;
                    case ALLEGRO_KEY_DOWN:  if (game->dy == 0) { game->dx = 0; game->dy = BLOCO; } break;
                    case ALLEGRO_KEY_LEFT:  if (game->dx == 0) { game->dx = -BLOCO; game->dy = 0; } break;
                    case ALLEGRO_KEY_RIGHT: if (game->dx == 0) { game->dx = BLOCO; game->dy = 0; } break;
                }
            }
            // esc pra sair do jogo
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) sair = TRUE;
        }

        // pra fechar o jogo. tava bugando
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) sair = TRUE;

        // só renderiza se a fila de eventos estiver vazia. tambem estava bugando
        if (redesenhar && al_is_event_queue_empty(game->fila_eventos)) {

            // fundo preto pra menu e tela de morte
            al_clear_to_color(al_map_rgb(0, 0, 0)); 

            // tela menu inicial
            if (estado_atual == MENU) {
                al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 120, ALLEGRO_ALIGN_CENTER, "SNAKE GAME - UFERSA");
                al_draw_text(game->fonte, al_map_rgb(0, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 80, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para iniciar");
                al_draw_text(game->fonte, al_map_rgb(255, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 20, ALLEGRO_ALIGN_CENTER, "--- TOP 5 RECORDES ---");
                
                // imprime o ranking
                for (int i = 0; i < MAX_RECORDES; i++) {
                    sprintf(texto_ranking, "Posicao %d: %d pts", i + 1, game->recordes[i]);
                    al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 + 10 + (i * 25), ALLEGRO_ALIGN_CENTER, texto_ranking);
                }
            } 

            // tela de morte
            else if (estado_atual == GAMEOVER) {
                al_draw_text(game->fonte, al_map_rgb(255, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 120, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
                al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 90, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para tentar novamente");
                
                // feedback busca binaria
                if (game->encontrou_na_busca != -1) {

                    // soma +1 no indice apenas para leitura
                    sprintf(texto_ranking, "Busca Binaria: Score ja existia na pos %d!", game->encontrou_na_busca + 1);
                    al_draw_text(game->fonte, al_map_rgb(0, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTER, texto_ranking);
                } 
                
                // placar inedito (-1)
                // testa se a pontuação é menor ou igual a ultima do ranking
                else if (game->placar <= game->recordes[MAX_RECORDES - 1]) {
                    al_draw_text(game->fonte, al_map_rgb(255, 50, 50), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTER, "Nenhum recorde batido!");
                } 

                // se nao caiu nos ifs de cima é porque bateu algum recorde
                else {
                    al_draw_text(game->fonte, al_map_rgb(255, 165, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTER, "Busca Binaria: Pontuacao inedita no ranking!");
                }
                
                //imprime o cabecalho ranking
                al_draw_text(game->fonte, al_map_rgb(255, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 10, ALLEGRO_ALIGN_CENTER, "--- RANKING ATUALIZADO ---");
                
               // laço pra imprimir as 5 posições do ranking  
                for (int i = 0; i < MAX_RECORDES; i++) {
                    sprintf(texto_ranking, "Posicao %d: %d pts", i + 1, game->recordes[i]);
                    al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 + 20 + (i * 25), ALLEGRO_ALIGN_CENTER, texto_ranking);
                }
            }

            // se não é menu nem game over, o round esta acontecendo
            else {
                desenhar_jogo(game); 
            }

            // double buffering (nao sei o que é. recomendação)
            al_flip_display();

            // reseta a flag. so vai ser alterada de novo com o timer
            redesenhar = FALSE; 
        }
    }

    destruir_jogo(game);
    return 0;
}
#include "jogo.h"
#include "ranking.h"
#include <stdio.h>
#include <time.h>

int main() {
    SnakeGame* game = criar_jogo();
    if (!game) return -1;

    enum ESTADO estado_atual = MENU; 
    int sair = FALSE;       
    int redesenhar = TRUE;  
    char texto_ranking[30]; 

    srand(time(NULL));      
    al_start_timer(game->timer);

    while (!sair) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(game->fila_eventos, &evento);
        
        if (evento.type == ALLEGRO_EVENT_TIMER) {

            if (estado_atual == JOGANDO) {

                if (mover_cobra(game) == FALSE) {
                    estado_atual = GAMEOVER; 
                    
                    game->encontrou_na_busca = busca_binaria_recordes(game->recordes, MAX_RECORDES, game->placar);

                    if (game->placar > game->recordes[MAX_RECORDES - 1]) {
                        game->recordes[MAX_RECORDES - 1] = game->placar;
                        ordenar_recordes(game->recordes, MAX_RECORDES);
                        salvar_recordes(game);
                    }
                }
            }
            redesenhar = TRUE; 
        } 

        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {

            if (estado_atual == MENU || estado_atual == GAMEOVER) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    resetar_jogo(game);     
                    estado_atual = JOGANDO; 
                }
            } 

            else {
                switch (evento.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:    if (game->dy == 0) { game->dx = 0; game->dy = -BLOCO; } break;
                    case ALLEGRO_KEY_DOWN:  if (game->dy == 0) { game->dx = 0; game->dy = BLOCO; } break;
                    case ALLEGRO_KEY_LEFT:  if (game->dx == 0) { game->dx = -BLOCO; game->dy = 0; } break;
                    case ALLEGRO_KEY_RIGHT: if (game->dx == 0) { game->dx = BLOCO; game->dy = 0; } break;
                }
            }
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) sair = TRUE;
        }

        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) sair = TRUE;

        if (redesenhar && al_is_event_queue_empty(game->fila_eventos)) {

            al_clear_to_color(al_map_rgb(0, 0, 0)); 

            if (estado_atual == MENU) {
                al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 120, ALLEGRO_ALIGN_CENTER, "SNAKE GAME - UFERSA");
                al_draw_text(game->fonte, al_map_rgb(0, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 80, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para iniciar");
                al_draw_text(game->fonte, al_map_rgb(255, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 20, ALLEGRO_ALIGN_CENTER, "--- TOP 5 RECORDES ---");
                
                for (int i = 0; i < MAX_RECORDES; i++) {
                    sprintf(texto_ranking, "Posicao %d: %d pts", i + 1, game->recordes[i]);
                    al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 + 10 + (i * 25), ALLEGRO_ALIGN_CENTER, texto_ranking);
                }
            } 

            else if (estado_atual == GAMEOVER) {
                al_draw_text(game->fonte, al_map_rgb(255, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 120, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
                al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 90, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para tentar novamente");
                
                if (game->encontrou_na_busca != -1) {
                    sprintf(texto_ranking, "Busca Binaria: Score ja existia na pos %d!", game->encontrou_na_busca + 1);
                    al_draw_text(game->fonte, al_map_rgb(0, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTER, texto_ranking);
                } 
                else if (game->placar <= game->recordes[MAX_RECORDES - 1]) {
                    al_draw_text(game->fonte, al_map_rgb(255, 50, 50), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTER, "Nenhum recorde batido!");
                } 
                else {
                    al_draw_text(game->fonte, al_map_rgb(255, 165, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 50, ALLEGRO_ALIGN_CENTER, "Busca Binaria: Pontuacao inedita no ranking!");
                }
                
                al_draw_text(game->fonte, al_map_rgb(255, 255, 0), LARGURA_TELA/2, ALTURA_TELA/2 - 10, ALLEGRO_ALIGN_CENTER, "--- RANKING ATUALIZADO ---");
                
                for (int i = 0; i < MAX_RECORDES; i++) {
                    sprintf(texto_ranking, "Posicao %d: %d pts", i + 1, game->recordes[i]);
                    al_draw_text(game->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2 + 20 + (i * 25), ALLEGRO_ALIGN_CENTER, texto_ranking);
                }
            }
            else {
                desenhar_jogo(game); 
            }

            al_flip_display();
            redesenhar = FALSE; 
        }
    }

    destruir_jogo(game);
    return 0;
}
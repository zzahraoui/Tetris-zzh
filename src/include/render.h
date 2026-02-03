/*
 * render.h - Interface pour le rendu graphique avec SDL2
 */

#ifndef RENDER_H
#define RENDER_H

#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Constantes d'affichage
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BLOCK_SIZE 25
#define GRID_OFFSET_X 250
#define GRID_OFFSET_Y 50

/*
 * Structure Renderer - Contient les ressources SDL
 */
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *font_large;
    bool running;
} Renderer;

/*
 * render_init - Initialise SDL et crée la fenêtre
 */
Renderer *render_init(void);

/*
 * render_destroy - Détruit le renderer et ferme SDL
 */
void render_destroy(Renderer *renderer);

/*
 * render_clear - Efface l'écran
 */
void render_clear(Renderer *renderer);

/*
 * render_present - Affiche le rendu à l'écran
 */
void render_present(Renderer *renderer);

/*
 * render_grid - Dessine la grille de jeu
 */
void render_grid(Renderer *renderer);

/*
 * render_block - Dessine un bloc individuel
 */
void render_block(Renderer *renderer, int x, int y, SDL_Color color);

/*
 * render_piece - Dessine une pièce Tetris
 */
void render_piece(Renderer *renderer, Piece *piece);

/*
 * render_fixed_blocks - Dessine tous les blocs fixés
 */
void render_fixed_blocks(Renderer *renderer, BlockList *blocks);

/*
 * render_ghost_piece - Dessine la pièce fantôme
 */
void render_ghost_piece(Renderer *renderer, GameState *game);

/*
 * render_next_piece - Dessine la prochaine pièce
 */
void render_next_piece(Renderer *renderer, Piece *piece);

/*
 * render_ui - Dessine l'interface utilisateur
 */
void render_ui(Renderer *renderer, GameState *game);

/*
 * render_game_over - Affiche l'écran de game over
 */
void render_game_over(Renderer *renderer, GameState *game);

/*
 * render_pause - Affiche le message de pause
 */
void render_pause(Renderer *renderer);

/*
 * render_text - Dessine du texte avec SDL_ttf
 */
void render_text(Renderer *renderer, const char *text, int x, int y,
                 SDL_Color color, TTF_Font *font);

#endif /* RENDER_H */
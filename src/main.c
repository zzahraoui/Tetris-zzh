/*
 * main.c - Point d'entrée du jeu Tetris
 *
 * Contient:
 * - La boucle principale du jeu
 * - La gestion des événements (clavier)
 * - L'orchestration entre logique et rendu
 */

#include "include/game.h"
#include "include/render.h"
#include <stdio.h>
#include <stdbool.h>

// FPS cible
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

/*
 * Gère les événements clavier
 */
void handle_input(SDL_Event *event, GameState *game, Renderer *renderer)
{
    if (event->type == SDL_QUIT)
    {
        renderer->running = false;
        return;
    }

    if (event->type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event->key.keysym.sym;

        // Game over: seules R et ESC fonctionnent
        if (game->game_over)
        {
            if (key == SDLK_r)
            {
                game_reset(game);
                printf("Nouvelle partie!\n");
            }
            else if (key == SDLK_ESCAPE)
            {
                renderer->running = false;
            }
            return;
        }

        // Contrôles du jeu
        switch (key)
        {
        case SDLK_LEFT:
            game_move_piece(game, -1, 0);
            break;

        case SDLK_RIGHT:
            game_move_piece(game, 1, 0);
            break;

        case SDLK_DOWN:
            // Descente rapide
            if (game_move_piece(game, 0, 1))
            {
                game->score += 1; // Petit bonus
            }
            break;

        case SDLK_UP:
        case SDLK_SPACE:
            // Rotation
            game_rotate_piece(game);
            break;

        case SDLK_w:
        case SDLK_x:
            // Hard drop (chute instantanée)
            game_drop_piece(game);
            break;

        case SDLK_p:
            // Pause
            game_toggle_pause(game);
            if (game->paused)
            {
                printf("=== PAUSE ===\n");
            }
            else
            {
                printf("=== REPRISE ===\n");
            }
            break;

        case SDLK_r:
            // Restart
            game_reset(game);
            printf("Nouvelle partie!\n");
            break;

        case SDLK_ESCAPE:
            // Quitter
            renderer->running = false;
            break;

        default:
            break;
        }
    }
}

/*
 * Affiche les contrôles dans la console
 */
void print_controls(void)
{
    printf("\n");
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║         TETRIS - LISTES CHAÎNÉES          ║\n");
    printf("╠═══════════════════════════════════════════╣\n");
    printf("║  CONTRÔLES:                               ║\n");
    printf("║  ←  → : Déplacer la pièce                ║\n");
    printf("║  ↓    : Descente rapide                  ║\n");
    printf("║  ↑/SPC: Rotation                         ║\n");
    printf("║  W/X  : Hard drop (chute instantanée)    ║\n");
    printf("║  P    : Pause                            ║\n");
    printf("║  R    : Nouvelle partie                  ║\n");
    printf("║  ESC  : Quitter                          ║\n");
    printf("╠═══════════════════════════════════════════╣\n");
    printf("║  SCORING:                                 ║\n");
    printf("║  1 ligne  : 40 × niveau                  ║\n");
    printf("║  2 lignes : 100 × niveau                 ║\n");
    printf("║  3 lignes : 300 × niveau                 ║\n");
    printf("║  4 lignes : 1200 × niveau (TETRIS!)      ║\n");
    printf("╚═══════════════════════════════════════════╝\n");
    printf("\n");
}

/*
 * Affiche les statistiques dans la console (debug)
 */
void print_stats(GameState *game)
{
    static int last_score = -1;

    // N'afficher que si le score change
    if (game->score != last_score)
    {
        printf("Score: %d | Niveau: %d | Lignes: %d | Blocs fixés: %d\n",
               game->score, game->level, game->lines_cleared,
               game->fixed_blocks->count);
        last_score = game->score;
    }
}

/*
 * Fonction principale
 */
int main(int argc, char *argv[])
{
    (void)argc; // Éviter warning unused
    (void)argv;

    printf("Initialisation de Tetris...\n");

    // Initialiser le renderer
    Renderer *renderer = render_init();
    if (renderer == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'initialiser le renderer\n");
        return 1;
    }

    // Initialiser le jeu
    GameState *game = game_init();
    if (game == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'initialiser le jeu\n");
        render_destroy(renderer);
        return 1;
    }

    // Afficher les contrôles
    print_controls();

    // Variables pour le timing
    Uint32 last_time = SDL_GetTicks();

    // Boucle principale
    SDL_Event event;

    while (renderer->running)
    {
        // Calculer delta time
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        // Gérer les événements
        while (SDL_PollEvent(&event))
        {
            handle_input(&event, game, renderer);
        }

        // Mettre à jour la logique du jeu
        game_update(game, delta_time);

        // Afficher les stats (debug)
        print_stats(game);

        // === RENDU ===

        // Effacer l'écran
        render_clear(renderer);

        // Dessiner la grille
        render_grid(renderer);

        // Dessiner les blocs fixés
        render_fixed_blocks(renderer, game->fixed_blocks);

        // Dessiner la pièce fantôme
        if (!game->game_over && !game->paused)
        {
            render_ghost_piece(renderer, game);
        }

        // Dessiner la pièce courante
        if (!game->game_over)
        {
            render_piece(renderer, game->current_piece);
        }

        // Dessiner l'UI
        render_ui(renderer, game);

        // Afficher l'écran de pause
        if (game->paused)
        {
            render_pause(renderer);
        }

        // Afficher l'écran de game over
        if (game->game_over)
        {
            render_game_over(renderer, game);
        }

        // Présenter le rendu
        render_present(renderer);

        // Limiter les FPS
        SDL_Delay(FRAME_DELAY);
    }

    // Nettoyage
    printf("\nFermeture du jeu...\n");
    printf("Score final: %d\n", game->score);

    game_destroy(game);
    render_destroy(renderer);

    printf("Au revoir!\n");

    return 0;
}

/*
 * EXPLICATION DE LA BOUCLE PRINCIPALE:
 * ====================================
 *
 * 1. TIMING:
 *    - On calcule delta_time pour avoir un jeu fluide
 *    - Indépendant de la vitesse du processeur
 *
 * 2. INPUT (Événements):
 *    - SDL_PollEvent récupère tous les événements
 *    - Clavier, souris, fermeture de fenêtre
 *    - handle_input gère la logique associée
 *
 * 3. UPDATE (Logique):
 *    - game_update applique la gravité
 *    - Vérifie les collisions
 *    - Fixe les pièces
 *    - Supprime les lignes complètes
 *
 * 4. RENDER (Affichage):
 *    - render_clear efface l'écran précédent
 *    - On dessine couche par couche:
 *      a. Grille de fond
 *      b. Blocs fixés
 *      c. Pièce fantôme (transparente)
 *      d. Pièce courante
 *      e. UI (score, next piece)
 *      f. Overlays (pause, game over)
 *    - render_present affiche tout à l'écran
 *
 * 5. FPS LIMITING:
 *    - SDL_Delay pour limiter à 60 FPS
 *    - Évite de consommer 100% du CPU
 *
 * Cette architecture sépare bien:
 * - La LOGIQUE (game.c) - Indépendante du rendu
 * - Le RENDU (render.c) - Indépendant de la logique
 * - L'ORCHESTRATION (main.c) - Coordination
 */
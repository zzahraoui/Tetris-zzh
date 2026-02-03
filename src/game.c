/*
 * game.c - Implémentation de la logique du jeu Tetris
 *
 * Ce fichier contient toute la logique métier:
 * - Gestion de la gravité
 * - Détection de collisions
 * - Suppression de lignes
 * - Calcul du score
 */

#include "include/game.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Initialise un nouveau jeu
 */
GameState *game_init(void)
{
    GameState *game = (GameState *)malloc(sizeof(GameState));
    if (game == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'allouer GameState\n");
        return NULL;
    }

    // Initialiser la liste des blocs fixés
    game->fixed_blocks = list_create();
    if (game->fixed_blocks == NULL)
    {
        free(game);
        return NULL;
    }

    // Créer les pièces initiales
    game->current_piece = piece_create_random();
    game->next_piece = piece_create_random();

    if (game->current_piece == NULL || game->next_piece == NULL)
    {
        game_destroy(game);
        return NULL;
    }

    // Initialiser les statistiques
    game->score = 0;
    game->level = 1;
    game->lines_cleared = 0;
    game->game_over = false;
    game->paused = false;
    game->fall_timer = 0.0f;
    game->fall_speed = 1.0f; // 1 seconde par chute au niveau 1

    return game;
}

/*
 * Détruit le jeu et libère toute la mémoire
 */
void game_destroy(GameState *game)
{
    if (game == NULL)
        return;

    if (game->fixed_blocks != NULL)
    {
        list_destroy(game->fixed_blocks);
    }

    if (game->current_piece != NULL)
    {
        piece_destroy(game->current_piece);
    }

    if (game->next_piece != NULL)
    {
        piece_destroy(game->next_piece);
    }

    free(game);
}

/*
 * Vérifie si une pièce entre en collision
 *
 * On parcourt chaque bloc de la pièce et on vérifie:
 * 1. S'il sort de la grille (limites)
 * 2. S'il chevauche un bloc fixé
 */
bool game_check_collision(Piece *piece, BlockList *fixed_blocks, int dx, int dy)
{
    if (piece == NULL || piece->blocks == NULL)
        return true;

    Block *current = piece->blocks->head;

    while (current != NULL)
    {
        int new_x = current->x + dx;
        int new_y = current->y + dy;

        // Vérifier les limites de la grille
        if (new_x < 0 || new_x >= GRID_WIDTH)
        {
            return true; // Collision avec les murs latéraux
        }

        if (new_y >= GRID_HEIGHT)
        {
            return true; // Collision avec le fond
        }

        // Vérifier la collision avec les blocs fixés
        // Note: on ignore les blocs au-dessus de la grille (y < 0)
        if (new_y >= 0)
        {
            if (list_find(fixed_blocks, new_x, new_y) != NULL)
            {
                return true; // Collision avec un bloc fixé
            }
        }

        current = current->next;
    }

    return false; // Pas de collision
}

/*
 * Déplace la pièce courante
 */
bool game_move_piece(GameState *game, int dx, int dy)
{
    if (game == NULL || game->current_piece == NULL)
        return false;
    if (game->game_over || game->paused)
        return false;

    // Vérifier si le mouvement est possible
    if (!game_check_collision(game->current_piece, game->fixed_blocks, dx, dy))
    {
        piece_move(game->current_piece, dx, dy);
        return true;
    }

    // Si on ne peut pas descendre, fixer la pièce
    if (dy > 0)
    {
        game_fix_piece(game);
    }

    return false;
}

/*
 * Fait tourner la pièce courante
 *
 * Algorithme:
 * 1. Créer une copie de la pièce
 * 2. Faire tourner la copie
 * 3. Vérifier les collisions
 * 4. Si OK, appliquer la rotation à la vraie pièce
 * 5. Si collision, essayer des ajustements (wall kicks)
 */
bool game_rotate_piece(GameState *game)
{
    if (game == NULL || game->current_piece == NULL)
        return false;
    if (game->game_over || game->paused)
        return false;

    // Créer une copie pour tester
    Piece *test_piece = piece_copy(game->current_piece);
    if (test_piece == NULL)
        return false;

    // Appliquer la rotation à la copie
    piece_rotate(test_piece);

    // Vérifier si la rotation est possible
    if (!game_check_collision(test_piece, game->fixed_blocks, 0, 0))
    {
        // Rotation OK! Appliquer à la vraie pièce
        piece_rotate(game->current_piece);
        piece_destroy(test_piece);
        return true;
    }

    // Essayer des ajustements (wall kicks)
    // Essai 1: Décaler à droite
    if (!game_check_collision(test_piece, game->fixed_blocks, 1, 0))
    {
        piece_rotate(game->current_piece);
        piece_move(game->current_piece, 1, 0);
        piece_destroy(test_piece);
        return true;
    }

    // Essai 2: Décaler à gauche
    if (!game_check_collision(test_piece, game->fixed_blocks, -1, 0))
    {
        piece_rotate(game->current_piece);
        piece_move(game->current_piece, -1, 0);
        piece_destroy(test_piece);
        return true;
    }

    // Rotation impossible
    piece_destroy(test_piece);
    return false;
}

/*
 * Fixe la pièce courante dans la grille
 *
 * Étapes:
 * 1. Transférer tous les blocs de la pièce dans fixed_blocks
 * 2. Vérifier les lignes complètes
 * 3. Créer une nouvelle pièce
 * 4. Vérifier si le jeu est terminé
 */
void game_fix_piece(GameState *game)
{
    if (game == NULL || game->current_piece == NULL)
        return;

    // Transférer les blocs de la pièce vers fixed_blocks
    Block *current = game->current_piece->blocks->head;
    while (current != NULL)
    {
        list_add(game->fixed_blocks, current->x, current->y, current->color);
        current = current->next;
    }

    // Vérifier et supprimer les lignes complètes
    int lines = game_check_lines(game);
    if (lines > 0)
    {
        // Calcul du score (scoring Tetris classique)
        int points[] = {0, 40, 100, 300, 1200}; // 0, 1, 2, 3, 4 lignes
        game->score += points[lines] * game->level;
        game->lines_cleared += lines;

        // Augmenter le niveau tous les 10 lignes
        game->level = 1 + (game->lines_cleared / 10);
        game->fall_speed = 1.0f / game->level; // Vitesse augmente avec le niveau
    }

    // Détruire l'ancienne pièce
    piece_destroy(game->current_piece);

    // La prochaine pièce devient la courante
    game->current_piece = game->next_piece;
    game->next_piece = piece_create_random();

    // Vérifier si la nouvelle pièce entre en collision (game over)
    if (game_check_collision(game->current_piece, game->fixed_blocks, 0, 0))
    {
        game->game_over = true;
        printf("GAME OVER! Score final: %d\n", game->score);
    }

    // Réinitialiser le timer de chute
    game->fall_timer = 0.0f;
}

/*
 * Vérifie et supprime les lignes complètes
 *
 * Algorithme:
 * 1. Pour chaque ligne Y de bas en haut
 * 2. Compter combien de blocs ont cette coordonnée Y
 * 3. Si count == 10 (ligne pleine):
 *    a. Supprimer tous les blocs de cette ligne
 *    b. Descendre tous les blocs au-dessus (y < Y)
 * 4. Revérifier la même ligne (car les blocs ont descendu)
 */
int game_check_lines(GameState *game)
{
    if (game == NULL)
        return 0;

    int lines_removed = 0;

    // Parcourir de bas en haut
    for (int y = GRID_HEIGHT - 1; y >= 0; y--)
    {
        int count = 0;

        // Compter les blocs sur cette ligne
        Block *current = game->fixed_blocks->head;
        while (current != NULL)
        {
            if (current->y == y)
            {
                count++;
            }
            current = current->next;
        }

        // Ligne complète?
        if (count == GRID_WIDTH)
        {
            lines_removed++;

            // Supprimer tous les blocs de cette ligne
            current = game->fixed_blocks->head;
            while (current != NULL)
            {
                Block *next = current->next;

                if (current->y == y)
                {
                    // Supprimer ce bloc
                    list_remove(game->fixed_blocks, current->x, current->y);
                }
                else if (current->y < y)
                {
                    // Descendre les blocs au-dessus
                    current->y++;
                }

                current = next;
            }

            // Revérifier cette ligne (car elle contient maintenant la ligne au-dessus)
            y++;
        }
    }

    return lines_removed;
}

/*
 * Met à jour la logique du jeu (appelée chaque frame)
 *
 * Gère la gravité: fait tomber la pièce automatiquement
 */
void game_update(GameState *game, float delta_time)
{
    if (game == NULL)
        return;
    if (game->game_over || game->paused)
        return;

    // Incrémenter le timer de chute
    game->fall_timer += delta_time;

    // Vérifier si c'est le moment de faire tomber la pièce
    if (game->fall_timer >= game->fall_speed)
    {
        game_move_piece(game, 0, 1); // Descendre d'une case
        game->fall_timer = 0.0f;
    }
}

/*
 * Hard drop: fait tomber la pièce instantanément
 */
void game_drop_piece(GameState *game)
{
    if (game == NULL || game->current_piece == NULL)
        return;
    if (game->game_over || game->paused)
        return;

    // Descendre jusqu'à la collision
    while (!game_check_collision(game->current_piece, game->fixed_blocks, 0, 1))
    {
        piece_move(game->current_piece, 0, 1);
        game->score += 2; // Bonus pour hard drop
    }

    // Fixer immédiatement
    game_fix_piece(game);
}

/*
 * Active/désactive la pause
 */
void game_toggle_pause(GameState *game)
{
    if (game == NULL)
        return;
    game->paused = !game->paused;
}

/*
 * Réinitialise le jeu
 */
void game_reset(GameState *game)
{
    if (game == NULL)
        return;

    // Vider la grille
    list_clear(game->fixed_blocks);

    // Détruire les anciennes pièces
    if (game->current_piece != NULL)
    {
        piece_destroy(game->current_piece);
    }
    if (game->next_piece != NULL)
    {
        piece_destroy(game->next_piece);
    }

    // Créer de nouvelles pièces
    game->current_piece = piece_create_random();
    game->next_piece = piece_create_random();

    // Réinitialiser les stats
    game->score = 0;
    game->level = 1;
    game->lines_cleared = 0;
    game->game_over = false;
    game->paused = false;
    game->fall_timer = 0.0f;
    game->fall_speed = 1.0f;
}

/*
 * Calcule la position Y de la pièce fantôme
 * (où la pièce atterrira si on la laisse tomber)
 */
int game_get_ghost_y(GameState *game)
{
    if (game == NULL || game->current_piece == NULL)
        return 0;

    // Trouver le Y minimum actuel de la pièce
    int min_y = GRID_HEIGHT;
    Block *current = game->current_piece->blocks->head;
    while (current != NULL)
    {
        if (current->y < min_y)
        {
            min_y = current->y;
        }
        current = current->next;
    }

    // Descendre jusqu'à la collision
    int ghost_offset = 0;
    while (!game_check_collision(game->current_piece, game->fixed_blocks, 0, ghost_offset + 1))
    {
        ghost_offset++;
    }

    return min_y + ghost_offset;
}
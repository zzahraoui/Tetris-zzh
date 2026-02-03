/*
 * game.h - Logique principale du jeu Tetris
 *
 * Gère:
 * - L'état du jeu (grille, score, pièces)
 * - Les collisions
 * - Les lignes complètes
 * - La gravité
 */

#ifndef GAME_H
#define GAME_H

#include "pieces.h"
#include "list.h"
#include <stdbool.h>

// Constantes de la grille
#define GRID_WIDTH 10  // Largeur de la grille (colonnes)
#define GRID_HEIGHT 20 // Hauteur de la grille (lignes)

/*
 * Structure GameState - État complet du jeu
 *
 * Contient toutes les informations nécessaires:
 * - fixed_blocks: Liste de tous les blocs fixés dans la grille
 * - current_piece: La pièce actuellement contrôlée par le joueur
 * - next_piece: La prochaine pièce (affichée dans le HUD)
 * - score: Score actuel du joueur
 * - level: Niveau actuel (affecte la vitesse)
 * - lines_cleared: Nombre total de lignes complétées
 * - game_over: Indicateur de fin de partie
 * - paused: Indicateur de pause
 */
typedef struct
{
    BlockList *fixed_blocks; // Blocs fixés dans la grille
    Piece *current_piece;    // Pièce en mouvement
    Piece *next_piece;       // Prochaine pièce
    int score;               // Score du joueur
    int level;               // Niveau actuel
    int lines_cleared;       // Lignes complétées au total
    bool game_over;          // État de fin de partie
    bool paused;             // État de pause
    float fall_timer;        // Timer pour la gravité
    float fall_speed;        // Vitesse de chute (secondes)
} GameState;

/*
 * game_init - Initialise un nouveau jeu
 *
 * Crée la grille vide, génère les premières pièces
 * et initialise le score à 0
 *
 * Retour: Pointeur vers le nouvel état de jeu
 */
GameState *game_init(void);

/*
 * game_destroy - Détruit le jeu et libère la mémoire
 *
 * Paramètres:
 *   game: L'état du jeu à détruire
 */
void game_destroy(GameState *game);

/*
 * game_update - Met à jour la logique du jeu
 *
 * Appelée à chaque frame:
 * - Applique la gravité
 * - Vérifie les collisions
 * - Fixe les pièces
 * - Vérifie les lignes complètes
 *
 * Paramètres:
 *   game: L'état du jeu
 *   delta_time: Temps écoulé depuis la dernière frame (en secondes)
 */
void game_update(GameState *game, float delta_time);

/*
 * game_move_piece - Déplace la pièce courante
 *
 * Vérifie d'abord si le mouvement est valide (pas de collision)
 *
 * Paramètres:
 *   game: L'état du jeu
 *   dx: Déplacement horizontal (-1 = gauche, +1 = droite)
 *   dy: Déplacement vertical (+1 = bas)
 *
 * Retour: true si le mouvement a été effectué, false sinon
 */
bool game_move_piece(GameState *game, int dx, int dy);

/*
 * game_rotate_piece - Fait tourner la pièce courante
 *
 * Vérifie d'abord si la rotation est possible
 * Si la rotation cause une collision, essaie des ajustements (wall kick)
 *
 * Paramètres:
 *   game: L'état du jeu
 *
 * Retour: true si la rotation a été effectuée, false sinon
 */
bool game_rotate_piece(GameState *game);

/*
 * game_drop_piece - Fait tomber la pièce instantanément (hard drop)
 *
 * Déplace la pièce vers le bas jusqu'à ce qu'elle entre en collision
 * puis la fixe immédiatement
 *
 * Paramètres:
 *   game: L'état du jeu
 */
void game_drop_piece(GameState *game);

/*
 * game_check_collision - Vérifie si une pièce entre en collision
 *
 * Vérifie:
 * 1. Les limites de la grille (murs, fond)
 * 2. Les blocs déjà fixés
 *
 * Paramètres:
 *   piece: La pièce à vérifier
 *   fixed_blocks: Les blocs fixés dans la grille
 *   dx: Décalage horizontal à tester
 *   dy: Décalage vertical à tester
 *
 * Retour: true si collision, false sinon
 */
bool game_check_collision(Piece *piece, BlockList *fixed_blocks, int dx, int dy);

/*
 * game_fix_piece - Fixe la pièce courante dans la grille
 *
 * Transfère tous les blocs de la pièce dans fixed_blocks
 * et crée une nouvelle pièce
 *
 * Paramètres:
 *   game: L'état du jeu
 */
void game_fix_piece(GameState *game);

/*
 * game_check_lines - Vérifie et supprime les lignes complètes
 *
 * Pour chaque ligne:
 * 1. Compte le nombre de blocs
 * 2. Si 10 blocs (ligne pleine):
 *    - Supprime tous les blocs de cette ligne
 *    - Descend tous les blocs au-dessus
 *    - Augmente le score
 *
 * Paramètres:
 *   game: L'état du jeu
 *
 * Retour: Nombre de lignes supprimées
 */
int game_check_lines(GameState *game);

/*
 * game_is_game_over - Vérifie si la partie est terminée
 *
 * La partie est finie si une nouvelle pièce entre directement
 * en collision (la grille est pleine en haut)
 *
 * Paramètres:
 *   game: L'état du jeu
 *
 * Retour: true si game over, false sinon
 */
bool game_is_game_over(GameState *game);

/*
 * game_toggle_pause - Active/désactive la pause
 *
 * Paramètres:
 *   game: L'état du jeu
 */
void game_toggle_pause(GameState *game);

/*
 * game_reset - Réinitialise le jeu
 *
 * Vide la grille, remet le score à 0, génère de nouvelles pièces
 *
 * Paramètres:
 *   game: L'état du jeu à réinitialiser
 */
void game_reset(GameState *game);

/*
 * game_get_ghost_y - Calcule la position Y d'une "pièce fantôme"
 *
 * La pièce fantôme montre où la pièce atterrira si on la laisse tomber
 *
 * Paramètres:
 *   game: L'état du jeu
 *
 * Retour: Position Y de la pièce fantôme
 */
int game_get_ghost_y(GameState *game);

#endif /* GAME_H */
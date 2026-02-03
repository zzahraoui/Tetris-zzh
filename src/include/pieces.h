/*
 * pieces.h - Gestion des pièces Tetris (Tetrominos)
 *
 * Les 7 types de pièces classiques de Tetris
 */

#ifndef PIECES_H
#define PIECES_H

#include "list.h"
#include <stdbool.h>

/*
 * Énumération des types de pièces Tetris
 *
 * Formes:
 * I: ████        Cyan
 * O: ██          Jaune
 *    ██
 * T:  █          Violet
 *    ███
 * S:  ██         Vert
 *    ██
 * Z: ██          Rouge
 *     ██
 * J: █           Bleu
 *    ███
 * L:   █         Orange
 *    ███
 */
typedef enum
{
    PIECE_I,    // Ligne droite (4 blocs alignés)
    PIECE_O,    // Carré (2x2)
    PIECE_T,    // Forme T
    PIECE_S,    // S ou Z inversé
    PIECE_Z,    // Z
    PIECE_J,    // J
    PIECE_L,    // L
    PIECE_COUNT // Nombre total de types (utilisé pour random)
} PieceType;

/*
 * Structure Piece - Représente une pièce Tetris
 *
 * Contient:
 * - blocks: Liste chaînée des 4 blocs qui composent la pièce
 * - type: Type de la pièce (I, O, T, etc.)
 * - rotation: Rotation actuelle (0, 1, 2, 3 pour 0°, 90°, 180°, 270°)
 */
typedef struct
{
    BlockList *blocks; // Liste des 4 blocs
    PieceType type;    // Type de pièce
    int rotation;      // État de rotation (0-3)
} Piece;

/*
 * piece_create - Crée une nouvelle pièce aléatoire
 *
 * La pièce est créée en haut de la grille (x=4, y=0)
 * avec une orientation par défaut
 *
 * Paramètres:
 *   type: Type de pièce à créer
 *
 * Retour: Pointeur vers la nouvelle pièce, ou NULL si échec
 */
Piece *piece_create(PieceType type);

/*
 * piece_create_random - Crée une pièce de type aléatoire
 *
 * Retour: Pointeur vers la nouvelle pièce
 */
Piece *piece_create_random(void);

/*
 * piece_destroy - Détruit une pièce et libère sa mémoire
 *
 * Paramètres:
 *   piece: La pièce à détruire
 */
void piece_destroy(Piece *piece);

/*
 * piece_move - Déplace une pièce
 *
 * Ajoute dx et dy à la position de chaque bloc
 *
 * Paramètres:
 *   piece: La pièce à déplacer
 *   dx: Déplacement horizontal (-1 = gauche, +1 = droite)
 *   dy: Déplacement vertical (+1 = bas)
 */
void piece_move(Piece *piece, int dx, int dy);

/*
 * piece_rotate - Fait tourner la pièce de 90° dans le sens horaire
 *
 * Utilise la formule de rotation:
 *   new_x = center_x - (old_y - center_y)
 *   new_y = center_y + (old_x - center_x)
 *
 * Note: La pièce O (carré) ne tourne pas
 *
 * Paramètres:
 *   piece: La pièce à faire tourner
 */
void piece_rotate(Piece *piece);

/*
 * piece_get_color - Retourne la couleur associée à un type de pièce
 *
 * Paramètres:
 *   type: Type de la pièce
 *
 * Retour: Couleur SDL_Color
 */
SDL_Color piece_get_color(PieceType type);

/*
 * piece_get_center - Calcule le centre de rotation d'une pièce
 *
 * Le centre est calculé comme la moyenne des positions des blocs
 *
 * Paramètres:
 *   piece: La pièce
 *   center_x: Pointeur pour stocker la coordonnée X du centre
 *   center_y: Pointeur pour stocker la coordonnée Y du centre
 */
void piece_get_center(Piece *piece, float *center_x, float *center_y);

/*
 * piece_copy - Crée une copie d'une pièce
 *
 * Utile pour tester une rotation avant de la valider
 *
 * Paramètres:
 *   piece: La pièce à copier
 *
 * Retour: Nouvelle pièce copiée
 */
Piece *piece_copy(Piece *piece);

#endif /* PIECES_H */
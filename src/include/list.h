/*
 * list.h - Interface pour les listes chaînées
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <SDL2/SDL.h> //  Utiliser la vraie définition SDL2

/*
 * Structure Block - Représente un bloc unique dans la grille
 */
typedef struct Block
{
    int x;              // Position horizontale (0-9)
    int y;              // Position verticale (0-19)
    SDL_Color color;    // Couleur RGB du bloc
    struct Block *next; // Pointeur vers le bloc suivant
} Block;

/*
 * Structure BlockList - Liste chaînée de blocs
 */
typedef struct BlockList
{
    Block *head; // Premier élément de la liste
    int count;   // Nombre d'éléments
} BlockList;

// Toutes les autres fonctions restent identiques...
BlockList *list_create(void);
void list_destroy(BlockList *list);
bool list_add(BlockList *list, int x, int y, SDL_Color color);
bool list_remove(BlockList *list, int x, int y);
Block *list_find(BlockList *list, int x, int y);
void list_clear(BlockList *list);
bool list_is_empty(BlockList *list);
BlockList *list_copy(BlockList *list);
void list_print(BlockList *list);
int list_count(BlockList *list);

#endif /* LIST_H */
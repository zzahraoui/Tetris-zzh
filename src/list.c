/*
 * list.c - Implémentation des opérations sur les listes chaînées
 */

#include "include/list.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Crée une nouvelle liste vide
 */
BlockList *list_create(void)
{
    BlockList *list = (BlockList *)malloc(sizeof(BlockList));
    if (list == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'allouer la mémoire pour la liste\n");
        return NULL;
    }

    list->head = NULL;
    list->count = 0;

    return list;
}

/*
 * Détruit une liste et libère toute sa mémoire
 */
void list_destroy(BlockList *list)
{
    if (list == NULL)
        return;

    Block *current = list->head;
    while (current != NULL)
    {
        Block *next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

/*
 * Ajoute un bloc à la fin de la liste
 */
bool list_add(BlockList *list, int x, int y, SDL_Color color)
{
    if (list == NULL)
        return false;

    Block *new_block = (Block *)malloc(sizeof(Block));
    if (new_block == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'allouer un nouveau bloc\n");
        return false;
    }

    new_block->x = x;
    new_block->y = y;
    new_block->color = color;
    new_block->next = NULL;

    if (list->head == NULL)
    {
        list->head = new_block;
    }
    else
    {
        Block *current = list->head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_block;
    }

    list->count++;
    return true;
}

/*
 * Recherche un bloc à une position donnée
 */
Block *list_find(BlockList *list, int x, int y)
{
    if (list == NULL)
        return NULL;

    Block *current = list->head;
    while (current != NULL)
    {
        if (current->x == x && current->y == y)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/*
 * Supprime un bloc de la liste
 */
bool list_remove(BlockList *list, int x, int y)
{
    if (list == NULL || list->head == NULL)
        return false;

    Block *current = list->head;
    Block *previous = NULL;

    while (current != NULL)
    {
        if (current->x == x && current->y == y)
        {
            if (previous == NULL)
            {
                list->head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            free(current);
            list->count--;
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

/*
 * Vide la liste sans la détruire
 */
void list_clear(BlockList *list)
{
    if (list == NULL)
        return;

    Block *current = list->head;
    while (current != NULL)
    {
        Block *next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
    list->count = 0;
}

/*
 * Vérifie si la liste est vide
 */
bool list_is_empty(BlockList *list)
{
    return (list == NULL || list->head == NULL);
}

/*
 * Crée une copie complète de la liste
 */
BlockList *list_copy(BlockList *list)
{
    if (list == NULL)
        return NULL;

    BlockList *new_list = list_create();
    if (new_list == NULL)
        return NULL;

    Block *current = list->head;
    while (current != NULL)
    {
        if (!list_add(new_list, current->x, current->y, current->color))
        {
            list_destroy(new_list);
            return NULL;
        }
        current = current->next;
    }

    return new_list;
}

/*
 * Affiche le contenu de la liste (pour le debug)
 */
void list_print(BlockList *list)
{
    if (list == NULL)
    {
        printf("Liste NULL\n");
        return;
    }

    printf("Liste (%d éléments): ", list->count);

    Block *current = list->head;
    while (current != NULL)
    {
        printf("(%d,%d) ", current->x, current->y);
        current = current->next;
    }
    printf("\n");
}

/*
 * Retourne le nombre d'éléments dans la liste
 */
int list_count(BlockList *list)
{
    if (list == NULL)
        return 0;
    return list->count;
}
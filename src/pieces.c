/*
 * pieces.c - Implémentation de la gestion des pièces Tetris
 */

#include "include/pieces.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

/*
 * Définition des couleurs pour chaque type de pièce
 */
SDL_Color piece_get_color(PieceType type)
{
    SDL_Color color;
    switch (type)
    {
    case PIECE_I:
        color.r = 0;
        color.g = 255;
        color.b = 255;
        color.a = 255;
        break;
    case PIECE_O:
        color.r = 255;
        color.g = 255;
        color.b = 0;
        color.a = 255;
        break;
    case PIECE_T:
        color.r = 128;
        color.g = 0;
        color.b = 128;
        color.a = 255;
        break;
    case PIECE_S:
        color.r = 0;
        color.g = 255;
        color.b = 0;
        color.a = 255;
        break;
    case PIECE_Z:
        color.r = 255;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        break;
    case PIECE_J:
        color.r = 0;
        color.g = 0;
        color.b = 255;
        color.a = 255;
        break;
    case PIECE_L:
        color.r = 255;
        color.g = 165;
        color.b = 0;
        color.a = 255;
        break;
    default:
        color.r = 255;
        color.g = 255;
        color.b = 255;
        color.a = 255;
        break;
    }
    return color;
}

/*
 * Crée une pièce selon son type
 */
Piece *piece_create(PieceType type)
{
    Piece *piece = (Piece *)malloc(sizeof(Piece));
    if (piece == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'allouer la pièce\n");
        return NULL;
    }

    piece->blocks = list_create();
    if (piece->blocks == NULL)
    {
        free(piece);
        return NULL;
    }

    piece->type = type;
    piece->rotation = 0;

    SDL_Color color = piece_get_color(type);
    int start_x = 4;
    int start_y = 0;

    switch (type)
    {
    case PIECE_I:
        list_add(piece->blocks, start_x - 1, start_y, color);
        list_add(piece->blocks, start_x, start_y, color);
        list_add(piece->blocks, start_x + 1, start_y, color);
        list_add(piece->blocks, start_x + 2, start_y, color);
        break;

    case PIECE_O:
        list_add(piece->blocks, start_x, start_y, color);
        list_add(piece->blocks, start_x + 1, start_y, color);
        list_add(piece->blocks, start_x, start_y + 1, color);
        list_add(piece->blocks, start_x + 1, start_y + 1, color);
        break;

    case PIECE_T:
        list_add(piece->blocks, start_x, start_y, color);
        list_add(piece->blocks, start_x - 1, start_y + 1, color);
        list_add(piece->blocks, start_x, start_y + 1, color);
        list_add(piece->blocks, start_x + 1, start_y + 1, color);
        break;

    case PIECE_S:
        list_add(piece->blocks, start_x, start_y, color);
        list_add(piece->blocks, start_x + 1, start_y, color);
        list_add(piece->blocks, start_x - 1, start_y + 1, color);
        list_add(piece->blocks, start_x, start_y + 1, color);
        break;

    case PIECE_Z:
        list_add(piece->blocks, start_x - 1, start_y, color);
        list_add(piece->blocks, start_x, start_y, color);
        list_add(piece->blocks, start_x, start_y + 1, color);
        list_add(piece->blocks, start_x + 1, start_y + 1, color);
        break;

    case PIECE_J:
        list_add(piece->blocks, start_x - 1, start_y, color);
        list_add(piece->blocks, start_x - 1, start_y + 1, color);
        list_add(piece->blocks, start_x, start_y + 1, color);
        list_add(piece->blocks, start_x + 1, start_y + 1, color);
        break;

    case PIECE_L:
        list_add(piece->blocks, start_x + 1, start_y, color);
        list_add(piece->blocks, start_x - 1, start_y + 1, color);
        list_add(piece->blocks, start_x, start_y + 1, color);
        list_add(piece->blocks, start_x + 1, start_y + 1, color);
        break;

    default:
        break;
    }

    return piece;
}

/*
 * Crée une pièce aléatoire
 */
Piece *piece_create_random(void)
{
    static bool seeded = false;
    if (!seeded)
    {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    PieceType type = (PieceType)(rand() % PIECE_COUNT);
    return piece_create(type);
}

/*
 * Détruit une pièce
 */
void piece_destroy(Piece *piece)
{
    if (piece == NULL)
        return;

    if (piece->blocks != NULL)
    {
        list_destroy(piece->blocks);
    }

    free(piece);
}

/*
 * Déplace une pièce
 */
void piece_move(Piece *piece, int dx, int dy)
{
    if (piece == NULL || piece->blocks == NULL)
        return;

    Block *current = piece->blocks->head;
    while (current != NULL)
    {
        current->x += dx;
        current->y += dy;
        current = current->next;
    }
}

/*
 * Calcule le centre de rotation
 */
void piece_get_center(Piece *piece, float *center_x, float *center_y)
{
    if (piece == NULL || piece->blocks == NULL)
    {
        *center_x = 0;
        *center_y = 0;
        return;
    }

    float sum_x = 0, sum_y = 0;
    int count = 0;

    Block *current = piece->blocks->head;
    while (current != NULL)
    {
        sum_x += current->x;
        sum_y += current->y;
        count++;
        current = current->next;
    }

    *center_x = sum_x / count;
    *center_y = sum_y / count;
}

/*
 * Fait tourner la pièce de 90° dans le sens horaire
 */
void piece_rotate(Piece *piece)
{
    if (piece == NULL || piece->blocks == NULL)
        return;

    if (piece->type == PIECE_O)
        return;

    float cx, cy;
    piece_get_center(piece, &cx, &cy);

    Block *current = piece->blocks->head;
    while (current != NULL)
    {
        int old_x = current->x;
        int old_y = current->y;

        current->x = (int)(cx - (old_y - cy));
        current->y = (int)(cy + (old_x - cx));

        current = current->next;
    }

    piece->rotation = (piece->rotation + 1) % 4;
}

/*
 * Copie une pièce
 */
Piece *piece_copy(Piece *piece)
{
    if (piece == NULL)
        return NULL;

    Piece *copy = (Piece *)malloc(sizeof(Piece));
    if (copy == NULL)
        return NULL;

    copy->type = piece->type;
    copy->rotation = piece->rotation;
    copy->blocks = list_copy(piece->blocks);

    if (copy->blocks == NULL)
    {
        free(copy);
        return NULL;
    }

    return copy;
}
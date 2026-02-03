/*
 * render.c - Implémentation du rendu graphique avec SDL2
 */

#include "include/render.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Initialise SDL et crée la fenêtre
 */
Renderer *render_init(void)
{
    Renderer *renderer = (Renderer *)malloc(sizeof(Renderer));
    if (renderer == NULL)
    {
        fprintf(stderr, "Erreur: Impossible d'allouer le renderer\n");
        return NULL;
    }

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        free(renderer);
        return NULL;
    }

    // ✅ Initialiser SDL_ttf
    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        free(renderer);
        return NULL;
    }

    // Créer la fenêtre
    renderer->window = SDL_CreateWindow(
        "Tetris - Listes Chaînées",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (renderer->window == NULL)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        free(renderer);
        return NULL;
    }

    // Créer le renderer
    renderer->renderer = SDL_CreateRenderer(
        renderer->window,
        -1,
        SDL_RENDERER_ACCELERATED);

    if (renderer->renderer == NULL)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        TTF_Quit();
        SDL_Quit();
        free(renderer);
        return NULL;
    }

    // ✅ Charger les polices (on utilise une police système Windows)
    // Essayons plusieurs polices courantes
    renderer->font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (renderer->font == NULL)
    {
        // Essayer une autre police
        renderer->font = TTF_OpenFont("C:/Windows/Fonts/calibri.ttf", 24);
    }
    if (renderer->font == NULL)
    {
        // Dernière tentative
        renderer->font = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 24);
    }

    renderer->font_large = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 48);
    if (renderer->font_large == NULL)
    {
        renderer->font_large = TTF_OpenFont("C:/Windows/Fonts/calibri.ttf", 48);
    }
    if (renderer->font_large == NULL)
    {
        renderer->font_large = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 48);
    }

    if (renderer->font == NULL || renderer->font_large == NULL)
    {
        fprintf(stderr, "Avertissement: Impossible de charger les polices\n");
        // On continue quand même
    }

    renderer->running = true;

    printf("SDL2 initialisé avec succès!\n");
    printf("Fenêtre: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);

    return renderer;
}

/*
 * Détruit le renderer
 */
void render_destroy(Renderer *renderer)
{
    if (renderer == NULL)
        return;

    // ✅ Libérer les polices
    if (renderer->font != NULL)
    {
        TTF_CloseFont(renderer->font);
    }
    if (renderer->font_large != NULL)
    {
        TTF_CloseFont(renderer->font_large);
    }

    if (renderer->renderer != NULL)
    {
        SDL_DestroyRenderer(renderer->renderer);
    }

    if (renderer->window != NULL)
    {
        SDL_DestroyWindow(renderer->window);
    }

    TTF_Quit(); // ✅ Ajouter
    SDL_Quit();
    free(renderer);
}
/*
 * Efface l'écran avec une couleur de fond
 */
void render_clear(Renderer *renderer)
{
    if (renderer == NULL)
        return;

    // Couleur de fond: noir
    SDL_SetRenderDrawColor(renderer->renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer->renderer);
}

/*
 * Affiche le rendu
 */
void render_present(Renderer *renderer)
{
    if (renderer == NULL)
        return;
    SDL_RenderPresent(renderer->renderer);
}

/*
 * Dessine la grille
 */
void render_grid(Renderer *renderer)
{
    if (renderer == NULL)
        return;

    // Couleur des lignes de la grille: gris foncé
    SDL_SetRenderDrawColor(renderer->renderer, 60, 60, 70, 255);

    // Dessiner les lignes verticales
    for (int x = 0; x <= GRID_WIDTH; x++)
    {
        int px = GRID_OFFSET_X + x * BLOCK_SIZE;
        SDL_RenderDrawLine(renderer->renderer, // ✅ SDL_RenderDrawLine
                           px, GRID_OFFSET_Y,
                           px, GRID_OFFSET_Y + GRID_HEIGHT * BLOCK_SIZE);
    }

    // Dessiner les lignes horizontales
    for (int y = 0; y <= GRID_HEIGHT; y++)
    {
        int py = GRID_OFFSET_Y + y * BLOCK_SIZE;
        SDL_RenderDrawLine(renderer->renderer, // ✅ SDL_RenderDrawLine
                           GRID_OFFSET_X, py,
                           GRID_OFFSET_X + GRID_WIDTH * BLOCK_SIZE, py);
    }

    // Contour épais de la grille: blanc
    SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 255, 255);
    SDL_Rect border = {// ✅ SDL_Rect au lieu de SDL_FRect
                       GRID_OFFSET_X - 2,
                       GRID_OFFSET_Y - 2,
                       GRID_WIDTH * BLOCK_SIZE + 4,
                       GRID_HEIGHT * BLOCK_SIZE + 4};
    SDL_RenderDrawRect(renderer->renderer, &border); // ✅ SDL_RenderDrawRect
}

/*
 * Dessine un bloc individuel
 */
void render_block(Renderer *renderer, int x, int y, SDL_Color color)
{
    if (renderer == NULL)
        return;

    // Calculer la position en pixels
    int px = GRID_OFFSET_X + x * BLOCK_SIZE;
    int py = GRID_OFFSET_Y + y * BLOCK_SIZE;

    // Dessiner le bloc rempli
    SDL_SetRenderDrawColor(renderer->renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {px + 1, py + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2}; // ✅ SDL_Rect
    SDL_RenderFillRect(renderer->renderer, &rect);

    // Dessiner le contour (effet 3D simple)
    SDL_SetRenderDrawColor(renderer->renderer,
                           color.r + 40, color.g + 40, color.b + 40, 255);
    SDL_RenderDrawRect(renderer->renderer, &rect); // ✅ SDL_RenderDrawRect
}

/*
 * Dessine une pièce
 */
void render_piece(Renderer *renderer, Piece *piece)
{
    if (renderer == NULL || piece == NULL || piece->blocks == NULL)
        return;

    Block *current = piece->blocks->head;
    while (current != NULL)
    {
        // Ne dessiner que les blocs visibles (y >= 0)
        if (current->y >= 0)
        {
            render_block(renderer, current->x, current->y, current->color);
        }
        current = current->next;
    }
}

/*
 * Dessine tous les blocs fixés
 */
void render_fixed_blocks(Renderer *renderer, BlockList *blocks)
{
    if (renderer == NULL || blocks == NULL)
        return;

    Block *current = blocks->head;
    while (current != NULL)
    {
        render_block(renderer, current->x, current->y, current->color);
        current = current->next;
    }
}

/*
 * Dessine la pièce fantôme (ghost piece)
 */
void render_ghost_piece(Renderer *renderer, GameState *game)
{
    if (renderer == NULL || game == NULL || game->current_piece == NULL)
        return;

    // Créer une copie de la pièce
    Piece *ghost = piece_copy(game->current_piece);
    if (ghost == NULL)
        return;

    // Descendre jusqu'à la position finale
    while (!game_check_collision(ghost, game->fixed_blocks, 0, 1))
    {
        piece_move(ghost, 0, 1);
    }

    // Dessiner avec transparence
    Block *current = ghost->blocks->head;
    while (current != NULL)
    {
        if (current->y >= 0)
        {
            int px = GRID_OFFSET_X + current->x * BLOCK_SIZE;
            int py = GRID_OFFSET_Y + current->y * BLOCK_SIZE;

            // Contour transparent
            SDL_SetRenderDrawColor(renderer->renderer,
                                   current->color.r,
                                   current->color.g,
                                   current->color.b,
                                   100);
            SDL_Rect rect = {px + 2, py + 2, BLOCK_SIZE - 4, BLOCK_SIZE - 4}; // ✅ SDL_Rect
            SDL_RenderDrawRect(renderer->renderer, &rect);                    // ✅ SDL_RenderDrawRect
        }
        current = current->next;
    }

    piece_destroy(ghost);
}

/*
 * Dessine la prochaine pièce
 */
void render_next_piece(Renderer *renderer, Piece *piece)
{
    if (renderer == NULL || piece == NULL)
        return;

    int offset_x = 550;
    int offset_y = 150;

    // Titre "NEXT"
    SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 255, 255);
    SDL_Rect title_rect = {offset_x, offset_y - 30, 80, 20}; // ✅ SDL_Rect
    SDL_RenderDrawRect(renderer->renderer, &title_rect);     // ✅ SDL_RenderDrawRect

    // Cadre
    SDL_Rect frame = {offset_x - 10, offset_y - 10, 120, 120}; // ✅ SDL_Rect
    SDL_RenderDrawRect(renderer->renderer, &frame);            // ✅ SDL_RenderDrawRect

    // Dessiner la pièce
    Block *current = piece->blocks->head;
    while (current != NULL)
    {
        int px = offset_x + (current->x - 3) * 20;
        int py = offset_y + current->y * 20;

        SDL_SetRenderDrawColor(renderer->renderer,
                               current->color.r,
                               current->color.g,
                               current->color.b,
                               255);
        SDL_Rect rect = {px, py, 18, 18}; // ✅ SDL_Rect
        SDL_RenderFillRect(renderer->renderer, &rect);

        current = current->next;
    }
}

/*
 * Dessine l'interface utilisateur (score, niveau, etc.)
 */
void render_ui(Renderer *renderer, GameState *game)
{
    if (renderer == NULL || game == NULL)
        return;

    SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 255, 255);

    // Zone Score
    SDL_Rect score_frame = {50, 100, 150, 80};            // ✅ SDL_Rect
    SDL_RenderDrawRect(renderer->renderer, &score_frame); // ✅ SDL_RenderDrawRect

    // Zone Level
    SDL_Rect level_frame = {50, 200, 150, 80};            // ✅ SDL_Rect
    SDL_RenderDrawRect(renderer->renderer, &level_frame); // ✅ SDL_RenderDrawRect

    // Zone Lines
    SDL_Rect lines_frame = {50, 300, 150, 80};            // ✅ SDL_Rect
    SDL_RenderDrawRect(renderer->renderer, &lines_frame); // ✅ SDL_RenderDrawRect

    // Prochaine pièce
    render_next_piece(renderer, game->next_piece);
}

/*
 * Affiche l'écran de game over
 */
/*
 * Dessine un chiffre simple (0-9)
 */
void render_digit(Renderer *renderer, int digit, int x, int y, int size)
{
    SDL_Rect rects[7]; // Maximum 7 segments pour afficher un chiffre
    int count = 0;

    int w = size;
    int h = size / 5;

    // Définir les segments pour chaque chiffre (style 7-segments simplifié)
    switch (digit)
    {
    case 0:
        rects[count++] = (SDL_Rect){x, y, w, h};         // Top
        rects[count++] = (SDL_Rect){x, y, h, w};         // Left top
        rects[count++] = (SDL_Rect){x + w - h, y, h, w}; // Right top
        rects[count++] = (SDL_Rect){x, y + w - h, w, h}; // Bottom
        break;
    case 1:
        rects[count++] = (SDL_Rect){x + w - h, y, h, w}; // Right
        break;
    case 2:
        rects[count++] = (SDL_Rect){x, y, w, h};             // Top
        rects[count++] = (SDL_Rect){x + w - h, y, h, w / 2}; // Right top
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h}; // Middle
        rects[count++] = (SDL_Rect){x, y + w / 2, h, w / 2}; // Left bottom
        rects[count++] = (SDL_Rect){x, y + w - h, w, h};     // Bottom
        break;
    case 3:
        rects[count++] = (SDL_Rect){x, y, w, h};             // Top
        rects[count++] = (SDL_Rect){x + w - h, y, h, w};     // Right
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h}; // Middle
        rects[count++] = (SDL_Rect){x, y + w - h, w, h};     // Bottom
        break;
    case 4:
        rects[count++] = (SDL_Rect){x, y, h, w / 2};         // Left top
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h}; // Middle
        rects[count++] = (SDL_Rect){x + w - h, y, h, w};     // Right
        break;
    case 5:
        rects[count++] = (SDL_Rect){x, y, w, h};                     // Top
        rects[count++] = (SDL_Rect){x, y, h, w / 2};                 // Left top
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h};         // Middle
        rects[count++] = (SDL_Rect){x + w - h, y + w / 2, h, w / 2}; // Right bottom
        rects[count++] = (SDL_Rect){x, y + w - h, w, h};             // Bottom
        break;
    case 6:
        rects[count++] = (SDL_Rect){x, y, w, h};                     // Top
        rects[count++] = (SDL_Rect){x, y, h, w};                     // Left
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h};         // Middle
        rects[count++] = (SDL_Rect){x + w - h, y + w / 2, h, w / 2}; // Right bottom
        rects[count++] = (SDL_Rect){x, y + w - h, w, h};             // Bottom
        break;
    case 7:
        rects[count++] = (SDL_Rect){x, y, w, h};         // Top
        rects[count++] = (SDL_Rect){x + w - h, y, h, w}; // Right
        break;
    case 8:
        rects[count++] = (SDL_Rect){x, y, w, h};             // Top
        rects[count++] = (SDL_Rect){x, y, h, w};             // Left
        rects[count++] = (SDL_Rect){x + w - h, y, h, w};     // Right
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h}; // Middle
        rects[count++] = (SDL_Rect){x, y + w - h, w, h};     // Bottom
        break;
    case 9:
        rects[count++] = (SDL_Rect){x, y, w, h};             // Top
        rects[count++] = (SDL_Rect){x, y, h, w / 2};         // Left top
        rects[count++] = (SDL_Rect){x + w - h, y, h, w};     // Right
        rects[count++] = (SDL_Rect){x, y + w / 2 - h, w, h}; // Middle
        rects[count++] = (SDL_Rect){x, y + w - h, w, h};     // Bottom
        break;
    }

    for (int i = 0; i < count; i++)
    {
        SDL_RenderFillRect(renderer->renderer, &rects[i]);
    }
}

/*
 * Dessine un nombre entier
 */
void render_number(Renderer *renderer, int number, int x, int y, int digit_size)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", number);

    int spacing = digit_size + 10;
    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            render_digit(renderer, buffer[i] - '0', x + i * spacing, y, digit_size);
        }
    }
}

/*
 * Dessine du texte stylisé (majuscules uniquement)
 */
void render_text_styled(Renderer *renderer, const char *text, int x, int y, int char_width, int char_height)
{
    int offset = 0;

    for (int i = 0; text[i] != '\0'; i++)
    {
        char c = text[i];

        if (c == ' ')
        {
            offset += char_width;
            continue;
        }

        // Dessiner des rectangles pour représenter les lettres
        SDL_Rect base = {x + offset, y, char_width - 5, char_height};

        switch (c)
        {
        case 'G':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'A':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'M':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'E':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'O':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'V':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'R':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'S':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'C':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'N':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'I':
            SDL_Rect thin = {x + offset + char_width / 3, y, char_width / 3, char_height};
            SDL_RenderFillRect(renderer->renderer, &thin);
            break;
        case 'L':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'P':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'U':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'Y':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        case 'Z':
            SDL_RenderFillRect(renderer->renderer, &base);
            break;
        default:
            // Caractère non géré
            break;
        }

        offset += char_width;
    }
}

/*
 * Affiche l'écran de game over avec toutes les infos
 */
/*
 * Affiche l'écran de game over
 */
/*
 * Dessine du texte à l'écran
 */
void render_text(Renderer *renderer, const char *text, int x, int y,
                 SDL_Color color, TTF_Font *font)
{
    if (renderer == NULL || text == NULL || font == NULL)
        return;

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL)
    {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer->renderer, surface);
    if (texture == NULL)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer->renderer, texture, NULL, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
/*
 * Affiche l'écran de game over avec du vrai texte
 */
void render_game_over(Renderer *renderer, GameState *game)
{
    if (renderer == NULL)
        return;

    // Fond semi-transparent
    SDL_SetRenderDrawColor(renderer->renderer, 0, 0, 0, 230);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer->renderer, &overlay);

    // Panneau principal
    int panel_width = 500;
    int panel_height = 400;
    int panel_x = (WINDOW_WIDTH - panel_width) / 2;
    int panel_y = (WINDOW_HEIGHT - panel_height) / 2;

    // Fond du panneau
    SDL_SetRenderDrawColor(renderer->renderer, 40, 40, 50, 255);
    SDL_Rect panel = {panel_x, panel_y, panel_width, panel_height};
    SDL_RenderFillRect(renderer->renderer, &panel);

    // Bordure rouge
    SDL_SetRenderDrawColor(renderer->renderer, 255, 0, 0, 255);
    for (int i = 0; i < 5; i++)
    {
        SDL_Rect border = {panel_x + i, panel_y + i, panel_width - 2 * i, panel_height - 2 * i};
        SDL_RenderDrawRect(renderer->renderer, &border);
    }

    // Si les polices ne sont pas chargées, afficher des rectangles
    if (renderer->font_large == NULL || renderer->font == NULL)
    {
        SDL_SetRenderDrawColor(renderer->renderer, 255, 0, 0, 255);
        SDL_Rect title = {panel_x + 100, panel_y + 40, 300, 60};
        SDL_RenderFillRect(renderer->renderer, &title);

        printf("\n=== GAME OVER ===\n");
        printf("Score final: %d\n", game->score);
        printf("Niveau atteint: %d\n", game->level);
        printf("Lignes complétées: %d\n", game->lines_cleared);
        return;
    }

    // ✅ TITRE "GAME OVER"
    SDL_Color red = {255, 50, 50, 255};
    render_text(renderer, "GAME OVER", panel_x + 120, panel_y + 40, red, renderer->font_large);

    // Ligne de séparation
    SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 255, 255);
    SDL_Rect sep = {panel_x + 50, panel_y + 110, panel_width - 100, 2};
    SDL_RenderFillRect(renderer->renderer, &sep);

    // ✅ SCORE
    int info_y = panel_y + 140;
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};

    render_text(renderer, "Score:", panel_x + 60, info_y, yellow, renderer->font);

    char score_str[32];
    snprintf(score_str, sizeof(score_str), "%d", game->score);
    render_text(renderer, score_str, panel_x + 280, info_y, white, renderer->font);

    // ✅ NIVEAU
    info_y += 50;
    SDL_Color cyan = {0, 255, 255, 255};

    render_text(renderer, "Niveau:", panel_x + 60, info_y, cyan, renderer->font);

    char level_str[32];
    snprintf(level_str, sizeof(level_str), "%d", game->level);
    render_text(renderer, level_str, panel_x + 280, info_y, white, renderer->font);

    // ✅ LIGNES
    info_y += 50;
    SDL_Color green = {0, 255, 0, 255};

    render_text(renderer, "Lignes:", panel_x + 60, info_y, green, renderer->font);

    char lines_str[32];
    snprintf(lines_str, sizeof(lines_str), "%d", game->lines_cleared);
    render_text(renderer, lines_str, panel_x + 280, info_y, white, renderer->font);

    // ✅ INSTRUCTIONS
    info_y += 80;
    SDL_Color orange = {255, 165, 0, 255};

    render_text(renderer, "Appuyez sur  R  pour rejouer", panel_x + 80, info_y, orange, renderer->font);

    SDL_Color gray = {200, 200, 200, 255};
    render_text(renderer, "ou  ESC  pour quitter", panel_x + 110, info_y + 35, gray, renderer->font);

    // Console backup
    printf("\n╔════════════════════════════════╗\n");
    printf("║       GAME OVER                ║\n");
    printf("╠════════════════════════════════╣\n");
    printf("║  Score final: %-16d ║\n", game->score);
    printf("║  Niveau atteint: %-13d ║\n", game->level);
    printf("║  Lignes complétées: %-10d ║\n", game->lines_cleared);
    printf("╠════════════════════════════════╣\n");
    printf("║  [R] Rejouer                   ║\n");
    printf("║  [ESC] Quitter                 ║\n");
    printf("╚════════════════════════════════╝\n");
}
/*
 * Affiche le message de pause
 */
void render_pause(Renderer *renderer)
{
    if (renderer == NULL)
        return;

    SDL_SetRenderDrawColor(renderer->renderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; // ✅ SDL_Rect
    SDL_RenderFillRect(renderer->renderer, &overlay);

    SDL_SetRenderDrawColor(renderer->renderer, 255, 255, 0, 255);
    SDL_Rect pause_frame = {// ✅ SDL_Rect
                            WINDOW_WIDTH / 2 - 100,
                            WINDOW_HEIGHT / 2 - 30,
                            200,
                            60};
    SDL_RenderDrawRect(renderer->renderer, &pause_frame); // ✅ SDL_RenderDrawRect
}
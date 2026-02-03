# 🎮 TETRIS - Documentation Complète


---
[tetris ](https://i.pinimg.com/1200x/79/9e/89/799e895007946b723b0e0ef3c1275589.jpg)

## 📋 Table des matières
1. [Architecture du Projet](#architecture)
2. [Concepts des Listes Chaînées](#concepts)
3. [Structure des Fichiers](#structure)
4. [Installation et Compilation](#installation)
5. [Logique du Jeu](#logique)
6. [Guide de Test](#testing)

---

## 🏗️ Architecture du Projet {#architecture}

```
tetris/
├── src/
│   ├── main.c           # Point d'entrée, boucle principale
│   ├── list.c           # Implémentation des listes chaînées
│   ├── pieces.c         # Gestion des pièces Tetris (tetrominos)
│   ├── game.c           # Logique du jeu (collision, rotation, lignes)
│   └── render.c         # Rendu graphique SDL3
├── include/
│   ├── list.h           # Interface des listes chaînées
│   ├── pieces.h         # Définitions des pièces
│   ├── game.h           # Interface de la logique de jeu
│   └── render.h         # Interface du rendu
├── Makefile             # Script de compilation
└── README.md            # Ce fichier
```

---

## 🔗 Concepts des Listes Chaînées {#concepts}

### Pourquoi des listes chaînées pour Tetris ?

#### 1. **Représentation dynamique de la pièce active**
```
Pièce en forme de L:
┌─┐
│ │ → (x=4, y=0) → (x=4, y=1) → (x=4, y=2) → (x=5, y=2) → NULL
└─┘
```
Chaque bloc de la pièce est un nœud dans une liste chaînée.

#### 2. **Grille de jeu flexible**
Au lieu d'un tableau 2D statique, on utilise une liste de blocs fixés:
```
Grille (20x10):
Liste des blocs occupés:
(2,19) → (3,19) → (4,19) → (5,18) → (6,18) → NULL
```

**Avantages:**
- Mémoire efficace (on stocke seulement les blocs occupés)
- Insertion/suppression rapide
- Détection de lignes complètes simplifiée

#### 3. **Animation des lignes complètes**
Quand une ligne est complète, on peut la parcourir et l'animer avant de la supprimer.

---

## 📁 Structure des Fichiers Détaillée {#structure}

### 1. `include/list.h` - Interface des Listes

```c
typedef struct Block {
    int x;              // Position X (0-9)
    int y;              // Position Y (0-19)
    SDL_Color color;    // Couleur du bloc
    struct Block *next; // Pointeur vers le bloc suivant
} Block;

typedef struct BlockList {
    Block *head;        // Premier élément
    int count;          // Nombre d'éléments
} BlockList;
```

**Opérations clés:**
- `list_create()` - Crée une nouvelle liste vide
- `list_add()` - Ajoute un bloc à la fin
- `list_remove()` - Supprime un bloc spécifique
- `list_find()` - Recherche un bloc par position
- `list_destroy()` - Libère toute la mémoire

### 2. `include/pieces.h` - Définition des Pièces

```c
typedef enum {
    PIECE_I,  // Ligne droite  ████
    PIECE_O,  // Carré         ██
    PIECE_T,  // T             ██
    PIECE_S,  // S zigzag      ███
    PIECE_Z,  // Z zigzag       ██
    PIECE_J,  // J              ███
    PIECE_L   // L             ███
} PieceType;

typedef struct {
    BlockList *blocks;  // Liste des 4 blocs
    PieceType type;
    int rotation;       // 0, 90, 180, 270
} Piece;
```

### 3. `include/game.h` - Logique du Jeu

```c
typedef struct {
    BlockList *fixed_blocks;    // Blocs fixés dans la grille
    Piece *current_piece;       // Pièce en mouvement
    Piece *next_piece;          // Prochaine pièce
    int score;
    int level;
    bool game_over;
} GameState;
```

**Fonctions principales:**
- `game_init()` - Initialise le jeu
- `game_update()` - Met à jour la logique (gravité, collisions)
- `game_move_piece()` - Déplace la pièce (gauche/droite)
- `game_rotate_piece()` - Rotation de la pièce
- `game_check_lines()` - Détecte et supprime les lignes complètes
- `game_fix_piece()` - Fixe la pièce dans la grille

---

## 🛠️ Installation et Compilation {#installation}

### Prérequis

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential libsdl3-dev
```

**macOS:**
```bash
brew install sdl3
```

**Windows (MSYS2):**
```bash
pacman -S mingw-w64-x86_64-SDL3
```

### Compilation

```bash
# Créer les dossiers nécessaires
mkdir -p obj

# Compiler le projet
make

# Lancer le jeu
make run

# Nettoyer
make clean
```

---

## 🎯 Logique du Jeu Expliquée {#logique}

### Boucle Principale (main.c)

```
INITIALISATION
    ↓
┌─────────────────┐
│  BOUCLE DE JEU  │
├─────────────────┤
│ 1. Gestion Input│ ← Clavier (←, →, ↓, ESPACE)
│ 2. Mise à jour  │ ← Gravité, Collisions
│ 3. Rendu        │ ← Affichage SDL3
└─────────────────┘
    ↓
GAME OVER / QUIT
```

### Flux de Données avec Listes

#### A. **Création d'une nouvelle pièce**
```
1. Allouer BlockList pour la pièce
2. Selon le type (I, O, T, S, Z, J, L):
   - Ajouter 4 blocs avec list_add()
   - Position initiale: centre en haut (x=4, y=0)
3. Assigner une couleur

Exemple Pièce T:
   █
  ███
  
Liste: (4,0) → (3,1) → (4,1) → (5,1) → NULL
```

#### B. **Détection de collision**
```c
bool check_collision(Piece *piece, BlockList *fixed, int dx, int dy) {
    Block *current = piece->blocks->head;
    
    while (current != NULL) {
        int new_x = current->x + dx;
        int new_y = current->y + dy;
        
        // Hors limites?
        if (new_x < 0 || new_x >= 10 || new_y >= 20)
            return true;
            
        // Collision avec bloc fixé?
        if (list_find(fixed, new_x, new_y) != NULL)
            return true;
            
        current = current->next;
    }
    return false;
}
```

#### C. **Fixer la pièce dans la grille**
```
Quand la pièce touche le fond:

1. Parcourir la liste des blocs de la pièce
2. Pour chaque bloc:
   - list_add(fixed_blocks, bloc->x, bloc->y, bloc->color)
3. Détruire la pièce courante: list_destroy(piece->blocks)
4. Créer une nouvelle pièce
```

#### D. **Vérification des lignes complètes**
```
Pour chaque ligne y de 0 à 19:
    Compter combien de blocs ont cette coordonnée y
    
    Si count == 10 (ligne pleine):
        1. Supprimer tous les blocs de cette ligne
           → list_remove() pour chaque bloc
        2. Descendre tous les blocs au-dessus
           → Pour chaque bloc avec bloc->y < y_removed:
               bloc->y++
        3. Score += 100 * level
```

**Algorithme optimisé:**
```c
void game_check_lines(GameState *game) {
    for (int y = 19; y >= 0; y--) {
        int count = 0;
        Block *current = game->fixed_blocks->head;
        
        // Compter les blocs sur cette ligne
        while (current != NULL) {
            if (current->y == y) count++;
            current = current->next;
        }
        
        if (count == 10) {
            // Ligne complète! Supprimer
            current = game->fixed_blocks->head;
            while (current != NULL) {
                Block *next = current->next;
                if (current->y == y)
                    list_remove(game->fixed_blocks, current->x, current->y);
                else if (current->y < y)
                    current->y++;  // Descendre
                current = next;
            }
            game->score += 100;
            y++;  // Revérifier cette ligne
        }
    }
}
```

---

## 🧪 Guide de Test {#testing}

### Tests Unitaires des Listes

**Test 1: Création et destruction**
```c
BlockList *list = list_create();
assert(list->count == 0);
assert(list->head == NULL);
list_destroy(list);
```

**Test 2: Ajout de blocs**
```c
BlockList *list = list_create();
SDL_Color red = {255, 0, 0, 255};

list_add(list, 5, 10, red);
assert(list->count == 1);
assert(list->head->x == 5);
assert(list->head->y == 10);

list_add(list, 6, 10, red);
assert(list->count == 2);
```

**Test 3: Recherche**
```c
Block *found = list_find(list, 5, 10);
assert(found != NULL);
assert(found->x == 5);

Block *not_found = list_find(list, 99, 99);
assert(not_found == NULL);
```

### Tests de Jeu

**Test 1: Collision latérale**
```
Déplacer la pièce jusqu'au bord gauche (x=0)
Tenter de déplacer encore à gauche
→ La pièce ne doit pas bouger
```

**Test 2: Rotation près d'un mur**
```
Placer une pièce I verticale contre le bord droit
Tenter une rotation
→ La rotation doit être bloquée ou la pièce ajustée
```

**Test 3: Ligne complète**
```
1. Placer 9 blocs manuellement sur la ligne 19
2. Faire descendre une pièce pour compléter la ligne
3. Vérifier que:
   - La ligne disparaît
   - Le score augmente
   - Les blocs au-dessus descendent
```

### Commandes de Debug

Ajouter dans le code:
```c
// Dans game.c
void game_print_debug(GameState *game) {
    printf("=== DEBUG INFO ===\n");
    printf("Score: %d\n", game->score);
    printf("Fixed blocks: %d\n", game->fixed_blocks->count);
    printf("Current piece blocks: %d\n", 
           game->current_piece->blocks->count);
    
    // Afficher la grille ASCII
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            if (list_find(game->fixed_blocks, x, y))
                printf("█");
            else
                printf("·");
        }
        printf("\n");
    }
}
```

### Contrôles du Jeu

- **←** : Déplacer à gauche
- **→** : Déplacer à droite
- **↓** : Descente rapide
- **ESPACE** : Rotation horaire
- **P** : Pause
- **ESC** : Quitter

---

## 📊 Analyse de la Complexité

### Opérations sur les Listes

| Opération | Complexité | Explication |
|-----------|-----------|-------------|
| `list_add()` | O(n) | Parcours jusqu'à la fin |
| `list_find()` | O(n) | Recherche linéaire |
| `list_remove()` | O(n) | Recherche + suppression |
| `list_destroy()` | O(n) | Libération de tous les nœuds |

### Améliorations Possibles

1. **Table de hachage** pour la grille → O(1) pour find()
2. **Liste doublement chaînée** → suppression en O(1) si on a le nœud
3. **Liste triée par Y** → optimisation de check_lines()

---

## 🎓 Points Clés pour la Présentation au Prof

### 1. Application Pratique des Listes
- Gestion dynamique de la mémoire
- Structures de taille variable
- Parcours, insertion, suppression

### 2. Avantages par Rapport à un Tableau
```
Tableau 2D (20x10 = 200 cases):
- Mémoire fixe: 200 * sizeof(Cell)
- Beaucoup d'espace vide gaspillé

Liste Chaînée:
- Mémoire: n * sizeof(Block) où n = blocs occupés
- Typiquement n < 80, donc ~40% de l'espace
```

### 3. Opérations Complexes Simplifiées
- Suppression de ligne: simple parcours et removal
- Descente des blocs: modification directe des nœuds
- Pas de décalage de tableau lourd

---

## 🚀 Extensions Possibles

1. **Hard Drop** - Faire tomber instantanément
2. **Hold** - Garder une pièce en réserve
3. **Ghost Piece** - Aperçu de la position finale
4. **Combo System** - Bonus pour lignes multiples
5. **Effets visuels** - Particules, flash sur ligne complète

---

## 📚 Ressources

- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/FrontPage)
- [Tetris Guideline](https://tetris.wiki/Tetris_Guideline)
- Cours sur les listes chaînées en C

---

**Bon courage pour ton projet! N'hésite pas à poser des questions. 🎮**
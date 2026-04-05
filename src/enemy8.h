#ifndef ENEMY8_H
#define ENEMY8_H

#include <genesis.h>
#include "enemies.h"
#include "game.h"

#define ENEMY8_WIDTH              128
#define ENEMY8_HEIGHT             320
#define ENEMY8_SPRITE_AREA_WIDTH   32
#define ENEMY8_SPRITE_AREA_HEIGHT  32
#define ENEMY8_COLS                 4   // colunas visuais (0-3)
#define ENEMY8_ROWS                10   // linhas (0-9)
#define ENEMY8_UNIQUE_COLS          2   // sprites únicos por linha (col 0 e col 1)
#define ENEMY8_SPRITE_COUNT        20   // ROWS * UNIQUE_COLS

typedef struct {
    s16     x;             // X do inimigo (canto esquerdo)
    s16     y;             // Y do inimigo (topo)
    Enemy  *base;          // ponteiro para o Enemy genérico
    Sprite *sprites[ENEMY8_SPRITE_COUNT]; // sprites[row*2 + sprite_col]
    Sprite *head_sprite;   // sprite da área da cabeça (colisão)
    u32     visible_mask;  // bit idx: 1 = sprites[idx] está alocado
} Enemy8Wrapper;

void ENEMY8_create(Enemy *enemy);
void ENEMY8_move(Enemy8Wrapper *w, s16 x, s16 y
);
void ENEMY8_destroy(Enemy8Wrapper *w);
void ENEMY8_draw(Enemy8Wrapper *w);

#endif
#include <genesis.h>
#include "game.h"
#include "resources.h"
#include "enemies.h"

#define ENEMY8_WIDTH              128
#define ENEMY8_HEIGHT             320
#define ENEMY8_SPRITE_AREA_WIDTH   32
#define ENEMY8_SPRITE_AREA_HEIGHT  32
#define ENEMY8_ROWS                10
#define ENEMY8_UNIQUE_COLS          2
#define ENEMY8_SPRITE_COUNT        20   // ROWS * UNIQUE_COLS

// Layout visual das 4 colunas na tela:
//  col 0  x+0   → sprites_normal[row*2+0]  flipH=FALSE
//  col 1  x+32  → sprites_normal[row*2+1]  flipH=FALSE
//  col 2  x+64  → sprites_flip  [row*2+1]  flipH=TRUE
//  col 3  x+96  → sprites_flip  [row*2+0]  flipH=TRUE
//
// Head sprite (16px cada, 32px total centrados na nave de 128px):
//  head esquerda:  x+48  flipH=FALSE
//  head direita:   x+64  flipH=TRUE

// HEAD_OFFSET_X: (128 - 32) / 2 = 48  (32px = 2 * 16px das duas metades do head)
#define ENEMY8_HEAD_OFFSET_X   48
#define ENEMY8_HEAD_WIDTH      16   // largura de cada metade do head sprite

static SpriteDefinition *enemy8_sprs[ENEMY8_SPRITE_COUNT] = {
    &enemy_0008_001, &enemy_0008_002, &enemy_0008_003, &enemy_0008_004,
    &enemy_0008_005, &enemy_0008_006, &enemy_0008_007, &enemy_0008_008,
    &enemy_0008_009, &enemy_0008_010, &enemy_0008_011, &enemy_0008_012,
    &enemy_0008_013, &enemy_0008_014, &enemy_0008_015, &enemy_0008_016,
    &enemy_0008_017, &enemy_0008_018, &enemy_0008_019, &enemy_0008_020
};

static SpriteDefinition *enemy8_head_area           = &enemy_0008_01;
static SpriteDefinition *enemy8_head_area_destroyed = &enemy_0008_02;

typedef struct {
    bool    active;
    s16     x;
    s16     y;
    Enemy  *base;
    Sprite *sprites_normal[ENEMY8_SPRITE_COUNT];
    Sprite *sprites_flip  [ENEMY8_SPRITE_COUNT];
    u32     visible_mask_normal;
    u32     visible_mask_flip;
    Sprite *head_sprite_left;   // metade esquerda do head (flipH=FALSE)
    Sprite *head_sprite_right;  // metade direita  do head (flipH=TRUE)
} Enemy8Wrapper;

static Enemy8Wrapper enemy8_wrappers[MAX_ENEMIES];

// -----------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------

static inline bool rect_h_visible(s16 x, s16 w) {
    return (x + w > 0) && (x < GAME_WINDOW_WIDTH);
}

static inline bool rect_v_visible(s16 y, s16 h) {
    return (y + h > 0) && (y < GAME_WINDOW_HEIGHT);
}

static void enemy8_sync_one(
    Sprite **sprites, u32 *mask, s16 idx,
    SpriteDefinition *def, s16 draw_x, s16 draw_y, bool flipH
) {
    bool visible = rect_h_visible(draw_x, ENEMY8_SPRITE_AREA_WIDTH) &&
                   rect_v_visible(draw_y, ENEMY8_SPRITE_AREA_HEIGHT);

    if (visible) {
        if (*mask & (1u << idx)) {
            SPR_setPosition(sprites[idx], draw_x, draw_y);
            SPR_setHFlip(sprites[idx], flipH);
        } else {
            sprites[idx] = SPR_addSprite(
                def, draw_x, draw_y,
                TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, flipH)
            );
            if (sprites[idx]) *mask |= (1u << idx);
        }
    } else {
        if (*mask & (1u << idx)) {
            SPR_releaseSprite(sprites[idx]);
            sprites[idx] = NULL;
            *mask &= ~(1u << idx);
        }
    }
}

// Sincroniza um dos dois head sprites (esq ou dir)
// head_sprite: ponteiro para o Sprite* (pode ser NULL)
// Retorna o novo Sprite* (pode ser NULL se fora da tela)
static Sprite* enemy8_sync_head(
    Sprite *spr, SpriteDefinition *def,
    s16 draw_x, s16 draw_y, bool flipH, s16 head_h
) {
    bool visible = rect_h_visible(draw_x, ENEMY8_HEAD_WIDTH) &&
                   rect_v_visible(draw_y, head_h);

    if (visible) {
        if (spr) {
            SPR_setPosition(spr, draw_x, draw_y);
            SPR_setHFlip(spr, flipH);
        } else {
            spr = SPR_addSprite(
                def, draw_x, draw_y,
                TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, flipH)
            );
        }
    } else {
        if (spr) {
            SPR_releaseSprite(spr);
            spr = NULL;
        }
    }
    return spr;
}

// -----------------------------------------------------------------
// API pública
// -----------------------------------------------------------------

void ENEMY8_initAll(void) {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        enemy8_wrappers[i].active = FALSE;
    }
}

Enemy8Wrapper* ENEMY8_getWrapper(Enemy *enemy) {
    return &enemy8_wrappers[enemy->index];
}

void ENEMY8_create(Enemy *enemy) {
    Enemy8Wrapper *w = &enemy8_wrappers[enemy->index];

    w->active               = TRUE;
    w->x                    = enemy->x;
    w->y                    = enemy->y;
    w->base                 = enemy;
    w->visible_mask_normal  = 0;
    w->visible_mask_flip    = 0;
    w->head_sprite_left     = NULL;
    w->head_sprite_right    = NULL;

    for (s16 i = 0; i < ENEMY8_SPRITE_COUNT; i++) {
        w->sprites_normal[i] = NULL;
        w->sprites_flip[i]   = NULL;
    }

    // Head esquerda: x+48, sem flip
    w->head_sprite_left = SPR_addSprite(
        enemy8_head_area,
        enemy->x + ENEMY8_HEAD_OFFSET_X,
        enemy->y,
        TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, FALSE)
    );
    // Head direita: x+64, com flip
    w->head_sprite_right = SPR_addSprite(
        enemy8_head_area,
        enemy->x + ENEMY8_HEAD_OFFSET_X + ENEMY8_HEAD_WIDTH,
        enemy->y,
        TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, TRUE)
    );

    // O campo sprite do Enemy aponta para o head esquerdo (usado pela colisão)
    enemy->sprite = w->head_sprite_left;

    ENEMY8_draw(w);
}

void ENEMY8_draw(Enemy8Wrapper *w) {
    // head_01 tem 64px de altura
    s16 head_h = 64;

    for (s16 row = 0; row < ENEMY8_ROWS; row++) {
        s16 draw_y = w->y + row * ENEMY8_SPRITE_AREA_HEIGHT;

        for (s16 sc = 0; sc < ENEMY8_UNIQUE_COLS; sc++) {
            s16 idx = row * ENEMY8_UNIQUE_COLS + sc;

            // Colunas 0 e 1: sem flip
            s16 draw_x_normal = w->x + sc * ENEMY8_SPRITE_AREA_WIDTH;
            enemy8_sync_one(
                w->sprites_normal, &w->visible_mask_normal,
                idx, enemy8_sprs[idx],
                draw_x_normal, draw_y, FALSE
            );

            // Colunas 2 e 3: com flip
            // sc=0 → col visual 3 → x+96
            // sc=1 → col visual 2 → x+64
            s16 draw_x_flip = w->x + ((sc == 0) ? 3 : 2) * ENEMY8_SPRITE_AREA_WIDTH;
            enemy8_sync_one(
                w->sprites_flip, &w->visible_mask_flip,
                idx, enemy8_sprs[idx],
                draw_x_flip, draw_y, TRUE
            );
        }
    }

    // Head sprites (sincronizados dinamicamente)
    w->head_sprite_left = enemy8_sync_head(
        w->head_sprite_left, enemy8_head_area,
        w->x + ENEMY8_HEAD_OFFSET_X,
        w->y + 16, FALSE, head_h
    );
    w->head_sprite_right = enemy8_sync_head(
        w->head_sprite_right, enemy8_head_area,
        w->x + ENEMY8_HEAD_OFFSET_X + ENEMY8_HEAD_WIDTH,
        w->y + 16, TRUE, head_h
    );

    // Mantém enemy->sprite apontando para o head esquerdo (colisão)
    if (w->base) w->base->sprite = w->head_sprite_left;
}

void ENEMY8_move(Enemy8Wrapper *w, s16 x, s16 y) {
    w->x       = x;
    w->y       = y;
    w->base->x = x;
    w->base->y = y;
    ENEMY8_draw(w);
}

static void enemy8_release_head(Enemy8Wrapper *w) {
    if (w->head_sprite_left)  { SPR_releaseSprite(w->head_sprite_left);  w->head_sprite_left  = NULL; }
    if (w->head_sprite_right) { SPR_releaseSprite(w->head_sprite_right); w->head_sprite_right = NULL; }
}

void ENEMY8_destroy(Enemy8Wrapper *w) {
    for (s16 i = 0; i < ENEMY8_SPRITE_COUNT; i++) {
        if (w->visible_mask_normal & (1u << i)) {
            SPR_releaseSprite(w->sprites_normal[i]);
            w->sprites_normal[i] = NULL;
        }
        if (w->visible_mask_flip & (1u << i)) {
            SPR_releaseSprite(w->sprites_flip[i]);
            w->sprites_flip[i] = NULL;
        }
    }
    w->visible_mask_normal = 0;
    w->visible_mask_flip   = 0;

    enemy8_release_head(w);

    // Mostra head destruída (duas metades)
    w->head_sprite_left = SPR_addSprite(
        enemy8_head_area_destroyed,
        w->x + ENEMY8_HEAD_OFFSET_X,
        w->y,
        TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, FALSE)
    );
    w->head_sprite_right = SPR_addSprite(
        enemy8_head_area_destroyed,
        w->x + ENEMY8_HEAD_OFFSET_X + ENEMY8_HEAD_WIDTH,
        w->y,
        TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, TRUE)
    );

    w->active = FALSE;
}
#include "enemy8.h"

// Layout visual:
//  col 0 = sprite único col 0, flipH=FALSE
//  col 1 = sprite único col 1, flipH=FALSE
//  col 2 = sprite único col 1, flipH=TRUE   <- espelho de col 1
//  col 3 = sprite único col 0, flipH=TRUE   <- espelho de col 0
//
// Índice do sprite único: idx = row * 2 + sprite_col  (sprite_col = 0 ou 1)
// visible_mask: bit idx indica se sprites[idx] está alocado

static SpriteDefinition *enemy8_sprs[ENEMY8_SPRITE_COUNT] = {
    &enemy_0008_001, &enemy_0008_002, &enemy_0008_003, &enemy_0008_004,
    &enemy_0008_005, &enemy_0008_006, &enemy_0008_007, &enemy_0008_008,
    &enemy_0008_009, &enemy_0008_010, &enemy_0008_011, &enemy_0008_012,
    &enemy_0008_013, &enemy_0008_014, &enemy_0008_015, &enemy_0008_016,
    &enemy_0008_017, &enemy_0008_018, &enemy_0008_019, &enemy_0008_020
};

static SpriteDefinition *enemy8_head_area           = &enemy_0008_01;
static SpriteDefinition *enemy8_head_area_destroyed = &enemy_0008_02;

// base->x e base->y são o X,Y da cabeça (única área com colisão)


// -----------------------------------------------------------------
// Helpers internos
// -----------------------------------------------------------------

static inline bool rect_h_visible(s16 x, s16 w) {
    return (x + w > 0) && (x < GAME_WINDOW_WIDTH);
}

static inline bool rect_v_visible(s16 y, s16 h) {
    return (y + h > 0) && (y < GAME_WINDOW_HEIGHT);
}

// Sincroniza um sprite único (identificado por row e sprite_col 0..1).
// Cada sprite único cobre 2 colunas visuais (normal + espelhada).
// Usamos uma única instância Sprite*, com flipH conforme necessário.
// Prioridade: coluna normal se visível, senão espelhada.
static void enemy8_sync_sprite(Enemy8Wrapper *w, s16 row, s16 sc) {
    s16 idx = row * ENEMY8_UNIQUE_COLS + sc;

    // Colunas visuais deste sprite único
    s16 vcol_normal = (sc == 0) ? 0 : 1;
    s16 vcol_flip   = (sc == 0) ? 3 : 2;

    s16 draw_x_normal = w->x + vcol_normal * ENEMY8_SPRITE_AREA_WIDTH;
    s16 draw_x_flip   = w->x + vcol_flip   * ENEMY8_SPRITE_AREA_WIDTH;
    s16 draw_y        = w->y + row * ENEMY8_SPRITE_AREA_HEIGHT;

    bool v_ok     = rect_v_visible(draw_y, ENEMY8_SPRITE_AREA_HEIGHT);
    bool h_normal = rect_h_visible(draw_x_normal, ENEMY8_SPRITE_AREA_WIDTH);
    bool h_flip   = rect_h_visible(draw_x_flip,   ENEMY8_SPRITE_AREA_WIDTH);

    bool should_show = v_ok && (h_normal || h_flip);

    if (should_show) {
        s16  draw_x = h_normal ? draw_x_normal : draw_x_flip;
        bool flipH  = h_normal ? FALSE : TRUE;

        if (w->visible_mask & (1u << idx)) {
            // Já alocado: só atualiza posição e flip
            SPR_setPosition(w->sprites[idx], draw_x, draw_y);
            SPR_setHFlip(w->sprites[idx], flipH);
        } else {
            // Aloca
            w->sprites[idx] = SPR_addSprite(
                enemy8_sprs[idx],
                draw_x, draw_y,
                TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, flipH)
            );
            if (w->sprites[idx]) {
                w->visible_mask |= (1u << idx);
            }
        }
    } else {
        // Fora da tela: desaloca
        if (w->visible_mask & (1u << idx)) {
            SPR_releaseSprite(w->sprites[idx]);
            w->sprites[idx]  = NULL;
            w->visible_mask &= ~(1u << idx);
        }
    }
}

// -----------------------------------------------------------------
// API pública
// -----------------------------------------------------------------

void ENEMY8_create(Enemy *enemy) {
    Enemy8Wrapper *w = MEM_alloc(sizeof(Enemy8Wrapper));
    if (!w) return;

    w->x            = enemy->x;
    w->y            = enemy->y;
    w->base         = enemy;
    w->visible_mask = 0;
    w->head_sprite  = NULL;

    for (s16 i = 0; i < ENEMY8_SPRITE_COUNT; i++) {
        w->sprites[i] = NULL;
    }

    // Head sprite: 16px de largura, centrado horizontalmente na nave (128/2 - 16/2 = 56)
    w->head_sprite = SPR_addSprite(
        enemy8_head_area,
        enemy->x + 56,
        enemy->y,
        TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, FALSE)
    );
    enemy->sprite = w->head_sprite;

    ENEMY8_draw(w);
}

void ENEMY8_draw(Enemy8Wrapper *w) {
    for (s16 row = 0; row < ENEMY8_ROWS; row++) {
        for (s16 sc = 0; sc < ENEMY8_UNIQUE_COLS; sc++) {
            enemy8_sync_sprite(w, row, sc);
        }
    }

    if (w->head_sprite) {
        SPR_setPosition(w->head_sprite, w->x + 56, w->y);
    }
}

void ENEMY8_move(Enemy8Wrapper *w, s16 x, s16 y) {
    w->x = x;
    w->y = y;
    w->base->x = x;
    w->base->y = y;
    ENEMY8_draw(w);
}

void ENEMY8_destroy(Enemy8Wrapper *w) {
    for (s16 i = 0; i < ENEMY8_SPRITE_COUNT; i++) {
        if (w->visible_mask & (1u << i)) {
            SPR_releaseSprite(w->sprites[i]);
            w->sprites[i] = NULL;
        }
    }
    w->visible_mask = 0;

    if (w->head_sprite) {
        SPR_releaseSprite(w->head_sprite);
        w->head_sprite = SPR_addSprite(
            enemy8_head_area_destroyed,
            w->x + 56,
            w->y,
            TILE_ATTR(BACKGROUND_PALLETE, TRUE, FALSE, FALSE)
        );
    }

    MEM_free(w);
}

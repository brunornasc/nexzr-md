#include <genesis.h>
#include "enemy.h"
#include "resources.h"
#include "game.h"
#include "bullet.h"

Enemy enemies[MAX_ENEMIES];
static u8 enemy_free_index[MAX_ENEMIES];
static s8 enemy_top_index = -1;

// --- Enemy8 (big ship drawn on BG plane) ---
#define ENEMY8_WIDTH_TILES   16
#define ENEMY8_HEIGHT_TILES  40
#define ENEMY8_VRAM_INDEX    (TILE_USER_INDEX + 300)

typedef struct {
    bool    active;
    s16     x;
    s16     y;
    Enemy  *base;
    Sprite *head_left;
    Sprite *head_right;
} Enemy8Wrapper;

static Enemy8Wrapper enemy8_wrappers[20];
static bool enemy8_vram_loaded = FALSE;

static void enemy8_prepare_vram() {
    if (enemy8_vram_loaded) return;
    VDP_loadTileSet(enemy_0008.tileset, ENEMY8_VRAM_INDEX, DMA);
    enemy8_vram_loaded = TRUE;
}

static void ENEMY8_create(Enemy *enemy, bool isEnemy) {
    enemy8_prepare_vram();
    Enemy8Wrapper *w = &enemy8_wrappers[enemy->index];
    w->active = TRUE;
    w->x = enemy->x;
    w->y = enemy->y;
    w->base = enemy;

    VDP_setTileMapEx(BG_B, enemy_0008.tilemap,
                     TILE_ATTR_FULL(BACKGROUND_PALLETE, isEnemy, 0, 0, ENEMY8_VRAM_INDEX),
                     w->x/8, w->y/8, 0, 0, ENEMY8_WIDTH_TILES, ENEMY8_HEIGHT_TILES, CPU);

    w->head_left = SPR_addSprite(&enemy_0008_01, w->x + 48, w->y + 16,
                                 TILE_ATTR(BACKGROUND_PALLETE, isEnemy, FALSE, FALSE));
    w->head_right = SPR_addSprite(&enemy_0008_01, w->x + 64, w->y + 16,
                                  TILE_ATTR(BACKGROUND_PALLETE, isEnemy, FALSE, TRUE));

    enemy->sprite = w->head_left;

    SPR_setDepth(w->head_left, SPR_MIN_DEPTH);
    SPR_setDepth(w->head_right, SPR_MIN_DEPTH);
    PAL_setPalette(BACKGROUND_PALLETE, enemy_0008.palette->data, DMA);
}

static void ENEMY8_destroy_by_index(u8 idx) {
    Enemy8Wrapper *w = &enemy8_wrappers[idx];
    VDP_clearPlane(BG_B, TRUE);
    if (w->head_left)  SPR_releaseSprite(w->head_left);
    if (w->head_right) SPR_releaseSprite(w->head_right);
    w->active = FALSE;
}

// --- Enemy defaults (factory) ---
typedef struct {
    s16 width;
    s16 height;
    s16 y_speed;
    s16 x_speed;
    EnemyType type;
    s16 health;
    const SpriteDefinition *sprite;
    const SpriteDefinition *bulletSprite;
    const ExplosionAnimationDefinition *explosionDefinition;
    u16 score_points;
    u8 max_frames;
    bool useMiscPalette;
    u16 accentColors[5];
    u8 accentColorIndex;
    u8 paletteAccentIndex;
} EnemyDefaults;

static const ExplosionAnimationDefinition EXPLOSION1_DEFAULTS = {
    &enemy_explosion, 4
};

static const ExplosionAnimationDefinition EXPLOSION2_DEFAULTS = {
    &enemy_explosion_big, 5
};

static const EnemyDefaults ENEMY1_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_1, 1, &enemy_0001, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 5, false
};

static const EnemyDefaults ENEMY2_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_2, 1, &enemy_0002, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 5, false
};

static const EnemyDefaults ENEMY3_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_3, 1, &enemy_0003, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 6, false
};

static const EnemyDefaults ENEMY4_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_4, 1, &enemy_0004, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 6, false
};

static const EnemyDefaults ENEMY5_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_5, 1, &enemy_0005, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 4, false
};

static const EnemyDefaults ENEMY6_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_6, 1, &enemy_0006, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 4, false
};

static const EnemyDefaults ENEMY7_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_7, 1, &enemy_0007, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 2, false
};

static const EnemyDefaults ENEMY9_DEFAULTS = {
    32, 32, 0, 0, ENEMY_TYPE_9, 5, &enemy_0009, &enemy_bullet_001, &EXPLOSION2_DEFAULTS, 60, 3, true
};

static const EnemyDefaults ENEMY_TELEPORT_SMALL_DEFAULTS = {
    16, 16, 0, 0, ENEMY_TYPE_TELEPORT_SMALL, 0, &teleport_small, &enemy_bullet_001, &EXPLOSION1_DEFAULTS, 60, 4, true
};
static void ENEMYFACTORY_initEnemy(Enemy *e, EnemyType type, s16 x, s16 y) {
    e->x = x;
    e->y = y;
    e->angle = 0;
    e->spriteIndex = 0;
    e->active = true;
    e->inverted = false;

    const EnemyDefaults *d;

    switch (type) {
        case ENEMY_TYPE_1: d = &ENEMY1_DEFAULTS; break;
        case ENEMY_TYPE_2: d = &ENEMY2_DEFAULTS; break;
        case ENEMY_TYPE_3: d = &ENEMY3_DEFAULTS; break;
        case ENEMY_TYPE_4: d = &ENEMY4_DEFAULTS; break;
        case ENEMY_TYPE_5: d = &ENEMY5_DEFAULTS; break;
        case ENEMY_TYPE_6: d = &ENEMY6_DEFAULTS; break;
        case ENEMY_TYPE_7: d = &ENEMY7_DEFAULTS; break;
        case ENEMY_TYPE_9: d = &ENEMY9_DEFAULTS; break;
        case ENEMY_TYPE_TELEPORT_SMALL: d = &ENEMY_TELEPORT_SMALL_DEFAULTS; break;
        default: return;
    }

    e->width              = d->width;
    e->height             = d->height;
    e->y_speed            = d->y_speed;
    e->x_speed            = d->x_speed;
    e->type               = d->type;
    e->health             = d->health;
    e->sprite             = NULL;
    e->spriteDefinition   = (SpriteDefinition*)d->sprite;
    e->score_points       = d->score_points;
    e->bulletSprite       = (SpriteDefinition*)d->bulletSprite;
    e->explosionDefinition = (ExplosionAnimationDefinition*)d->explosionDefinition;
    e->destroying         = false;
    e->max_frames         = d->max_frames;
    e->useMiscPalette     = d->useMiscPalette;
}

// --- Static helpers ---
static void ENEMY_destroyAnim(Enemy* enemy);

void ENEMY_gotHit(Enemy* enemy, u8 damage) {
    enemy->health -= damage;

    if (enemy->health <= 0) {
        enemy->destroying = true;
        enemy->spriteIndex = 0;
        SPR_releaseSprite(enemy->sprite);
        enemy->sprite = SPR_addSprite(
            enemy->explosionDefinition->sprite,
            enemy->x,
            enemy->y,
            TILE_ATTR(ENEMY_PALLETE, TRUE, FALSE, FALSE)
        );

        ENEMY_destroyAnim(enemy);
        Game_addGameScore(enemy->score_points);
    }
}

void ENEMY_deactivate(Enemy* enemy) {
    if (!enemy->active) return;

    if (enemy->type == ENEMY_TYPE_8) {
        ENEMY8_destroy_by_index(enemy->index);
    }

    enemy->active = false;
    enemy->destroying = false;

    if (enemy->sprite) {
        SPR_releaseSprite(enemy->sprite);
        enemy->sprite = NULL;
    }

    enemy_free_index[++enemy_top_index] = enemy->index;
}

void ENEMY_initializeAll() {
    enemy_top_index = -1;

    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        memset(&enemies[i], 0, sizeof(Enemy));
        enemies[i].active = false;
        enemies[i].sprite = NULL;
        enemies[i].spriteDefinition = NULL;
        enemies[i].explosionDefinition = NULL;
        enemies[i].bulletSprite = NULL;
        enemies[i].health = 0;
        enemies[i].index = i;
        enemy_free_index[++enemy_top_index] = i;
        enemies[i].useMiscPalette = false;
    }
}

Enemy* ENEMY_create(Enemy *enemy) {
    if (enemy_top_index < 0) {
        return NULL;
    }

    u8 idx = enemy_free_index[enemy_top_index--];
    Enemy *e = &enemies[idx];

    *e = *enemy;

    e->index = idx;
    e->active = TRUE;
    e->destroying = FALSE;
    e->inverted = FALSE;
    e->spriteIndex = 0;
    e->sprite = NULL;

    if (e->type == ENEMY_TYPE_8) {
        ENEMY8_create(e, e->useMiscPalette);
        return e;
    }

    u8 paletteIndex = ENEMY_PALLETE;

    if (enemy->useMiscPalette) {
        paletteIndex = MISC_PALLETE;
        PAL_setPalette(MISC_PALLETE, enemy->spriteDefinition->palette->data, DMA);
    }

    e->sprite = SPR_addSprite(
        enemy->spriteDefinition,
        e->x,
        e->y,
        TILE_ATTR(paletteIndex, TRUE, FALSE, FALSE)
    );

    if (e->sprite != NULL) {
        SPR_setAnim(e->sprite, 0);
    }

    return e;
}

Enemy *ENEMYFACTORY_createEnemy(EnemyType type, s16 x, s16 y) {
    Enemy temp;
    ENEMYFACTORY_initEnemy(&temp, type, x, y);
    return ENEMY_create(&temp);
}

void ENEMY_update() {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        Enemy* enemy = &enemies[i];

        if (!enemy->active)
            continue;

        enemy->y += enemy->y_speed;
        enemy->x += enemy->x_speed;

        if (enemy->destroying) {
            ENEMY_destroyAnim(enemy);
            continue;
        }

        if (enemy->type != ENEMY_TYPE_8) {
            if (enemy->y > (GAME_WINDOW_HEIGHT + (enemy->height << 1)) || enemy->y < -(enemy->height << 1)) {
                ENEMY_deactivate(enemy);
                continue;
            }

            if (enemy->x < -(enemy->width << 1) || enemy->x > (GAME_WINDOW_WIDTH + (enemy->width << 1))) {
                ENEMY_deactivate(enemy);
                continue;
            }
        }

        switch (enemy->type) {
            case ENEMY_TYPE_1: {
                s16 dy = player.y - enemy->y;

                if (dy < 0) {
                    break;
                }

                s16 dx = player.x - enemy->x;

                bool flipH = (dx < 0);
                SPR_setHFlip(enemy->sprite, flipH);

                u16 adx = (dx < 0) ? -dx : dx;
                u16 ady = (dy < 0) ? -dy : dy;

                if (ady > (adx << 2)) {
                      enemy->spriteIndex = 0;
                }
                else if (ady > adx) {
                    enemy->spriteIndex = 1;
                }
                else if (adx > (ady << 2)) {
                    enemy->spriteIndex = 4;
                }
                else if (adx > ady) {
                    enemy->spriteIndex = 3;
                }
                else {
                    enemy->spriteIndex = 2;
                }

                break;
            }
            case ENEMY_TYPE_2:
                if (enemy->spriteIndex < enemy->max_frames && !enemy->inverted) {
                    enemy->spriteIndex++;
                    break;
                }

                if (enemy->spriteIndex > 3 && !enemy->inverted) {
                    enemy->inverted = true;
                    SPR_setVFlip(enemy->sprite, enemy->inverted);
                    break;
                }

                if (enemy->inverted && enemy->spriteIndex > 0) {
                    enemy->spriteIndex--;
                    break;
                }

                if (enemy->inverted && enemy->spriteIndex < 1) {
                    enemy->inverted = false;
                    SPR_setVFlip(enemy->sprite, enemy->inverted);
                    break;
                }

            case ENEMY_TYPE_8:
                PAL_setColor(enemy->paletteAccentIndex, enemy->accentColors[enemy->accentColorIndex]);
                enemy->accentColorIndex++;

                if (enemy->accentColorIndex >= 5) {
                    enemy->accentColorIndex = 0;
                }

                continue;

            case ENEMY_TYPE_TELEPORT_SMALL:
                enemy->spriteIndex++;

                if (enemy->spriteIndex >= enemy->max_frames) {
                    ENEMY_deactivate(enemy);
                    continue;
                
                }
                
                SPR_setFrame(enemy->sprite, enemy->spriteIndex);                                
                break;

            default:
                enemy->spriteIndex++;
                if (enemy->spriteIndex >= enemy->max_frames) {
                    enemy->spriteIndex = 0;
                }
                break;
        }

        if (enemy->type != ENEMY_TYPE_8) {
            SPR_setFrame(enemy->sprite, enemy->spriteIndex);
            SPR_setPosition(enemy->sprite, enemy->x, enemy->y);
            SPR_setAlwaysOnTop(enemy->sprite); // pode ser q o cara tenha q ficar abaixo de algum foreground, mas por enquanto vamos deixar assim
        }
    }
}

void ENEMY_shoot(Enemy* enemy, SpriteDefinition* bulletSprite, s16 velX, s16 velY) {
    if (!enemy->active) {
        return;
    }

    BULLET_enemyShoot(bulletSprite, enemy->x + (enemy->width >> 2), enemy->y + enemy->height + 1, velX, velY);
}

void ENEMY_setLinearMovement(Enemy* enemy, s16 vx, s16 vy) {
    enemy->x_speed = vx;
    enemy->y_speed = vy;
}

void ENEMY_setArcedMovement(Enemy* enemy, s16 vx, s16 vy, s16 angle, s16 velocity) {
    enemy->x_speed = vx;
    enemy->y_speed = vy;
    enemy->angle = angle;
}

// ate funciona mas fica lento
void ENEMY_setMoveToPlayer(Enemy* enemy, s16 velocity) {
    s16 dx = player.x - enemy->x;
    s16 dy = player.y - enemy->y;

    fix16 fdx = FIX16(dx);
    fix16 fdy = FIX16(dy);
    fix16 dist_sq = fix16Mul(fdx, fdx) + fix16Mul(fdy, fdy);
    fix16 dist = fix16Sqrt(dist_sq);

    if (dist == 0) return;

    fix16 norm_x = fix16Div(fdx, dist);
    fix16 norm_y = fix16Div(fdy, dist);

    enemy->x_speed = fix16ToInt(fix16Mul(norm_x, FIX16(velocity))) >> 2;
    enemy->y_speed = fix16ToInt(fix16Mul(norm_y, FIX16(velocity))) >> 2;
}

static void ENEMY_destroyAnim(Enemy* enemy) {
    if (enemy->spriteIndex < enemy->explosionDefinition->maxFrames) {
        enemy->spriteIndex++;
        SPR_setFrame(enemy->sprite, enemy->spriteIndex);
        return;
    }

    ENEMY_deactivate(enemy);
}

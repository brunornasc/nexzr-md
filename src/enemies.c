#include "enemies.h"
#include "resources.h"
#include "game.h"
#include <genesis.h>

Enemy enemies[MAX_ENEMIES];
u8 enemy_free_index[MAX_ENEMIES];
s8 enemy_top_index = -1;

void ENEMY_actionFlipSpriteHorizontally(Enemy* enemy, u8 minFrame, u8 maxFrame);
void ENEMY_destroyAnim(Enemy* enemy);

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
            TILE_ATTR(ENEMY_PALLETE, FALSE, FALSE, FALSE)
        );

        ENEMY_destroyAnim(enemy);
        Game_addGameScore(enemy->score_points);
    }
        
}

void ENEMY_deactivate(Enemy* enemy) {
    if (!enemy->active) return;

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

    //PAL_setPalette(ENEMY_PALLETE, enemy->spriteDefinition->palette->data, DMA);
    u8 paletteIndex = ENEMY_PALLETE;

    if (enemy->useMiscPalette) {
        paletteIndex = MISC_PALLETE;
        PAL_setPalette(MISC_PALLETE, enemy->spriteDefinition->palette->data, DMA);
    }

    e->sprite = SPR_addSprite(
        enemy->spriteDefinition,
        e->x,
        e->y,
        TILE_ATTR(paletteIndex, FALSE, FALSE, FALSE)
    );

    if (e->sprite != NULL) {
        SPR_setAnim(e->sprite, 0);
    }

    return e;
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

        // Garbage collector

        if (enemy->y > (GAME_WINDOW_HEIGHT + (enemy->height << 1)) || enemy->y < -(enemy->height << 1)) {
            ENEMY_deactivate(enemy);
            continue;
        }

        if (enemy->x < -(enemy->width << 1) || enemy->x > (GAME_WINDOW_WIDTH + (enemy->width << 1))) {
            ENEMY_deactivate(enemy);
            continue;
        }        

        // Sprite animation - default for all types
        enemy->spriteIndex++;
        if (enemy->spriteIndex >= enemy->max_frames) {
            enemy->spriteIndex = 0;
        }

        SPR_setFrame(enemy->sprite, enemy->spriteIndex);
        SPR_setPosition(enemy->sprite, enemy->x, enemy->y);
        SPR_setAlwaysOnTop(enemy->sprite);
    }
}

void ENEMY_shoot(Enemy* enemy, SpriteDefinition* bulletSprite, s16 velX, s16 velY) {
    if (!enemy->active) {
        return;
    }

    BULLET_enemyShoot(bulletSprite, enemy->x + (enemy->width >> 2), enemy->y + enemy->height + 1, velX, velY);
}

void ENEMY_actionFlipSpriteHorizontally(Enemy* enemy, u8 minFrame, u8 maxFrame) {
    if (enemy->spriteIndex < maxFrame && !enemy->inverted) {
        enemy->spriteIndex++;
        return;                
    }

    else if (enemy->spriteIndex > maxFrame -1 && !enemy->inverted) {
        enemy->inverted = true;
        enemy->spriteIndex--;
        SPR_setHFlip(enemy->sprite, enemy->inverted);
        return;                
    }

    else if (enemy->inverted && enemy->spriteIndex > minFrame) {
        enemy->spriteIndex--;
        return;                
    }

    else if (enemy->inverted && enemy->spriteIndex < minFrame + 1) {
        enemy->inverted = false;
        enemy->spriteIndex++;
        SPR_setHFlip(enemy->sprite, enemy->inverted);
        return;                
    }    
}

void ENEMY_incrementAllocEnemies() {
    //Todo
}

void ENEMY_destroyAnim(Enemy* enemy) {
    // enemy_explosion
    if (enemy->spriteIndex < enemy->explosionDefinition->maxFrames) {
        enemy->spriteIndex++;
        SPR_setFrame(enemy->sprite, enemy->spriteIndex);
        return;                
    }

    ENEMY_deactivate(enemy);
    return;
}

// Novas funções de movimento
void ENEMY_setLinearMovement(Enemy* enemy, s16 vx, s16 vy) {
    enemy->x_speed = vx;
    enemy->y_speed = vy;
}

void ENEMY_setArcedMovement(Enemy* enemy, s16 vx, s16 vy, s16 angle, s16 velocity) {
    enemy->x_speed = vx;
    enemy->y_speed = vy;
    enemy->angle = angle;
    // velocity pode ser usado para algo futuro, como magnitude
}

void ENEMY_setMoveToPlayer(Enemy* enemy, s16 velocity) {
    // Calcular direção para o player
    s16 dx = player.x - enemy->x;
    s16 dy = player.y - enemy->y;
    
    // Usar fix16 para normalizar
    fix16 fdx = FIX16(dx);
    fix16 fdy = FIX16(dy);
    fix16 dist_sq = fix16Mul(fdx, fdx) + fix16Mul(fdy, fdy);
    fix16 dist = fix16Sqrt(dist_sq);
    
    if (dist == 0) return; // Evitar divisão por zero
    
    fix16 norm_x = fix16Div(fdx, dist);
    fix16 norm_y = fix16Div(fdy, dist);
    
    enemy->x_speed = fix16ToInt(fix16Mul(norm_x, FIX16(velocity))) >> 2;
    enemy->y_speed = fix16ToInt(fix16Mul(norm_y, FIX16(velocity))) >> 2;
}
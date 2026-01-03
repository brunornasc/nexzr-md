#include "enemies.h"
#include "resources.h"
#include "game.h"

Enemy enemies[MAX_ENEMIES];
u8 enemy_free_index[MAX_ENEMIES];
s8 enemy_top_index = -1;

void ENEMY_gotHit(Enemy* enemy, u8 damage) {
    enemy->health -= damage;

    if (enemy->health <= 0) 
        ENEMY_deactivate(enemy);    
}

void ENEMY_deactivate(Enemy* enemy) {
    if (!enemy->active) return;

    enemy->active = false;
    if (enemy->sprite) {
        SPR_releaseSprite(enemy->sprite);
        enemy->sprite = NULL;
    }
    
    enemy_free_index[++enemy_top_index] = enemy->index;
}

void ENEMY_initializeAll() {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].sprite = NULL;
        enemies[i].health = 0;
        enemies[i].index = i;
        enemy_free_index[++enemy_top_index] = i;
    }
}

void ENEMY_create(Enemy *enemy) {
    if (enemy_top_index < 0) return; // Segurança: pool cheio

    // Pega um índice livre do pool
    u8 idx = enemy_free_index[enemy_top_index--];
    
    // COPIAR os dados da struct temporária para o array global
    enemies[idx].x = enemy->x;
    enemies[idx].y = enemy->y;
    enemies[idx].x_speed = enemy->x_speed;
    enemies[idx].y_speed = enemy->y_speed;
    enemies[idx].width = enemy->width;
    enemies[idx].height = enemy->height;
    enemies[idx].health = enemy->health;
    enemies[idx].active = true; // MUITO IMPORTANTE

    // Carrega a paleta (opcional se já estiver fixa)
    PAL_setPalette(ENEMY_PALLETE, enemy->sprite->palette->data, DMA);

    // Note que removi o '&' antes de enemy->sprite
    enemies[idx].sprite = SPR_addSprite(enemy->sprite, enemy->x, enemy->y, TILE_ATTR(ENEMY_PALLETE, FALSE, FALSE, FALSE));

    if (enemies[idx].sprite != NULL) {
        SPR_setAnim(enemies[idx].sprite, 0);
    }
}

void ENEMY_update() {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        Enemy* enemy = &enemies[i];
    
        if (!enemy->active)
            continue;
    
        enemy->y += enemy->y_speed;
        enemy->x += enemy->x_speed;

        // Garbage collector

        if (enemy->y > GAME_WINDOW_HEIGHT || enemy->y < -enemy->height) {
            ENEMY_deactivate(enemy);
            enemy_free_index[++enemy_top_index] = i;
            continue;
        }

        if (enemy->x < -enemy->width || enemy->x > GAME_WINDOW_WIDTH) {
            ENEMY_deactivate(enemy);
            enemy_free_index[++enemy_top_index] = i;
            continue;
        }
    
        SPR_setPosition(enemy->sprite, enemy->x, enemy->y);
    }
}
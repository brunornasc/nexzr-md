#include "enemies.h"
#include "resources.h"
#include "game.h"

Enemy enemies[MAX_ENEMIES];
u8 enemy_free_index[MAX_ENEMIES];
s8 enemy_top_index = -1;

void ENEMY_actionFlipSpriteHorizontally(Enemy* enemy, u8 minFrame, u8 maxFrame);

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

Enemy* ENEMY_create(Enemy *enemy) {
    if (enemy_top_index < 0) return;

    u8 idx = enemy_free_index[enemy_top_index--];
    
    enemies[idx].x = enemy->x;
    enemies[idx].y = enemy->y;
    enemies[idx].x_speed = enemy->x_speed;
    enemies[idx].y_speed = enemy->y_speed;
    enemies[idx].width = enemy->width;
    enemies[idx].height = enemy->height;
    enemies[idx].health = enemy->health;
    enemies[idx].active = TRUE;
    enemies[idx].type = enemy->type;
    enemies[idx].inverted = FALSE;
    enemies[idx].spriteIndex = 0;
    enemies[idx].bulletSprite = enemy->bulletSprite;    

    PAL_setPalette(ENEMY_PALLETE, enemy->sprite->palette->data, DMA);
    enemies[idx].sprite = SPR_addSprite(enemy->sprite, enemy->x, enemy->y, TILE_ATTR(ENEMY_PALLETE, FALSE, FALSE, FALSE));

    if (enemies[idx].sprite != NULL) {
        SPR_setAnim(enemies[idx].sprite, 0);
    }

    return &enemies[idx];
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
            continue;
        }

        if (enemy->x < -enemy->width || enemy->x > GAME_WINDOW_WIDTH) {
            ENEMY_deactivate(enemy);
            continue;
        }        

        // Sprite rotation

        //TODO enemy 5 e 6 tem só 4 sprites de animação e o 7 2
        
        switch (enemy->type) {
            case ENEMY_TYPE_1:
            case ENEMY_TYPE_2:
                if (enemy->spriteIndex < 4 && !enemy->inverted) {
                    enemy->spriteIndex++;
                    break;                
                }

                else if (enemy->spriteIndex > 3 && !enemy->inverted) {
                    enemy->inverted = true;
                    SPR_setVFlip(enemy->sprite, enemy->inverted);
                    break;                
                }

                else if (enemy->inverted && enemy->spriteIndex > 0) {
                    enemy->spriteIndex--;
                    break;                
                }

                else if (enemy->inverted && enemy->spriteIndex < 1) {
                    enemy->inverted = false;
                    SPR_setVFlip(enemy->sprite, enemy->inverted);
                    break;                
                }                

            case ENEMY_TYPE_3:
               ENEMY_actionFlipSpriteHorizontally(enemy, 0, 4);
               break;

            default:
                break;
        }

        SPR_setFrame(enemy->sprite, enemy->spriteIndex);
        SPR_setPosition(enemy->sprite, enemy->x, enemy->y);
    }
}

void ENEMY_shoot(Enemy* enemy, SpriteDefinition* bulletSprite, s16 velX, s16 velY) {
    BULLET_enemyShoot(bulletSprite, enemy->x + enemy->width / 2, enemy->y + enemy->height + 1, velX, velY);
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
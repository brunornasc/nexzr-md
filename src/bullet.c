#include "bullet.h"
#include "game.h"
#include "resources.h"

Bullet slasher_bullets[MAX_BULLETS];
u8 free_indices[MAX_BULLETS];
s8 top_index = -1; 

void BULLET_setup_pool() {
    for (u8 i = 0; i < MAX_BULLETS; i++) {
        slasher_bullets[i].active = FALSE;
        free_indices[++top_index] = i;
    }
}

void BULLET_slasherShoot(s16 posX, s16 posY) {
    if (top_index < 0) return;

    u8 idx = free_indices[top_index--];
    
    Bullet* b = &slasher_bullets[idx];
    b->x = posX;
    b->y = posY;
    b->velX = 0;
    b->velY = -8;
    b->active = TRUE;
    b->sprite = SPR_addSprite(&slasher_weapon_0001, posX, posY, TILE_ATTR(SLASHER_BULLET_PALLETE, 0, 0, 0));
}

void BULLET_deactivate(u8 array_index) {
    Bullet* b = &slasher_bullets[array_index];
    
    if (!b->active) return;

    b->active = FALSE;
    if (b->sprite) {
        SPR_releaseSprite(b->sprite);
        b->sprite = NULL;
    }

    free_indices[++top_index] = array_index;
}

void Bullet_updateAll() {
    for (u8 i = 0; i < MAX_BULLETS; i++) {
        Bullet* b = &slasher_bullets[i];
        if (b->active) {
            b->x += b->velX;
            b->y += b->velY;

            if (b->y < -16 || b->y > GAME_WINDOW_HEIGHT || b->x < -16 || b->x > GAME_WINDOW_WIDTH) {
                BULLET_deactivate(i);

            } else {
                SPR_setPosition(b->sprite, b->x, b->y);

            }
        }
    }
}
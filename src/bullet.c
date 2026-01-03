#include "bullet.h"
#include "resources.h"
#include "game.h"
#include "hud.h"

// --- Pool do Slasher (9 slots) ---
Bullet slasher_bullets[MAX_SLASHER_BULLETS];
u8 slasher_free_indices[MAX_SLASHER_BULLETS];
s8 slasher_top = -1;
u16 slasher_shoot_timer = 0;

// --- Pool dos Inimigos (11 slots) ---
Bullet enemy_bullets[MAX_ENEMY_BULLETS];
u8 enemy_free_indices[MAX_ENEMY_BULLETS];
s8 enemy_top = -1;

void BULLET_setup_pool() {
    // Inicializa Slasher
    slasher_top = -1;
    for (u8 i = 0; i < MAX_SLASHER_BULLETS; i++) {
        slasher_bullets[i].active = FALSE;
        slasher_free_indices[++slasher_top] = i;
    }

    // Inicializa Inimigos
    enemy_top = -1;
    for (u8 i = 0; i < MAX_ENEMY_BULLETS; i++) {
        enemy_bullets[i].active = FALSE;
        enemy_free_indices[++enemy_top] = i;
    }
}

void BULLET_slasherShoot(s16 posX, s16 posY) {
    if (slasher_shoot_timer > 0 || slasher_top < 0) return;

    u8 idx = slasher_free_indices[slasher_top--];
    Bullet* b = &slasher_bullets[idx];
    
    // ajustar
    b->x = posX; b->y = posY;
    b->velX = 0; b->velY = -8;
    b->active = TRUE;
    b->width = 16; 
    b->height = 16;
    b->sprite = SPR_addSprite(&slasher_weapon_0001, posX, posY, TILE_ATTR(SLASHER_BULLET_PALLETE, 0, 0, 0));
    
    slasher_shoot_timer = SLASHER_SHOOT_COOLDOWN;
}

void BULLET_enemyShoot(s16 posX, s16 posY, s16 velX, s16 velY) {
    if (enemy_top < 0) return; // Se não houver slot de bala inimiga livre, cancela

    u8 idx = enemy_free_indices[enemy_top--];
    Bullet* b = &enemy_bullets[idx];

    b->x = posX; b->y = posY;
    b->velX = velX; b->velY = velY;
    b->active = TRUE;
    // Aqui você usa o sprite da bala do inimigo
    // b->sprite = SPR_addSprite(&enemy_bullet_resource, posX, posY, TILE_ATTR(ENEMY_PALLETE, 0, 0, 0));
}

static void deactivate_bullet(Bullet* b, u8* stack, s8* top, u8 index) {
    b->active = FALSE;
    if (b->sprite) {
        SPR_releaseSprite(b->sprite);
        b->sprite = NULL;
    }

    stack[++(*top)] = index;
}

void BULLET_updateAll() {
    if (slasher_shoot_timer > 0) slasher_shoot_timer--;

    for (u8 i = 0; i < MAX_SLASHER_BULLETS; i++) {
        Bullet* b = &slasher_bullets[i];

        if (b->active) {
            b->x += b->velX; 
            b->y += b->velY;

            if (b->y < -16 || b->y > GAME_WINDOW_HEIGHT || b->x < -16 || b->x > GAME_WINDOW_WIDTH) 
                deactivate_bullet(b, slasher_free_indices, &slasher_top, i);

            else {
                SPR_setPosition(b->sprite, b->x, b->y);
                COLLISION_checkBulletCollisionWithEnemy(b);

            }
                

        }
    }

    for (u8 i = 0; i < MAX_ENEMY_BULLETS; i++) {
        Bullet* b = &enemy_bullets[i];

        if (b->active) {
            b->x += b->velX; 
            b->y += b->velY;

            if (b->y < -16 || b->y > GAME_WINDOW_HEIGHT || b->x < -16 || b->x > GAME_WINDOW_WIDTH)
                deactivate_bullet(b, enemy_free_indices, &enemy_top, i);

            else {
                SPR_setPosition(b->sprite, b->x, b->y);
                COLLISION_checkBulletCollisionWithSlasher(b);

            }
        }
    }
}
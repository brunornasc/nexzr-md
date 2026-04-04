#include "bullet.h"
#include "resources.h"
#include "game.h"
#include "hud.h"

void BULLET_enemyShoot2(SpriteDefinition* bulletSprite, s16 posX, s16 posY, s16 velX, s16 velY);

// Struct interna para manter posição em fixed-point
typedef struct {
    s32 x_fp;
    s32 y_fp;
} BulletPosition;

Bullet slasher_bullets[MAX_SLASHER_BULLETS];
BulletPosition slasher_positions[MAX_SLASHER_BULLETS];
u8 slasher_free_indices[MAX_SLASHER_BULLETS];
s8 slasher_top = -1;
u16 slasher_shoot_timer = 0;

Bullet enemy_bullets[MAX_ENEMY_BULLETS];
BulletPosition enemy_positions[MAX_ENEMY_BULLETS];
u8 enemy_free_indices[MAX_ENEMY_BULLETS];
s8 enemy_top = -1;

void BULLET_setup_pool() {
    slasher_top = -1;
    for (u8 i = 0; i < MAX_SLASHER_BULLETS; i++) {
        slasher_bullets[i].active = FALSE;
        slasher_free_indices[++slasher_top] = i;
    }

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

    b->x = posX;
    b->y = posY;
    b->velX = 0;
    b->velY = -8 << 8; // Slasher já manda em Fixed Point
    b->active = TRUE;
    b->width = 16;
    b->height = 16;
    b->sprite = SPR_addSprite(&slasher_weapon_0001, posX, posY, TILE_ATTR(SLASHER_BULLET_PALLETE, TRUE, 0, 0));

    slasher_positions[idx].x_fp = (s32)posX << 8;
    slasher_positions[idx].y_fp = (s32)posY << 8;

    slasher_shoot_timer = SLASHER_SHOOT_COOLDOWN;
}

void BULLET_enemyShoot_slasherDirection(Enemy *enemy, Player* player, s16 speed) {
    if (!enemy->active) return;

    s32 dx = (s32)((player->x + 8) - enemy->x);
    s32 dy = (s32)((player->y + 8) - enemy->y);

    // Se estiver exatamente em cima, atira para baixo
    if (dx == 0 && dy == 0) {
        BULLET_enemyShoot(enemy->bulletSprite, enemy->x, enemy->y, 0, speed);
        return;
    }

    s32 abs_dx = (dx < 0) ? -dx : dx;
    s32 abs_dy = (dy < 0) ? -dy : dy;
    s32 dist = (abs_dx > abs_dy) ? (abs_dx + (abs_dy >> 1)) : (abs_dy + (abs_dx >> 1));

    if (dist == 0) dist = 1;

    // Aqui já estamos convertendo para Fixed Point (* 256)
    s16 velX = (s16)((dx * speed * 256) / dist);
    s16 velY = (s16)((dy * speed * 256) / dist);

    BULLET_enemyShoot2(enemy->bulletSprite, enemy->x, enemy->y, velX, velY);

}

// A função base agora recebe o valor JÁ em Fixed Point
void BULLET_enemyShoot(SpriteDefinition* bulletSprite, s16 posX, s16 posY, s16 velX, s16 velY) {
    if (enemy_top < 0) return;

    u8 idx = enemy_free_indices[enemy_top--];
    Bullet* b = &enemy_bullets[idx];

    b->x = posX;
    b->y = posY;
    b->velX = velX << 8;
    b->velY = velY << 8;
    b->active = TRUE;
    b->width = 8;
    b->height = 8;

    b->sprite = SPR_addSprite(bulletSprite, posX, posY, TILE_ATTR(ENEMY_BULLET_PALLETE, TRUE, 0, 0));

    enemy_positions[idx].x_fp = (s32)posX << 8;
    enemy_positions[idx].y_fp = (s32)posY << 8;
}

void BULLET_enemyShoot2(SpriteDefinition* bulletSprite, s16 posX, s16 posY, s16 velX, s16 velY) {
    if (enemy_top < 0) return;

    u8 idx = enemy_free_indices[enemy_top--];
    Bullet* b = &enemy_bullets[idx];

    b->x = posX;
    b->y = posY;
    b->velX = velX;
    b->velY = velY;
    b->active = TRUE;
    b->width = 8;
    b->height = 8;

    b->sprite = SPR_addSprite(bulletSprite, posX, posY, TILE_ATTR(ENEMY_BULLET_PALLETE, TRUE, 0, 0));

    enemy_positions[idx].x_fp = (s32)posX << 8;
    enemy_positions[idx].y_fp = (s32)posY << 8;
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

    // Update Slasher Bullets
    for (u8 i = 0; i < MAX_SLASHER_BULLETS; i++) {
        Bullet* b = &slasher_bullets[i];
        if (b->active) {
            slasher_positions[i].x_fp += b->velX;
            slasher_positions[i].y_fp += b->velY;
            b->x = (s16)(slasher_positions[i].x_fp >> 8);
            b->y = (s16)(slasher_positions[i].y_fp >> 8);

            if (b->y < -16 || b->y > GAME_WINDOW_HEIGHT || b->x < -16 || b->x > GAME_WINDOW_WIDTH)
                deactivate_bullet(b, slasher_free_indices, &slasher_top, i);
            else {
                SPR_setPosition(b->sprite, b->x, b->y);
                if (COLLISION_checkBulletCollisionWithEnemy(b)) deactivate_bullet(b, slasher_free_indices, &slasher_top, i);
            }
        }
    }

    // Update Enemy Bullets
    for (u8 i = 0; i < MAX_ENEMY_BULLETS; i++) {
        Bullet* b = &enemy_bullets[i];
        if (b->active) {
            enemy_positions[i].x_fp += b->velX;
            enemy_positions[i].y_fp += b->velY;
            b->x = (s16)(enemy_positions[i].x_fp >> 8);
            b->y = (s16)(enemy_positions[i].y_fp >> 8);

            if (b->y < -16 || b->y > GAME_WINDOW_HEIGHT || b->x < -16 || b->x > GAME_WINDOW_WIDTH)
                deactivate_bullet(b, enemy_free_indices, &enemy_top, i);
            else {
                SPR_setPosition(b->sprite, b->x, b->y);
                if (COLLISION_checkBulletCollisionWithSlasher(b)) deactivate_bullet(b, enemy_free_indices, &enemy_top, i);
            }
        }
    }
}
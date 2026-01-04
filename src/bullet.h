#ifndef BULLET_H
#define BULLET_H

#include <genesis.h>

#define MAX_SLASHER_BULLETS 9
#define MAX_ENEMY_BULLETS 11
#define SLASHER_SHOOT_COOLDOWN 8

typedef struct {
    s16 x, y;
    s16 velX, velY;
    s16 width, height;
    bool active;
    Sprite* sprite;
} Bullet;


void BULLET_setup_pool();
void BULLET_updateAll();
void BULLET_slasherShoot(s16 posX, s16 posY);
void BULLET_enemyShoot(SpriteDefinition* bulletSprite, s16 posX, s16 posY, s16 velX, s16 velY);
#endif
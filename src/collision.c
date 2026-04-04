#include "collision.h"

static inline __attribute__((always_inline)) 
u8 COLLISION_testCollision(s16 ax, s16 ay, s16 aw, s16 ah,
                            s16 bx, s16 by, s16 bw, s16 bh) {
    return (ax < bx + bw) &&
           (ax + aw > bx) &&
           (ay < by + bh) &&
           (ay + ah > by);
}



int COLLISION_checkBulletCollisionWithEnemy(Bullet* bullet) {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        Enemy* enemy = &enemies[i];

        if (!enemy->active || enemy->health <= 0)
            continue;      


        if (COLLISION_testCollision(bullet->x, bullet->y, bullet->width, bullet->height,
                                    enemy->x, enemy->y, enemy->width, enemy->height)) {            
            ENEMY_gotHit(enemy, 1);
            return true;
        }
    }

    return false;
}

int COLLISION_checkBulletCollisionWithSlasher(Bullet* bullet) {

    if (COLLISION_testCollision(player.x + 8, player.y + 8, 8, 8,
                                bullet->x, bullet->y, bullet->width, bullet->height)) {
            PLAYER_gotHit(&player, 1);
            return true;
    }
    
    return false;
}

int COLLISION_checkEnemyCollisionWithPlayer(Enemy* enemy) {
    if (!enemy->active || enemy->health <= 0)
        return false;      

    if (COLLISION_testCollision(player.x + 8, player.y + 8, 8, 8,
                                enemy->x + 2, enemy->y + 2, enemy->width - 2, enemy->height - 2)) {
            PLAYER_gotHit(&player, 1);
            return true;
    }
    
    return false;
}

void COLLISION_checkAllCollisions() {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        Enemy* enemy = &enemies[i];
        if (COLLISION_checkEnemyCollisionWithPlayer(enemy)) {
            kprintf("Player hit by enemy at index %d!", i);
        }
    }
}
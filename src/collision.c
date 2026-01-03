#include "collision.h"

void COLLISION_checkBulletCollisionWithEnemy(Bullet* bullet) {

    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        Enemy* enemy = &enemies[i];

        if (bullet->x < enemy->x + enemy->width &&
            bullet->x + bullet->width > enemy->x && 
            bullet->y < enemy->y + enemy->height &&
            bullet->y + bullet->height > enemy->y) {            
                ENEMY_gotHit(enemy, 1);
        }
    }
}

void COLLISION_checkBulletCollisionWithSlasher(Bullet* bullet) {

    if (bullet->x < player.x + 32 &&
        bullet->x + bullet->width > player.x && 
        bullet->y < player.y + 32 &&
        bullet->y + bullet->height > player.y) {
            PLAYER_gotHit(1);
    }

}
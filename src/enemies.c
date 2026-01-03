#include "enemies.h"

Enemy enemies[MAX_ENEMIES];

void ENEMY_gotHit(Enemy* enemy, u8 damage) {
    enemy->health -= damage;

    if (enemy->health <= 0) 
        ENEMY_deactivate(enemy);    
}

void ENEMY_deactivate(Enemy* enemy) {
    enemy->active = false;
    SPR_releaseSprite(enemy->sprite);
    enemy->sprite = NULL;
}

void ENEMY_initializeAll() {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].sprite = NULL;
        enemies[i].health = 0;
    }
}
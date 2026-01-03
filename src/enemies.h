#ifndef ENEMIES_H
#define ENEMIES_H

#include <genesis.h>

#define MAX_ENEMIES 10

typedef enum {
    ENEMY_TYPE_1,
    ENEMY_TYPE_2,
    ENEMY_TYPE_3,
    ENEMY_TYPE_4,
    ENEMY_TYPE_5,
    ENEMY_TYPE_6,
    MAX_ENEMY_TYPES
} EnemyType;

typedef struct {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 x_speed, y_speed;
    u8 spriteIndex;
    EnemyType type;
    bool active;
    bool inverted;
    SpriteDefinition* sprite;
    u8 health;
    Sprite *bulletSprite;
    u8 index;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

void ENEMY_gotHit(Enemy* enemy, u8 damage);
void ENEMY_deactivate(Enemy* enemy);
void ENEMY_initializeAll();
void ENEMY_create(Enemy *enemy);
void ENEMY_update();


#endif
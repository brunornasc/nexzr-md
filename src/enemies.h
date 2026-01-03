#ifndef ENEMIES_H
#define ENEMIES_H

#include <genesis.h>

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
    s16 speed;
    u8 spriteIndex;
    EnemyType type;
    bool active;
    bool inverted;
    Sprite* sprite;
} Enemy;


#endif
#ifndef ENEMIES_H
#define ENEMIES_H

#include <genesis.h>

#define MAX_ENEMIES 20

typedef enum {
    ENEMY_TYPE_1,
    ENEMY_TYPE_2,
    ENEMY_TYPE_3,
    ENEMY_TYPE_4,
    ENEMY_TYPE_5,
    ENEMY_TYPE_6,
    ENEMY_TYPE_7,
    ENEMY_TYPE_8,
    MAX_ENEMY_TYPES
} EnemyType;

typedef struct {
    SpriteDefinition* sprite;
    u8 maxFrames;
} ExplosionAnimationDefinition;

typedef struct {
    u32 score_points;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 x_speed;
    s16 y_speed;
    u8 spriteIndex;
    u8 max_frames;
    u8 health;
    u8 index;
    EnemyType type;
    bool active;
    bool destroying;
    bool inverted;
    Sprite* sprite;
    SpriteDefinition* spriteDefinition;
    SpriteDefinition* bulletSprite;
    ExplosionAnimationDefinition* explosionDefinition;    
    u16 accentColors[5];
    u8 accentColorIndex;
    u8 paletteAccentIndex;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

void ENEMY_gotHit(Enemy* enemy, u8 damage);
void ENEMY_deactivate(Enemy* enemy);
void ENEMY_initializeAll();
Enemy* ENEMY_create(Enemy *enemy);
void ENEMY_update();
void ENEMY_shoot(Enemy* enemy, SpriteDefinition* bulletSprite, s16 velX, s16 velY);
void ENEMY_incrementAllocEnemies();


#endif
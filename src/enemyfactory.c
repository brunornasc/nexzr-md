#include <genesis.h>
#include "resources.h"
#include "enemyfactory.h"

// Usamos typedef para poder usar 'EnemyDefaults' sem escrever 'struct' toda vez
typedef struct {
    s16 width;
    s16 height;
    s16 y_speed;
    s16 x_speed;
    EnemyType type;
    s16 health;
    SpriteDefinition *sprite;       // No SGDK geralmente usamos SpriteDefinition para recursos
    SpriteDefinition *bulletSprite;
    u16 score_points;
} EnemyDefaults;

static const EnemyDefaults ENEMY1_DEFAULTS = {
    16, 16, 3, 0, ENEMY_TYPE_1, 1, &enemy_0001, &enemy_bullet_001, 100
};

static const EnemyDefaults ENEMY2_DEFAULTS = {
    16, 16, 3, 0, ENEMY_TYPE_2, 1, &enemy_0002, &enemy_bullet_001, 100
};

static const EnemyDefaults ENEMY3_DEFAULTS = {
    16, 16, 3, 0, ENEMY_TYPE_3, 1, &enemy_0003, &enemy_bullet_001, 100
};

Enemy *ENEMYFACTORY_createEnemy(EnemyType type, s16 x, s16 y) {
    Enemy *e = malloc(sizeof(Enemy));
    if (e != NULL) {
        ENEMYFACTORY_initEnemy(e, type, x, y);
    }
    return e;
}

void ENEMYFACTORY_initEnemy(Enemy *e, EnemyType type, s16 x, s16 y) {
    e->x = x;
    e->y = y;
    e->spriteIndex = 0;
    e->active = true;
    e->inverted = false;

    const EnemyDefaults *d;

    switch (type) {
        case ENEMY_TYPE_1:
            d = &ENEMY1_DEFAULTS;
            break;
        case ENEMY_TYPE_2:
            d = &ENEMY2_DEFAULTS;
            break;
        case ENEMY_TYPE_3:
            d = &ENEMY3_DEFAULTS;
            break;
        default:
            return;
    }

    e->width        = d->width;
    e->height       = d->height;
    e->y_speed      = d->y_speed;
    e->x_speed      = d->x_speed;
    e->type         = d->type;
    e->health       = d->health;
    e->sprite       = d->sprite;
    e->score_points = d->score_points;
    e->bulletSprite = d->bulletSprite;
}
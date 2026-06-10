#ifndef LEVEL_H
#define LEVEL_H

#include <genesis.h>
#include "enemy.h"

#define MAX_CUSTOM_ACTIONS 5

typedef void (*ScriptCustomFunc)(Enemy* e);

typedef enum {
    ACTION_SPAWN,
    ACTION_SET_SHOOT_RATE,
    ACTION_SHOOT_ONCE,
    ACTION_SHOOT_SLASHER_DIRECTION,
    ACTION_DEACTIVATE,
    ACTION_STOP_SHOOT,
    ACTION_SET_LINEAR_MOVEMENT,
    ACTION_SET_ARCED_MOVEMENT,
    ACTION_SET_MOVE_TO_PLAYER,
    ACTION_CUSTOM_0,
    ACTION_CUSTOM_1
} ScriptActionType;

typedef struct {
    u16 frame;
    ScriptActionType action;
    u8 slot;
    EnemyType type;
    s16 x;
    s16 y;
    u16 shootInterval;
    s16 shootSpeed;
} ScriptItem;

typedef struct {
    Enemy* enemy;
    u16 shootInterval;
    u16 shootTimer;
    s16 shootSpeed;
    ScriptCustomFunc customActions[MAX_CUSTOM_ACTIONS];
} EnemySlot;

void Level1_init();

#endif

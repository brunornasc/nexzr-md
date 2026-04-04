#ifndef _GAME_SCRIPT_H_
#define _GAME_SCRIPT_H_

#include <genesis.h>
#include "enemies.h"

#define MAX_CUSTOM_ACTIONS 5

typedef void (*ScriptCustomFunc)(Enemy* e);

typedef enum {
    ACTION_SPAWN,
    ACTION_SET_SHOOT_RATE,
    ACTION_SHOOT_ONCE,
    ACTION_SHOOT_SLASHER_DIRECTION,
    ACTION_DEACTIVATE,
    ACTION_STOP_SHOOT,
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
    ScriptCustomFunc customActions[MAX_CUSTOM_ACTIONS]; // slots para funções personalizadas
} EnemySlot;

// Funções do Motor de Script
void SCRIPT_init(EnemySlot* slots, u8 numSlots);
void SCRIPT_process(EnemySlot* slots, u8 numSlots, const ScriptItem* table, u16 tableLen, u16 currentFrame, int* scriptIndex);
void SCRIPT_updateShooting(EnemySlot* slots, u8 numSlots);

#endif
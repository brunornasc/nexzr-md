#include "game_script.h"
#include "bullet.h"
#include "game.h"

/**
 * Inicializa os slots de inimigos zerando tudo
 */
void SCRIPT_init(EnemySlot* slots, u8 numSlots) {
    for (u8 i = 0; i < numSlots; i++) {
        slots[i].enemy = NULL;
        slots[i].shootInterval = 0;
        slots[i].shootTimer = 0;
        slots[i].shootSpeed = 0;
        slots[i].customActions[0] = NULL;
        slots[i].customActions[1] = NULL;
    }
}

/**
 * Processa a tabela de script baseada no frame atual
 */
void SCRIPT_process(EnemySlot* slots, u8 numSlots, const ScriptItem* table, u16 tableLen, u16 currentFrame, int* scriptIndex) {
    while (*scriptIndex < tableLen && table[*scriptIndex].frame == currentFrame) {
        const ScriptItem* item = &table[(*scriptIndex)++];
        
        if (item->slot >= numSlots) continue;
        EnemySlot* slot = &slots[item->slot];

        switch (item->action) {
            case ACTION_SPAWN:
                if (slot->enemy && slot->enemy->active) ENEMY_deactivate(slot->enemy);
                slot->enemy = ENEMYFACTORY_createEnemy(item->type, item->x, item->y);
                // Reset automático de tiro ao spawnar para evitar "herança" de lixo
                slot->shootInterval = 0;
                slot->shootTimer = 0;
                slot->shootSpeed = 0;
                break;

            case ACTION_SET_SHOOT_RATE:
                slot->shootInterval = item->shootInterval;
                slot->shootTimer = 0;
                slot->shootSpeed = item->shootSpeed;
                break;

            case ACTION_SHOOT_ONCE:
                if (slot->enemy && slot->enemy->active)
                    BULLET_enemyShoot(slot->enemy->bulletSprite, slot->enemy->x, slot->enemy->y, 0, item->shootSpeed);
                break;

            case ACTION_SHOOT_SLASHER_DIRECTION:
                if (slot->enemy && slot->enemy->active)
                    BULLET_enemyShoot_slasherDirection(slot->enemy, &player, item->shootSpeed);
                break;

            case ACTION_DEACTIVATE:
                if (slot->enemy && slot->enemy->active) ENEMY_deactivate(slot->enemy);
                slot->enemy = NULL;
                break;

            case ACTION_STOP_SHOOT:
                slot->shootInterval = 0;
                break;

            case ACTION_SET_LINEAR_MOVEMENT:
                if (slot->enemy && slot->enemy->active)
                    ENEMY_setLinearMovement(slot->enemy, item->x, item->y); // Usar x e y como vx, vy
                break;

            case ACTION_SET_ARCED_MOVEMENT:
                if (slot->enemy && slot->enemy->active)
                    ENEMY_setArcedMovement(slot->enemy, item->x, item->y, item->shootInterval, item->shootSpeed);
                break;

            case ACTION_SET_MOVE_TO_PLAYER:
                if (slot->enemy && slot->enemy->active)
                    ENEMY_setMoveToPlayer(slot->enemy, item->shootSpeed); // shootSpeed como velocity
                break;

            case ACTION_CUSTOM_0:
                if (slot->enemy && slot->enemy->active && slot->customActions[0])
                    slot->customActions[0](slot->enemy);
                break;

            case ACTION_CUSTOM_1:
                if (slot->enemy && slot->enemy->active && slot->customActions[1])
                    slot->customActions[1](slot->enemy);
                break;
        }
    }
}

/**
 * Gerencia o cooldown de tiro de cada slot ativo
 */
void SCRIPT_updateShooting(EnemySlot* slots, u8 numSlots) {
    for (u8 i = 0; i < numSlots; i++) {
        EnemySlot* slot = &slots[i];
        
        // Se não tem inimigo ou o inimigo morreu, limpa o slot
        if (!slot->enemy || !slot->enemy->active) {
            slot->enemy = NULL;
            continue;
        }

        if (slot->shootInterval == 0) continue;

        if (slot->shootTimer == 0) {
            // Por padrão, o automático usa o slasher direction que você gosta
            BULLET_enemyShoot_slasherDirection(slot->enemy, &player, slot->shootSpeed);
            slot->shootTimer = slot->shootInterval;
        } else {
            slot->shootTimer--;
        }
    }
}
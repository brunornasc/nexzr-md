//
// Created by bruno on 14/02/2026.
//

#ifndef NEXZR_MD_ENEMYFACTORY_H
#define NEXZR_MD_ENEMYFACTORY_H

#include "enemies.h"

Enemy *ENEMYFACTORY_createEnemy(EnemyType type, s16 x, s16 y);
void ENEMYFACTORY_initEnemy(Enemy *e, EnemyType type, s16 x, s16 y);

#endif //NEXZR_MD_ENEMYFACTORY_H
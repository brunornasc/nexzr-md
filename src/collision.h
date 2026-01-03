#ifndef COLLISION_H
#define COLLISION_H

#include <genesis.h>
#include "enemies.h"
#include "player.h"
#include "bullet.h"
#include "game.h"

void COLLISION_checkBulletCollisionWithSlasher(Bullet* bullet);
void COLLISION_checkBulletCollisionWithEnemy(Bullet* bullet);

#endif
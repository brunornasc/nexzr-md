#ifndef COLLISION_H
#define COLLISION_H

#include <genesis.h>
#include "enemies.h"
#include "player.h"
#include "bullet.h"
#include "game.h"

int COLLISION_checkBulletCollisionWithSlasher(Bullet* bullet);
int COLLISION_checkBulletCollisionWithEnemy(Bullet* bullet);
int COLLISION_checkEnemyCollisionWithPlayer(Enemy* enemy);
void COLLISION_checkAllCollisions();

#endif
#ifndef ENEMY8_H
#define ENEMY8_H

#include <genesis.h>
#include "enemies.h"
// esta ultrapassando o limite de sprites do mega
typedef struct Enemy8Wrapper Enemy8Wrapper;
 
void           ENEMY8_initAll(void);
void           ENEMY8_create(Enemy *enemy, bool isEnemy);
void           ENEMY8_move(Enemy8Wrapper *w, s16 x, s16 y);
void           ENEMY8_destroy(Enemy8Wrapper *w);
void ENEMY8_destroy_by_index(u8 idx);


#endif
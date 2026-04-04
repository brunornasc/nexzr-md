#ifndef _LASERS_H_
#define _LASERS_H_

#include <genesis.h>

#define MAX_LASERS 3
#define LASER_DURATION 4
#define LASER_COOLDOWN 15
#define LASER_MAX_LENGTH 6  
#define LASER_TILE_INDEX  (TILE_USER_INDEX + 640)

void BACKGROUND_LASERS_init();
void BACKGROUND_LASERS_update();
void BACKGROUND_LASERS_dispose();

#endif
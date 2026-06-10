#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <genesis.h>

#define WARP_DURATION 230

void Background_init();
void Background_stop();
void Background_resume();
void Background_dispose();
bool Background_isRunning();

void BACKGROUND_EXPLOSIONS_init();
void BACKGROUND_EXPLOSIONS_update();
void BACKGROUND_EXPLOSIONS_dispose();

void BACKGROUND_LASERS_init();
void BACKGROUND_LASERS_update();
void BACKGROUND_LASERS_dispose();

#endif

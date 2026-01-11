#ifndef TEST_H
#define TEST_H

#include <genesis.h>

#define WARP_DURATION 230

void Background_init();
void Background_stop();
void Background_resume();
bool Background_isRunning();

#endif
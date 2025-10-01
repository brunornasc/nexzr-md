#ifndef GAME_H
#define GAME_H

#include <genesis.h>
#include "player.h"
#include "menu.h"
#include "entitymanager.h"
#include "callback.h"
#include "resources.h"

#define GAME_WINDOW_START_POSITION_TOP 0
#define GAME_WINDOW_START_POSITION_LEFT 0
#define GAME_WINDOW_WIDTH 320
#define GAME_WINDOW_HEIGHT 240

//u32 globalCounter;
static u32 currentFrame;
static u8 currentLevel;

static Player player;

void Game_init();
void Game_update();

#endif
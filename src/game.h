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

typedef enum {
  ENGLISH,
  BRAZILIAN,
  SPANISH
} languages;

typedef enum {
  EASY,
  NORMAL,
  HARD
} difficulties;

typedef struct {
  u8 language;
  bool md_mode;
  u8 difficulty;
} game_options_struct;

static game_options_struct game_options;

//u32 globalCounter;
static u32 currentFrame;
static u8 currentLevel;

static Player player;

void Game_init();
void Game_update();

#endif
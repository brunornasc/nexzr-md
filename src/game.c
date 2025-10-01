#include "game.h"
#include "menu.h"
#include "characters.h"

void initialize_screen() {
  VDP_setScreenWidth320();
  VDP_setScreenHeight240();
}

void Game_init() {
  initialize_screen();
  SPR_init();

  currentFrame = 0;

  Characters_init();
  Menu_init();
  currentLevel = 0;
}

void Game_update() {
  currentFrame++;

  Entity_executeAll();
  SPR_update();
  SYS_doVBlankProcess();
}
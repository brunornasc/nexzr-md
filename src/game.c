#include "game.h"
#include "menu.h"

void initialize_screen() {
  VDP_setScreenWidth320();
  VDP_setScreenHeight240();
}

void Game_init() {
  initialize_screen();
  SPR_init();
  Menu_init();
}

void Game_update() {
  Entity_executeAll();
  SPR_update();
  SYS_doVBlankProcess();
}
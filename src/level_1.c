#include "level_1.h"
#include "background.h"
#include "game.h"
#include "player.h"
#include "i18n.h"
#include "hud.h"

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) ;

void Level1_init() {
  Background_init();
  PLAYER_init(&player);
  Game_setJoyHandler(level1_joyEventHandler);
  Characters_prepareToPrint();

  currentLevel = LEVEL_1;

  // hud test
  HUD_setScore(100);
}

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state & BUTTON_START) {
            Game_pause();
            HUD_showPaused();
        }
    }
}

// TODO hud
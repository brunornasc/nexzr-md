#include "level_1.h"
#include "background.h"
#include "game.h"
#include "player.h"
#include "i18n.h"

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) ;

void Level1_init() {
  Background_init();
  PLAYER_init(&player);
  Game_setJoyHandler(level1_joyEventHandler);

  currentLevel = LEVEL_1;
  game_paused = false;
}

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state & BUTTON_START) {
            game_paused = !game_paused;

            if (Game_isPaused()) {
                Characters_print(TXT_PAUSED, 17, 13, FONT_ACTIVE);
                Background_stop();

            } else {
                VDP_clearPlane(BG_B, TRUE);
                Background_resume();

            }

        }
    }
}
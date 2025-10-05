#include "hud.h"
#include "game.h"

void Characters_clear(u16 x, u16 y, u16 width);

void HUD_init() {

}

void HUD_clear() {

}

void HUD_setScore(unsigned int score) {
  Characters_print("SC", 2, 1, FONT_ACTIVE);
  char sc[20];

  sprintf(sc, "%d", score);
  Characters_print(sc, 8, 2, FONT_ACTIVE);
}

void HUD_setLives(u8 lives) {

}

void HUD_showPaused() {
	if (Game_isPaused()) {
		Characters_print(TXT_PAUSED, 17, 13, FONT_ACTIVE);

	} else {		
  		Characters_clearXY(17, 13, 10);

	}
}


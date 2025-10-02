#include "game.h"
#include "menu.h"
#include "characters.h"

void _globalJoyEventHandler(u16 joy, u16 changed, u16 state);
void (*currentInputHandler)(u16 joy, u16 changed, u16 state) = NULL;

void initialize_screen() {
  VDP_setScreenWidth320();
  VDP_setScreenHeight240();
}

void Game_init() {
  initialize_screen();
  SPR_init();
  JOY_setEventHandler(&_globalJoyEventHandler);

  currentFrame = 0;
  I18N_setLanguage(LANG_EN);
  game_options.difficulty = NORMAL;
  game_options.md_mode = false;
  game_options.language = LANG_EN;

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

void Game_setJoyHandler(void (*handler)(u16 joy, u16 changed, u16 state)) {
  currentInputHandler = handler;
}

void _globalJoyEventHandler(u16 joy, u16 changed, u16 state) {
    if (currentInputHandler) {
        currentInputHandler(joy, changed, state);
    }
}

void Game_resetScreen() {
  VDP_clearPlane(BG_A, TRUE);
	VDP_clearPlane(BG_B, TRUE);
  PAL_setPalette(PAL0, palette_black, DMA);
	PAL_setPalette(PAL1, palette_black, DMA);
  PAL_setPalette(PAL2, palette_black, DMA);
	PAL_setPalette(PAL3, palette_black, DMA);
}

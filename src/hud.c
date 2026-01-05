#include "hud.h"
#include "game.h"
#include "resources.h"

Sprite* HUD_slasher;
Sprite* HUD_stageNum;
Sprite* HUD_stageText[5];
Sprite* teste;

void Characters_clear(u16 x, u16 y, u16 width);

void HUD_init() {
  HUD_slasher = SPR_addSprite(&hud_slasher, GAME_WINDOW_WIDTH - 32, GAME_WINDOW_HEIGHT - 32, TILE_ATTR(SLASHER_PALLETE, TRUE, FALSE, FALSE));
  HUD_setScore(0);
  HUD_setLives(Game_getLivesCount());
}

void HUD_clear() {
  SPR_releaseSprite(HUD_slasher);
  Characters_clearXY(2, 1, 10);
  Characters_clearXY(8, 2, 10);
}

void HUD_setScore(unsigned int score) {
  Characters_print("SC", 2, 1, FONT_ACTIVE);
  char sc[20];

  sprintf(sc, "%d", score);
  Characters_print(sc, 5, 2, FONT_ACTIVE);
}

void HUD_setLives(u8 lives) {
  char lvs[2];

  sprintf(lvs, "%d", lives);
  Characters_print(lvs, 38, 27, FONT_ACTIVE);
}

void HUD_showPaused() {
	if (Game_isPaused()) {
		Characters_print(TXT_PAUSED, 17, 13, FONT_ACTIVE);

	} else {		
  	Characters_clearXY(17, 13, 10);

	}
}

void HUD_showStage(u8 idx) {
    u16 centerX = GAME_WINDOW_WIDTH / 2;
    u16 centerY = GAME_WINDOW_HEIGHT / 4;
    
    u16 charWidth = 16; 
    u16 totalWidth = (charWidth * 5) + charWidth;
    u16 startX = centerX - (totalWidth / 2);

    for (int i = 0; i < 5; i++) {
        HUD_stageText[i] = SPR_addSprite(&level_stage, 
                                         startX + (i * charWidth), 
                                         centerY, 
                                         TILE_ATTR(SLASHER_PALLETE, TRUE, FALSE, FALSE));
        
        SPR_setFrame(HUD_stageText[i], i);
        SPR_setZ(HUD_stageText[i], SPR_MIN_DEPTH);
    }

    u16 numX = startX + (5 * charWidth) + 4;
    
    HUD_stageNum = SPR_addSprite(&level_numbers, numX, centerY, TILE_ATTR(SLASHER_PALLETE, TRUE, FALSE, FALSE));
    SPR_setFrame(HUD_stageNum, idx - 1);
    SPR_setZ(HUD_stageNum, SPR_MIN_DEPTH);
}

void HUD_dismissStage() {
    for (int i = 0; i < 5; i++) {
        SPR_releaseSprite(HUD_stageText[i]);
    }
    SPR_releaseSprite(HUD_stageNum);
}

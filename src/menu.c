#include "menu.h"
#include "game.h"

typedef enum {
  GAME_START,
  CARNIVAL_MODE,
  OPTIONS
} menu_options_main;

typedef enum {
  LANGUAGE,
  MD_MODE,
  DIFFICULTY,
  CREDITS,
  BACK
} menu_options_secondary;

typedef enum {
  MAIN,
  SECONDARY
} menus;

u8 option_selected = GAME_START;
u8 menu_current = MAIN;

void drawMainMenu();
void drawMainBackground();
void print_options();
void handleMainMenu(u16 joy, u16 changed, u16 state);
void joyEvent(u16 joy, u16 changed, u16 state);
void redrawMainMenu();
void drawSecondaryBackground();
void drawMenuSecondary();
void handleSecondaryMenu(u16 joy, u16 changed, u16 state);

void Menu_init() {
  	drawMainMenu();
 	JOY_setEventHandler(joyEvent);
}

void drawMainMenu() {
  	drawMainBackground();
	print_options();
}

void drawMainBackground() {
  	PAL_setPalette(PAL0, titlescreen.palette->data, DMA);
	VDP_drawImageEx(BG_B,
                  &titlescreen,
                  TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX),
                  GAME_WINDOW_START_POSITION_LEFT,
                  GAME_WINDOW_START_POSITION_TOP,
                  FALSE,
                  TRUE);
}

void drawSecondaryBackground() {
  	PAL_setPalette(PAL0, menuscreen.palette->data, DMA);
	VDP_drawImageEx(BG_B,
                  &menuscreen,
                  TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX),
                  GAME_WINDOW_START_POSITION_LEFT,
                  GAME_WINDOW_START_POSITION_TOP,
                  FALSE,
                  TRUE);
}

void print_options() {
  	Characters_print("GAME START", 15, 13, FONT_ACTIVE);
 	Characters_print("CARNIVAL MODE", 13, 15, FONT_INACTIVE);
 	Characters_print("OPTIONS", 17, 17, FONT_INACTIVE);
}

void joyEvent(u16 joy, u16 changed, u16 state) {
  if (menu_current == MAIN) {
 	handleMainMenu(joy, changed, state);

  } else if (menu_current == SECONDARY) {
 	handleSecondaryMenu(joy, changed, state);

  }
}

void handleMainMenu(u16 joy, u16 changed, u16 state) {

  if (joy == JOY_1) {
  	if (changed & state & BUTTON_DOWN) {
    	if (option_selected < OPTIONS) option_selected++;
    }
    if (changed & state & BUTTON_UP) {
    	if (option_selected > GAME_START) option_selected--;
    }
 	if (changed & state & BUTTON_START) {
   		if (option_selected == OPTIONS) {
		  menu_current = SECONDARY;
    	  option_selected = LANGUAGE;
		  VDP_clearPlane(BG_B, TRUE);
    	  drawSecondaryBackground();
    	  drawMenuSecondary();

		  return;
  		}
 	}

 	redrawMainMenu();
  }
}

void redrawMainMenu() {
	Characters_print("GAME START", 15, 13, option_selected == GAME_START ? FONT_ACTIVE : FONT_INACTIVE);
	Characters_print("CARNIVAL MODE", 13, 15, option_selected == CARNIVAL_MODE ? FONT_ACTIVE : FONT_INACTIVE);
	Characters_print("OPTIONS", 17, 17, option_selected == OPTIONS ? FONT_ACTIVE : FONT_INACTIVE);
}

void handleSecondaryMenu(u16 joy, u16 changed, u16 state) {
  if (joy == JOY_1) {
  	if (changed & state & BUTTON_DOWN) {
    	if (option_selected < BACK) option_selected++;
    }
    if (changed & state & BUTTON_UP) {
    	if (option_selected > LANGUAGE) option_selected--;
    }
 	if (changed & state & BUTTON_START) {
   		if (option_selected == BACK) {
		  menu_current = MAIN;
    	  option_selected = GAME_START;
		  VDP_clearPlane(BG_B, TRUE);
    	  drawMainBackground();
    	  drawMainMenu();

		  return;
  		}
  		else if (option_selected == LANGUAGE) {
    		game_options.language++;
   			if (game_options.language > 2)
     			game_options.language = 0;

  		}
 	}

 	drawMenuSecondary();
  }
}

void drawMenuSecondary() {
  	Characters_print("LANGUAGE", 5, 3, option_selected == LANGUAGE ? FONT_ACTIVE : FONT_INACTIVE);
	Characters_print("MD MODE", 5, 6, option_selected == MD_MODE ? FONT_ACTIVE : FONT_INACTIVE);
	Characters_print("DIFFICULTY", 5, 9, option_selected == DIFFICULTY ? FONT_ACTIVE : FONT_INACTIVE);
 	Characters_print("CREDITS", 5, 12, option_selected == CREDITS ? FONT_ACTIVE : FONT_INACTIVE);
 	Characters_print("BACK", 5, 18, option_selected == BACK ? FONT_ACTIVE : FONT_INACTIVE);

 	const char* language_option;

 	switch (game_options.language) {
   		case ENGLISH: {
    		language_option = "ENGLISH";
   			break;
   		}
  		case BRAZILIAN: {
    		language_option = "PORTUGUES BRASIL";
   			break;
   		}
  		case SPANISH: {
    		language_option = "ESPANHOL";
   			break;
   		}
 	}

 	Characters_print(language_option, 20, 3, option_selected == LANGUAGE ? FONT_ACTIVE : FONT_INACTIVE);
	Characters_print("OFF", 20, 6, option_selected == MD_MODE ? FONT_ACTIVE : FONT_INACTIVE);
	Characters_print("NORMAL", 20, 9, option_selected == DIFFICULTY ? FONT_ACTIVE : FONT_INACTIVE);
}
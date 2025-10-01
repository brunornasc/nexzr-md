#include "menu.h"
#include "game.h"

typedef enum {
  GAME_START,
  CARNIVAL_MODE,
  OPTIONS
} menu_options_main;

typedef enum {
  MAIN,
  SECONDARY
} menus;

u8 option_selected = GAME_START;
u8 menu_current = MAIN;

void drawMainMenu();
void draw_background();
void print_options();
void handleMainMenu(u16 joy, u16 changed, u16 state);
void joyEvent(u16 joy, u16 changed, u16 state);
void redrawMainMenu();

void Menu_init() {
  	drawMainMenu();
 	JOY_setEventHandler(joyEvent);
}

void drawMainMenu() {
  	draw_background();
	print_options();
}

void draw_background() {
  	PAL_setPalette(PAL0, titlescreen.palette->data, DMA);
	VDP_drawImageEx(BG_B,
                  &titlescreen,
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

 	redrawMainMenu();
  }
}

void redrawMainMenu() {
  	Characters_print("GAME START", 15, 13, option_selected == GAME_START ? FONT_ACTIVE : FONT_INACTIVE);
 	Characters_print("CARNIVAL MODE", 13, 15, option_selected == CARNIVAL_MODE ? FONT_ACTIVE : FONT_INACTIVE);
 	Characters_print("OPTIONS", 17, 17, option_selected == OPTIONS ? FONT_ACTIVE : FONT_INACTIVE);
}
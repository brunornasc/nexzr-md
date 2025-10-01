#include "menu.h"
#include "game.h"

typedef enum {
  GAME_START,
  CARNIVAL_MODE,
  OPTIONS
} menu_options;

u8 option_selected = GAME_START;

void draw_background();
void print_options();

void Menu_init() {
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
}
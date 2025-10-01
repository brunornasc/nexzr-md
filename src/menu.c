#include <genesis.h>
#include "resources.h"
#include "menu.h"

void Menu_init() {
  PAL_setPalette(PAL0, titlescreen.palette->data, DMA);
  //bool VDP_drawImageEx(VDPPlane plane, const Image *image, u16 basetile, u16 x, u16 y, bool loadpal, bool dma);
  VDP_drawImageEx(BG_A,
                  &titlescreen,
                  TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX),
                  GAME_WINDOW_START_POSITION_LEFT,
                  GAME_WINDOW_START_POSITION_TOP,
                  FALSE,
                  TRUE);
}
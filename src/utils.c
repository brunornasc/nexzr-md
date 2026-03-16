#include "utils.h"
#include "game.h"

void UTILS_wait(u8 seconds) {

  unsigned t = seconds * FRAMES_PER_SECOND;

  for (unsigned i = 0; i < t; i++)
    SYS_doVBlankProcess();
}

void UTILS_loadAndFadeIn(Image img, u8 seconds) {
  unsigned t = seconds * FRAMES_PER_SECOND;

  VDP_drawImageEx(BG_A,
  &img,
  TILE_ATTR_FULL(BACKGROUND_PALLETE, FALSE, FALSE, FALSE, TILE_USER_INDEX),
  GAME_WINDOW_START_POSITION_LEFT,
  GAME_WINDOW_START_POSITION_TOP,
  FALSE,
  TRUE);

  PAL_fadeIn(0, 63, img.palette->data, t, FALSE);
  PAL_setPalette(BACKGROUND_PALLETE, img.palette->data, DMA);

  while (PAL_isDoingFade())
	 SYS_doVBlankProcess();
}

void UTILS_fadeOut(u8 seconds) {
  unsigned t = seconds * FRAMES_PER_SECOND;

  PAL_fadeOut(0, 63, t, FALSE);

  while (PAL_isDoingFade())
	 SYS_doVBlankProcess();
}

// Função auxiliar para desenhar imagem espelhada horizontalmente
void UTILS_drawImageMirroredH(VDPPlane plane, const Image* img, u16 paletteIndex, 
                         u16 startTileIndex, s16 destX, s16 destY) {
    u16 w = 8;  // largura em tiles
    u16 h = 40;                           // altura em tiles
    
    for (u16 ty = 0; ty < h; ty++) {
        for (u16 tx = 0; tx < w; tx++) {
            // Tile espelhado: pega o tile da direita para esquerda
            u16 srcTileOffset = (ty * w) + (w - 1 - tx);
            u16 tileIndex = startTileIndex + srcTileOffset;
            
            // Posição destino: coluna invertida
            s16 px = destX + (w - 1 - tx);
            s16 py = destY + ty;
            
            // Flip horizontal no tile individual + reordenação das posições
            u16 attr = TILE_ATTR_FULL(paletteIndex, FALSE, FALSE, TRUE, tileIndex);
            VDP_setTileMapXY(plane, attr, px, py);
        }
    }
}
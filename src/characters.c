#include "characters.h"
#include <genesis.h>

#define FONT_WIDTH        8
#define FONT_HEIGHT       4
#define CHARS_PER_ROW     23

void Characters_init() {
    VDP_loadTileSet(&characters, TILE_FONT_INDEX, DMA);
}

void Characters_prepareToPrint() {
    PAL_setPalette(PAL1, characters_pallete.palette->data, DMA);
}

void Characters_print(const char* str, u16 x, u16 y, FontState state) {
    u16 i = 0;

    while (str[i] != '\0') {
        char c = str[i];
        u16 tileIndex = 0;

        // Letras A-Z
        if (c >= 'A' && c <= 'Z') {
            tileIndex = (c - 'A');   // índice dentro do alfabeto
        }
        // Dígitos 0-9
        else if (c >= '0' && c <= '9') {
            tileIndex = 26 + (c - '0');  // supondo que 0-9 vêm logo após A-Z
        }
        else {
            // caractere não suportado → pula
            i++;
            x++;
            continue;
        }

        tileIndex += (state * CHARS_PER_ROW);

        // Desenha o tile na tela
        VDP_setTileMapXY(BG_B,
            TILE_ATTR_FULL(PAL1, 0, 0, 0, TILE_FONT_INDEX + tileIndex),
            x + i, y);

        i++;
    }

}

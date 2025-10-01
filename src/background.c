#include "background.h"

#define MAX_STARS 50

typedef struct {
    int x;
    int y;
    int speed;
    int color; // 0 = branco, 1 = azul
} Star;

static Star stars[MAX_STARS];

// Índices de tiles
static u16 emptyTileIndex;
static u16 starWhiteTileIndex;
static u16 starBlueTileIndex;

void Background_init() {
    // Tile vazio
    u32 emptyTile[8] = {0};

    // Tile com 1 pixel branco (linha 3, coluna 4)
    u32 whiteTile[8] = {0};
    whiteTile[3] = 0x00000100; // cada 4 bits = 1 pixel → aqui cor 1

    // Tile com 1 pixel azul (linha 3, coluna 4)
    u32 blueTile[8] = {0};
    blueTile[3] = 0x00000200; // mesma posição, mas cor 2

    // Carregar tiles na VRAM
    emptyTileIndex     = TILE_USER_INDEX;
    starWhiteTileIndex = emptyTileIndex + 1;
    starBlueTileIndex  = emptyTileIndex + 2;

    VDP_loadTileData(emptyTile, emptyTileIndex, 1, DMA);
    VDP_loadTileData(whiteTile, starWhiteTileIndex, 1, DMA);
    VDP_loadTileData(blueTile, starBlueTileIndex, 1, DMA);

    // Inicializar estrelas
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = random() % 40; // 40 tiles de largura
        stars[i].y = random() % 28; // 28 tiles de altura
        stars[i].speed = 1 + (random() % 3);
        stars[i].color = random() % 2;
    }
}

void Background_update(void* context) {
    for (int i = 0; i < MAX_STARS; i++) {
        // Limpa posição antiga
        VDP_setTileMapXY(BG_A, emptyTileIndex, stars[i].x, stars[i].y);

        // Move estrela
        stars[i].y += stars[i].speed;
        if (stars[i].y >= 28) {
            stars[i].y = 0;
            stars[i].x = random() % 40;
            stars[i].speed = 1 + (random() % 3);
            stars[i].color = random() % 2;
        }

        // Desenha na nova posição
        u16 tile = (stars[i].color == 0) ? starWhiteTileIndex : starBlueTileIndex;
        VDP_setTileMapXY(BG_A, tile, stars[i].x, stars[i].y);
    }
}

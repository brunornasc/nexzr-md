#include "background_lasers.h"

typedef struct {
    s16 tileX, tileY;  
    u8 length, angle, colorIndex, tileVramIndex;
    s8 timer;
} Laser;

static Laser lasers[MAX_LASERS];
static const u8 laserColors[3] = {2, 5, 8};

static const u32 laserTiles[4][8] = {
    { 0x00000000, 0x00000000, 0x00000000, 0x55555555, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00500000, 0x00500000, 0x00500000, 0x00500000, 0x00500000, 0x00500000, 0x00500000, 0x00500000 },
    { 0x50000000, 0x05000000, 0x00500000, 0x00050000, 0x00005000, 0x00000500, 0x00000050, 0x00000005 },
    { 0x00000005, 0x00000050, 0x00000500, 0x00005000, 0x00050000, 0x00500000, 0x05000000, 0x50000000 }
};

static void createLaserTile(u8 angle, u8 color, u32* out) {
    for (u8 r = 0; r < 8; r++) {
        u32 line = laserTiles[angle][r], colored = 0;
        for (s8 n = 7; n >= 0; n--) {
            u8 v = (line >> (n << 2)) & 0xF;
            colored |= ((u32)(v == 5 ? color : v) << (n << 2));
        }
        out[r] = colored;
    }
}

void BACKGROUND_LASERS_init() {
    for (u8 i = 0; i < MAX_LASERS; i++) {
        lasers[i].timer = -(random() % LASER_COOLDOWN);
        lasers[i].tileVramIndex = LASER_TILE_INDEX + (i << 2);
    }
}

void BACKGROUND_LASERS_update() {
    for (u8 i = 0; i < MAX_LASERS; i++) {
        Laser* l = &lasers[i];
        if (l->timer < 0) { l->timer++; continue; }
        
        if (l->timer == 0) {
            l->tileX = random() % 40; l->tileY = random() % 28;
            l->length = 1 + (random() % LASER_MAX_LENGTH); l->angle = random() % 4;
            l->colorIndex = laserColors[random() % 3];
            u32 tile[8]; createLaserTile(l->angle, l->colorIndex, tile);
            VDP_loadTileData(tile, l->tileVramIndex + l->angle, 1, DMA);
            l->timer = 1;
        }

        if (l->timer > 0 && l->timer <= LASER_DURATION) {
            u16 attr = TILE_ATTR_FULL(PAL2, 0, 0, 0, l->tileVramIndex + l->angle);
            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX, y = l->tileY;
                if (l->angle == 0) x += j; else if (l->angle == 1) y += j;
                else if (l->angle == 2) { x += j; y += j; } else { x += j; y -= j; }
                if (x >= 0 && x < 40 && y >= 0 && y < 28) VDP_setTileMapXY(VDP_BG_B, attr, x, y);
            }
            l->timer++;
        }

        if (l->timer > LASER_DURATION) {
            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX, y = l->tileY;
                if (l->angle == 0) x += j; else if (l->angle == 1) y += j;
                else if (l->angle == 2) { x += j; y += j; } else { x += j; y -= j; }
                if (x >= 0 && x < 40 && y >= 0 && y < 28) VDP_setTileMapXY(VDP_BG_B, 0, x, y);
            }
            l->timer = -(random() % (LASER_COOLDOWN * 2));
        }
    }
}

void BACKGROUND_LASERS_dispose() {
    // Limpar tiles da tela se necessário ao fechar a fase
}
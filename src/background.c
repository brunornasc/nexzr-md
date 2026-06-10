#include "background.h"
#include "game.h"
#include "resources.h"
#include "entitymanager.h"

// --- Star field ---
#define STAR_COUNT 20
#define MAX_STAR_HEIGHT 5
#define DEACELERATION_FRAMES_ANIM 7

typedef struct {
    Sprite* spr[MAX_STAR_HEIGHT];
    int x, y;
    u8 size;
    u8 speed;
    u8 colorFrame;
    bool done;
    u8 decelCounter;
    u8 blinkCounter;
    bool visible;
} Star;

static Star stars[STAR_COUNT];
static u8 warpTimer = 0;
static bool isWarping = true;
static bool isDeacelerating = false;
static u8 deAceleratedStarsCount = 0;
static Entity* backgroundTask;

static void update_background(void* context);

void Background_init() {
    Game_resetScreen();
    currentFrame = 0;

    PAL_setPalette(SLASHER_PALLETE, slasher.palette->data, DMA);

    for (int i = 0; i < STAR_COUNT; i++) {
        u8 size = (random() % MAX_STAR_HEIGHT) + 1;
        u8 speed = ((3 + size) << 1) + (random() % 2);
        u8 colorFrame = random() % 3;

        int x = random() % GAME_WINDOW_WIDTH;
        int y = random() % 224;

        for (int j = 0; j < size; j++) {
            stars[i].spr[j] = SPR_addSprite(&star_warp, x, (y + (j << 3)), TILE_ATTR(SLASHER_PALLETE, FALSE, FALSE, FALSE));
            SPR_setFrame(stars[i].spr[j], colorFrame);
        }

        stars[i].x = x;
        stars[i].y = y;
        stars[i].size = size;
        stars[i].speed = speed;
        stars[i].colorFrame = colorFrame;
        stars[i].done = false;
        stars[i].decelCounter = DEACELERATION_FRAMES_ANIM;
        stars[i].blinkCounter = WARP_DURATION + 20;
        stars[i].visible = true;
    }

    backgroundTask = Entity_add(NULL, update_background);
}

void Background_stop() {
    backgroundTask->active = false;
}

void Background_resume() {
    backgroundTask->active = true;
}

bool Background_isRunning() {
    return backgroundTask->active;
}

static void update_background(void* context) {
    if (Game_isPaused()) return;

    if ((currentFrame & 1) && !isDeacelerating && !isWarping) return;

    for (int i = 0; i < STAR_COUNT; i++) {
        Star* s = &stars[i];

        s->y += s->speed;
        if (s->y > GAME_WINDOW_HEIGHT) {
            s->y = GAME_WINDOW_START_POSITION_TOP;
            s->x = random() % GAME_WINDOW_WIDTH;
        }

        s->blinkCounter--;

        if (s->blinkCounter < (s->speed + 2)) {
            SPR_setVisibility(s->spr[0], HIDDEN);
            s->blinkCounter = random() % 20 + 1;
            s->visible = false;

        } else {
            if (!s->visible) {
                SPR_setVisibility(s->spr[0], VISIBLE);
                s->visible = true;
            }

            int y = s->y;

            for (int j = 0; j < s->size; j++) {
                SPR_setPosition(s->spr[j], s->x, y);
                SPR_setAlwaysAtBottom(s->spr[j]);
                y -= 8;
            }
        }

        if (isDeacelerating) {
            if (s->decelCounter > 0) s->decelCounter--;

            if (s->decelCounter == 0) {
                if (s->size > 1) {
                    SPR_releaseSprite(s->spr[s->size - 1]);
                    s->spr[s->size - 1] = NULL;
                    s->size--;
                    s->speed = 2 + s->size;

                } else if (!s->done) {
                    SPR_setAnimAndFrame(s->spr[0], 1, s->colorFrame);
                    s->speed = (random() % 4) + 1;
                    s->done = true;
                    deAceleratedStarsCount++;
                }

                s->decelCounter = DEACELERATION_FRAMES_ANIM;
            }
        }

        if (deAceleratedStarsCount >= STAR_COUNT) {
            isDeacelerating = false;
        }
    }

    if (isWarping) {
        warpTimer++;

        if (warpTimer > WARP_DURATION){
            isDeacelerating = true;
            isWarping = false;
        }
    }
}

void Background_dispose() {
    for (int i = 0; i < STAR_COUNT; i++) {
        for (int j = 0; j < MAX_STAR_HEIGHT; j++) {
            if (stars[i].spr[j] != NULL) {
                SPR_releaseSprite(stars[i].spr[j]);
                stars[i].spr[j] = NULL;
            }
        }
    }

    warpTimer = 0;
    isWarping = true;
    isDeacelerating = false;
    deAceleratedStarsCount = 0;

    if (backgroundTask != NULL) {
        Entity_removeEntity(backgroundTask->index);
        backgroundTask = NULL;
    }
}

// --- Background Explosions ---
#define MAX_EXPLOSIONS 3
#define EXPLOSION_ANIMATION_FRAMES 8
#define EXPLOSION_FRAME_COUNT 8
#define EXPLOSION_COOLDOWN 30

typedef struct {
    s16 x, y;
    Sprite* sprite;
    s8 frameIndex;
    u8 currentFrame;
    u8 totalFrames;
} Explosion;

static Explosion explosions[MAX_EXPLOSIONS];

void BACKGROUND_EXPLOSIONS_init() {
    PAL_setPalette(PAL2, enemy_0003.palette->data, DMA);

    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        Explosion* e = &explosions[i];
        e->frameIndex = -(random() % EXPLOSION_COOLDOWN);
        e->sprite = SPR_addSprite(&stage1_explosions, 0, 0, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
        SPR_setVisibility(e->sprite, HIDDEN);
    }
}

void BACKGROUND_EXPLOSIONS_update() {
    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        Explosion* e = &explosions[i];
        if (e->frameIndex < 0) { e->frameIndex++; continue; }

        if (e->frameIndex == 0) {
            e->x = random() % (320 - 16);
            e->y = random() % (224 - 16);
            SPR_setPosition(e->sprite, e->x, e->y);
            SPR_setVisibility(e->sprite, VISIBLE);
            e->currentFrame = 0;
            e->totalFrames = EXPLOSION_FRAME_COUNT;
            e->frameIndex = 1;
            continue;
        }

        e->currentFrame++;
        if (e->currentFrame >= e->totalFrames) e->currentFrame = 0;
        SPR_setFrame(e->sprite, e->currentFrame);
        SPR_setAlwaysAtBottom(e->sprite);

        if (++e->frameIndex >= EXPLOSION_ANIMATION_FRAMES) {
            SPR_setVisibility(e->sprite, HIDDEN);
            e->frameIndex = -(random() % (EXPLOSION_COOLDOWN << 1));
        }
    }
}

void BACKGROUND_EXPLOSIONS_dispose() {
    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].sprite) SPR_releaseSprite(explosions[i].sprite);
    }
}

// --- Background Lasers ---
#define MAX_LASERS 3
#define LASER_DURATION 4
#define LASER_COOLDOWN 15
#define LASER_MAX_LENGTH 6
#define LASER_TILE_INDEX  (TILE_USER_INDEX + 640)

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
            u16 attr = TILE_ATTR_FULL(PAL2, FALSE, 0, 0, l->tileVramIndex + l->angle);
            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX, y = l->tileY;
                if (l->angle == 0) x += j; else if (l->angle == 1) y += j;
                else if (l->angle == 2) { x += j; y += j; } else { x += j; y -= j; }
                if (x >= 0 && x < 40 && y >= 0 && y < 28) VDP_setTileMapXY(VDP_BG_A, attr, x, y);
            }
            l->timer++;
        }

        if (l->timer > LASER_DURATION) {
            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX, y = l->tileY;
                if (l->angle == 0) x += j; else if (l->angle == 1) y += j;
                else if (l->angle == 2) { x += j; y += j; } else { x += j; y -= j; }

                if (x >= 0 && x < 40 && y >= 0 && y < 28)
                    VDP_setTileMapXY(VDP_BG_A, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, TILE_SYSTEM_INDEX), x, y);
            }
            l->timer = -(random() % (LASER_COOLDOWN * 2));
        }
    }
}

void BACKGROUND_LASERS_dispose() {
}

#include "background_explosions.h"
#include "resources.h" // Onde está o stage1_explosions

typedef struct {
    s16 x, y;
    Sprite* sprite;
    s8 frameIndex;   
    u8 currentFrame; 
    u8 totalFrames;  
} Explosion;

static Explosion explosions[MAX_EXPLOSIONS];

void BACKGROUND_EXPLOSIONS_init() {
    // Pegando a paleta do inimigo 3 como base (ajuste conforme seu jogo)
    PAL_setPalette(PAL2, enemy_0003.palette->data, DMA);

    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        Explosion* e = &explosions[i];
        e->frameIndex = -(random() % EXPLOSION_COOLDOWN);
        e->sprite = SPR_addSprite(&stage1_explosions, 0, 0, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
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
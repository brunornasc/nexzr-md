#include <genesis.h>
#include "game.h"
#include "resources.h"
#include "enemies.h"

// 128x320px = 16x40 tiles
#define ENEMY8_WIDTH_TILES   16
#define ENEMY8_HEIGHT_TILES  40

// Posição na VRAM para os tiles da imagem (ajuste se necessário)
#define ENEMY8_VRAM_INDEX    (TILE_USER_INDEX + 300)

typedef struct {
    bool    active;
    s16     x;
    s16     y;
    Enemy  *base;
    Sprite *head_left;
    Sprite *head_right;
} Enemy8Wrapper;

static Enemy8Wrapper enemy8_wrappers[3];
static bool enemy8_vram_loaded = FALSE;

// -----------------------------------------------------------------------
// Carregamento de Recursos
// -----------------------------------------------------------------------
static void enemy8_prepare_vram() {
    if (enemy8_vram_loaded) return;

    // Carrega os tiles da imagem completa na VRAM
    VDP_loadTileSet(enemy_0008.tileset, ENEMY8_VRAM_INDEX, DMA);
    
    // Aplica a paleta da imagem
    PAL_setPalette(BACKGROUND_PALLETE, enemy_0008.palette->data, DMA);

    enemy8_vram_loaded = TRUE;
}

// -----------------------------------------------------------------------
// API do Inimigo
// -----------------------------------------------------------------------

void ENEMY8_initAll(void) {
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        enemy8_wrappers[i].active = FALSE;
    }
    enemy8_vram_loaded = FALSE;
}

void ENEMY8_create(Enemy *enemy, bool isEnemy) {
    enemy8_prepare_vram();

    Enemy8Wrapper *w = &enemy8_wrappers[enemy->index];
    w->active = TRUE;
    w->x = enemy->x;
    w->y = enemy->y;
    w->base = enemy;

    // Desenha a nave no Plano B uma única vez
    // Usamos VDP_setTileMapEx para mapear a imagem inteira para a VRAM
    VDP_setTileMapEx(BG_B, enemy_0008.tilemap, 
                     TILE_ATTR_FULL(BACKGROUND_PALLETE, isEnemy, 0, 0, ENEMY8_VRAM_INDEX), 
                     0, 0, 0, 0, ENEMY8_WIDTH_TILES, ENEMY8_HEIGHT_TILES, DMA);

    // Cria os sprites da cabeça para colisão (VDP Sprites)
    w->head_left = SPR_addSprite(&enemy_0008_01, w->x + 48, w->y + 16, 
                                 TILE_ATTR(BACKGROUND_PALLETE, isEnemy, FALSE, FALSE));
    w->head_right = SPR_addSprite(&enemy_0008_01, w->x + 64, w->y + 16, 
                                  TILE_ATTR(BACKGROUND_PALLETE, isEnemy, FALSE, TRUE));

    enemy->sprite = w->head_left; // Referência para colisão do sistema
}

void ENEMY8_move(Enemy8Wrapper *w, s16 x, s16 y) {
    w->x = x;
    w->y = y;
    w->base->x = x;
    w->base->y = y;

    // O "Pulo do Gato": Movemos o PLANO B para mover a nave
    // Valores negativos para scroll horizontal pois o plano "corre" no sentido oposto
    VDP_setHorizontalScroll(BG_B, -x);
    VDP_setVerticalScroll(BG_B, y);

    // Sincroniza os sprites da cabeça com a posição do plano
    if (w->head_left)  SPR_setPosition(w->head_left,  x + 48, y);
    if (w->head_right) SPR_setPosition(w->head_right, x + 64, y);
}

void ENEMY8_destroy(Enemy8Wrapper *w) {
    // Limpa o plano para o chefe sumir
    VDP_clearPlane(BG_B, TRUE);

    if (w->head_left)  SPR_releaseSprite(w->head_left);
    if (w->head_right) SPR_releaseSprite(w->head_right);

    // Head destruída (opcional: criar efeito de explosão aqui)
    w->active = FALSE;
}
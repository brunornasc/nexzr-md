#include "level_1.h"
#include "background.h"
#include "game.h"
#include "player.h"
#include "i18n.h"
#include "hud.h"
#include "entitymanager.h"
#include "enemies.h"
#include "bullet.h"
#include "collision.h"
#include "resources.h"
#include "sounds.h"
#include "enemyfactory.h"

// ========================================================
// EXPLOSIONS
// ========================================================

#define MAX_EXPLOSIONS 3
#define EXPLOSION_ANIMATION_FRAMES 8 //1 % 3 segundo
#define EXPLOSION_FRAME_COUNT 8
#define EXPLOSION_COOLDOWN  (30)
#define EXPLOSION_SIZE     16
#define EXPLOSION_MAX_X  (GAME_WINDOW_WIDTH  > EXPLOSION_SIZE ? GAME_WINDOW_WIDTH  - EXPLOSION_SIZE : 1)
#define EXPLOSION_MAX_Y  (GAME_WINDOW_HEIGHT > EXPLOSION_SIZE ? GAME_WINDOW_HEIGHT - EXPLOSION_SIZE : 1)

typedef struct {
  s16 x;
  s16 y;
  Sprite* sprite;
  s8 frameIndex;   // negativo = cooldown, zero = spawn, positivo = tempo de vida
  u8 currentFrame; // frame atual da animação (separado do timer)
  u8 totalFrames;  // total de frames do sprite (preenchido no spawn)
} Explosion;

// ========================================================
// LASERS
// ========================================================

#define MAX_LASERS 3
#define LASER_DURATION 4
#define LASER_COOLDOWN 15
#define LASER_MAX_LENGTH 6  // em tiles (6*8 = 48px)
#define LASER_TILE_INDEX 1  // Índice base para os tiles dos lasers (usa 1-20: 5 lasers * 4 ângulos)

typedef struct {
    s16 tileX, tileY;  // posição em tiles
    u8 length;         // comprimento em tiles (1-6)
    u8 angle;          // 0=horizontal, 1=vertical, 2=diagonal \, 3=diagonal /
    u8 colorIndex;     // índice da cor (2, 5 ou 8)
    u8 tileVramIndex;  // índice único na VRAM para este laser
    s8 timer;          // negativo = cooldown, positivo = visível
} Laser;

const u8 laserColors[3] = {2, 5, 8};

const u32 laserTiles[4][8] = {
    // Horizontal (máscara - 5 será substituído pela cor escolhida)
    {
        0x00000000,
        0x00000000,
        0x00000000,
        0x55555555,  // linha no meio
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000
    },
    // Vertical
    {
        0x00500000,
        0x00500000,
        0x00500000,
        0x00500000,
        0x00500000,
        0x00500000,
        0x00500000,
        0x00500000
    },
    // Diagonal \ (top-left to bottom-right)
    {
        0x50000000,
        0x05000000,
        0x00500000,
        0x00050000,
        0x00005000,
        0x00000500,
        0x00000050,
        0x00000005
    },
    // Diagonal / (top-right to bottom-left)
    {
        0x00000005,
        0x00000050,
        0x00000500,
        0x00005000,
        0x00050000,
        0x00500000,
        0x05000000,
        0x50000000
    }
};

// ========================================================
// SCRIPT DE INIMIGOS
// ========================================================

#define LEVEL1_ENEMY_SLOTS 4

typedef enum {
    LEVEL1_ACTION_SPAWN,
    LEVEL1_ACTION_SET_SHOOT_RATE,
    LEVEL1_ACTION_SHOOT_ONCE,
    LEVEL1_ACTION_DEACTIVATE,
    LEVEL1_ACTION_STOP_SHOOT
} Level1ActionType;

typedef struct {
    u16 frame;
    Level1ActionType action;
    u8 slot;
    EnemyType type;
    s16 x;
    s16 y;
    u16 shootInterval;
    s16 shootSpeed;
} Level1ScriptItem;

typedef struct {
    Enemy* enemy;
    u16 shootInterval;
    u16 shootTimer;
    s16 shootSpeed;
} Level1EnemySlot;

// TODO: adicionar boss wave, transição de fase e mais ondas
static const Level1ScriptItem level1_script_table[] = {
    // --- WAVE 1: inimigos pela direita ---
    { 500,  LEVEL1_ACTION_SPAWN,          0, ENEMY_TYPE_1, GAME_WINDOW_WIDTH-80, -8, 0,  0 },
    { 540,  LEVEL1_ACTION_SPAWN,          1, ENEMY_TYPE_1, GAME_WINDOW_WIDTH-80, -8, 0,  0 },
    { 560,  LEVEL1_ACTION_SET_SHOOT_RATE, 0, 0,            0,                    0,  40, 5 },
    { 580,  LEVEL1_ACTION_SPAWN,          2, ENEMY_TYPE_1, GAME_WINDOW_WIDTH-80, -8, 0,  0 },
    { 580,  LEVEL1_ACTION_SHOOT_ONCE,     0, 0,            0,                    0,  0,  5 },
    { 580,  LEVEL1_ACTION_SHOOT_ONCE,     1, 0,            0,                    0,  0,  5 },
    { 600,  LEVEL1_ACTION_SHOOT_ONCE,     0, 0,            0,                    0,  0,  5 },
    { 600,  LEVEL1_ACTION_SHOOT_ONCE,     1, 0,            0,                    0,  0,  6 },
    { 600,  LEVEL1_ACTION_SHOOT_ONCE,     2, 0,            0,                    0,  0,  5 },
    { 620,  LEVEL1_ACTION_SPAWN,          3, ENEMY_TYPE_1, GAME_WINDOW_WIDTH-80, -8, 0,  0 },
    { 620,  LEVEL1_ACTION_SET_SHOOT_RATE, 3, 0,            0,                    0,  45, 5 },
    { 640,  LEVEL1_ACTION_SHOOT_ONCE,     0, 0,            0,                    0,  0,  6 },
    { 640,  LEVEL1_ACTION_SHOOT_ONCE,     1, 0,            0,                    0,  0,  6 },
    { 640,  LEVEL1_ACTION_SHOOT_ONCE,     2, 0,            0,                    0,  0,  5 },
    { 640,  LEVEL1_ACTION_SHOOT_ONCE,     3, 0,            0,                    0,  0,  5 },

    // --- FIM DA WAVE 1 ---
    { 1050, LEVEL1_ACTION_DEACTIVATE,     0, 0, 0, 0, 0, 0 },
    { 1050, LEVEL1_ACTION_DEACTIVATE,     1, 0, 0, 0, 0, 0 },
    { 1050, LEVEL1_ACTION_DEACTIVATE,     2, 0, 0, 0, 0, 0 },
    { 1050, LEVEL1_ACTION_DEACTIVATE,     3, 0, 0, 0, 0, 0 },

    // --- WAVE 2: inimigos pela esquerda ---
    { 1100, LEVEL1_ACTION_SPAWN,          0, ENEMY_TYPE_1, 80, -8, 0,  0 },
    { 1140, LEVEL1_ACTION_SPAWN,          1, ENEMY_TYPE_1, 80, -8, 0,  0 },
    { 1180, LEVEL1_ACTION_SET_SHOOT_RATE, 0, 0,            0,  0,  36, 5 },
    { 1220, LEVEL1_ACTION_SHOOT_ONCE,     0, 0,            0,  0,  0,  5 },
    { 1220, LEVEL1_ACTION_SHOOT_ONCE,     1, 0,            0,  0,  0,  5 },
    { 1220, LEVEL1_ACTION_SPAWN,          2, ENEMY_TYPE_1, 80, -8, 0,  0 },
    { 1260, LEVEL1_ACTION_SHOOT_ONCE,     0, 0,            0,  0,  0,  5 },
    { 1260, LEVEL1_ACTION_SHOOT_ONCE,     1, 0,            0,  0,  0,  6 },
    { 1260, LEVEL1_ACTION_SHOOT_ONCE,     2, 0,            0,  0,  0,  5 },
    { 1300, LEVEL1_ACTION_SPAWN,          3, ENEMY_TYPE_1, 80, -8, 0,  0 },
    { 1340, LEVEL1_ACTION_SHOOT_ONCE,     0, 0,            0,  0,  0,  6 },
    { 1340, LEVEL1_ACTION_SHOOT_ONCE,     1, 0,            0,  0,  0,  6 },
    { 1340, LEVEL1_ACTION_SHOOT_ONCE,     2, 0,            0,  0,  0,  5 },
    { 1340, LEVEL1_ACTION_SHOOT_ONCE,     3, 0,            0,  0,  0,  5 },

    // TODO: wave 3, wave 4, boss
};
static const int level1_script_len = sizeof(level1_script_table) / sizeof(level1_script_table[0]);

// ========================================================
// STATE
// ========================================================

Laser lasers[MAX_LASERS];
Explosion explosions[MAX_EXPLOSIONS];
Entity* level1Entity;
unsigned long level1_frame = 0;

static Level1EnemySlot level1_slots[LEVEL1_ENEMY_SLOTS];
static int level1_script_index = 0;

// ========================================================
// FORWARD DECLARATIONS
// ========================================================

void level1_joyEventHandler(u16 joy, u16 changed, u16 state);
void level1_update(void* context);
void level1_dispose();
void level1_script();
void LEVEL1_initExplosions();
void LEVEL1_updateExplosions();
void LEVEL1_disposeExplosions();
void LEVEL1_initLasers();
void LEVEL1_updateLasers();
void LEVEL1_disposeLasers();
static inline __attribute__((always_inline))
void LEVEL1_createColoredTile(u8 angle, u8 colorIndex, u32* output);
static void LEVEL1_resetEnemySlots(void);
static void LEVEL1_processScript(void);
static void LEVEL1_updateEnemyShooting(void);

// ========================================================
// INIT / DISPOSE
// ========================================================

void Level1_init() {
    currentLevel = LEVEL_1;
    Background_init();
    HUD_init();
    ENEMY_initializeAll();
    LEVEL1_resetEnemySlots();
    level1Entity = Entity_add(NULL, level1_update);

    XGM_startPlay(track1);
    PLAYER_init(&player);
    Game_setJoyHandler(level1_joyEventHandler);
}

void level1_dispose() {
    Entity_removeEntity(level1Entity->index);
    LEVEL1_disposeLasers();
    LEVEL1_disposeExplosions();
}

// ========================================================
// INPUT
// ========================================================

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state & BUTTON_START) {
            Game_pause();
            HUD_showPaused();
        }
    }
}

// ========================================================
// UPDATE
// ========================================================

void level1_update(void* context) {
    if (Game_isPaused()) return;

    level1_script();
    BULLET_updateAll();
    level1_frame++;
}

// ========================================================
// EXPLOSIONS
// ========================================================

void LEVEL1_disposeExplosions() {
    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].sprite != NULL) {
            SPR_releaseSprite(explosions[i].sprite);
            explosions[i].sprite = NULL;
        }
    }
}

void LEVEL1_initExplosions() {
    PAL_setPalette(ENEMY_PALLETE, enemy_0003.palette->data, DMA);

    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        Explosion* e = &explosions[i];

        e->frameIndex = -(random() % EXPLOSION_COOLDOWN);
        e->x = random() % EXPLOSION_MAX_X;
        e->y = random() % EXPLOSION_MAX_Y;

        e->sprite = SPR_addSprite(
            &stage1_explosions,
            e->x,
            e->y,
            TILE_ATTR(ENEMY_PALLETE, FALSE, FALSE, FALSE)
        );

        SPR_setVisibility(e->sprite, HIDDEN);
    }
}

void LEVEL1_updateExplosions() {
    for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
        Explosion* e = &explosions[i];

        if (e->sprite == NULL) continue;

        // -------- COOLDOWN --------
        if (e->frameIndex < 0) {
            e->frameIndex++;
            continue;
        }

        // -------- SPAWN --------
        if (e->frameIndex == 0) {
            e->x = random() % EXPLOSION_MAX_X;
            e->y = random() % EXPLOSION_MAX_Y;

            SPR_setPosition(e->sprite, e->x, e->y);
            SPR_setHFlip(e->sprite, e->x % 2);
            SPR_setVFlip(e->sprite, e->y % 2);
            SPR_setVisibility(e->sprite, VISIBLE);            

            e->currentFrame = 0;
            e->totalFrames  = EXPLOSION_FRAME_COUNT; // número de frames na sprite sheet
            SPR_setFrame(e->sprite, 0);
            SPR_setAlwaysAtBottom(e->sprite);

            e->frameIndex = 1;
            continue;
        }

        // -------- ANIMAÇÃO MANUAL --------
        e->frameIndex++;
        e->currentFrame++;

        if (e->currentFrame >= e->totalFrames)
            e->currentFrame = 0; // loop, ou troca por hide se não quiser loop

        SPR_setFrame(e->sprite, e->currentFrame);
        SPR_setAlwaysAtBottom(e->sprite);

        // -------- FIM DA DURAÇÃO --------
        if (e->frameIndex >= EXPLOSION_ANIMATION_FRAMES) {
            SPR_setVisibility(e->sprite, HIDDEN);
            e->frameIndex = -(random() % (EXPLOSION_COOLDOWN << 1));
            if (e->frameIndex == 0) e->frameIndex = -1;
        }
    }
}

// ========================================================
// LASERS
// ========================================================

static inline __attribute__((always_inline))
void LEVEL1_createColoredTile(u8 angle, u8 colorIndex, u32* output) {
    for (u8 row = 0; row < 8; row++) {
        u32 line = laserTiles[angle][row];
        u32 colored = 0;

        // Percorre cada nibble (4 bits) da linha
        for (s8 nibble = 7; nibble >= 0; nibble--) {
            u8 value = (line >> (nibble << 2)) & 0xF;

            // Se o valor for 5 (nossa máscara), substitui pela cor escolhida
            // Senão mantém o valor original (0 para transparente)
            if (value == 5) {
                colored |= ((u32)colorIndex << (nibble << 2));
            } else {
                colored |= ((u32)value << (nibble << 2));
            }
        }

        output[row] = colored;
    }
}

void LEVEL1_initLasers() {
    // Inicializa os lasers
    for (u8 i = 0; i < MAX_LASERS; i++) {
        lasers[i].timer        = -(random() % LASER_COOLDOWN);
        lasers[i].length       = 0;
        lasers[i].angle        = 0;
        lasers[i].colorIndex   = 5;  // default
        lasers[i].tileVramIndex = LASER_TILE_INDEX + (i << 2);  // Cada laser tem 4 tiles (um por ângulo)
    }
}

void LEVEL1_updateLasers() {
    for (u8 i = 0; i < MAX_LASERS; i++) {
        Laser* l = &lasers[i];

        // -------- COOLDOWN --------
        if (l->timer < 0) {
            l->timer++;
            continue;
        }

        // -------- SPAWN --------
        if (l->timer == 0) {
            // Posição inicial aleatória em tiles
            l->tileX  = random() % 40;  // 320/8 = 40 tiles
            l->tileY  = random() % 28;  // 224/8 = 28 tiles

            // Comprimento aleatório (1-6 tiles)
            l->length = 1 + (random() % LASER_MAX_LENGTH);

            // Ângulo aleatório (0-3)
            l->angle  = random() % 4;

            // Cor aleatória (2, 5 ou 8)
            l->colorIndex = laserColors[random() % 3];

            // Carrega o tile com a cor escolhida na VRAM (índice único para este laser)
            u32 coloredTile[8];
            LEVEL1_createColoredTile(l->angle, l->colorIndex, coloredTile);
            VDP_loadTileData(coloredTile, l->tileVramIndex + l->angle, 1, DMA);

            l->timer = 1;
        }

        // -------- DESENHAR LASER --------
        if (l->timer > 0 && l->timer <= LASER_DURATION) {
            u16 tileAttr = TILE_ATTR_FULL(ENEMY_PALLETE, 0, FALSE, FALSE, l->tileVramIndex + l->angle);

            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX;
                s16 y = l->tileY;

                // Calcula posição baseado no ângulo
                switch(l->angle) {
                    case 0: x += j; break;          // Horizontal
                    case 1: y += j; break;          // Vertical
                    case 2: x += j; y += j; break;  // Diagonal (backslash)
                    case 3: x += j; y -= j; break;  // Diagonal (forward slash)
                }

                // Verifica limites da tela
                if (x >= 0 && x < 40 && y >= 0 && y < 28) {
                    VDP_setTileMapXY(VDP_BG_B, tileAttr, x, y);
                }
            }

            l->timer++;
        }

        // -------- FIM DA DURAÇÃO --------
        if (l->timer > LASER_DURATION) {
            // Limpa os tiles do laser (tile 0 = transparente/vazio)
            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX;
                s16 y = l->tileY;

                switch(l->angle) {
                    case 0: x += j; break;
                    case 1: y += j; break;
                    case 2: x += j; y += j; break;
                    case 3: x += j; y -= j; break;
                }

                if (x >= 0 && x < 40 && y >= 0 && y < 28) {
                    VDP_setTileMapXY(VDP_BG_B, TILE_ATTR_FULL(ENEMY_PALLETE, 0, FALSE, FALSE, 0), x, y);                    
                }
            }

            // Volta para cooldown aleatório
            l->timer = -(random() % (LASER_COOLDOWN * 2));
            if (l->timer == 0) l->timer = -1;
        }
    }
}

void LEVEL1_disposeLasers() {
    // Limpa todos os lasers ativos da tela
    for (u8 i = 0; i < MAX_LASERS; i++) {
        Laser* l = &lasers[i];
        if (l->timer > 0 && l->timer <= LASER_DURATION) {
            for (u8 j = 0; j < l->length; j++) {
                s16 x = l->tileX;
                s16 y = l->tileY;

                switch(l->angle) {
                    case 0: x += j; break;
                    case 1: y += j; break;
                    case 2: x += j; y += j; break;
                    case 3: x += j; y -= j; break;
                }

                if (x >= 0 && x < 40 && y >= 0 && y < 28) {
                    VDP_setTileMapXY(VDP_BG_B, TILE_ATTR_FULL(ENEMY_PALLETE, 0, FALSE, FALSE, 0), x, y);
                }
            }
        }
    }
}

// ========================================================
// SCRIPT: SLOTS DE INIMIGOS
// ========================================================

static void LEVEL1_resetEnemySlots(void) {
    for (u8 i = 0; i < LEVEL1_ENEMY_SLOTS; i++) {
        level1_slots[i].enemy         = NULL;
        level1_slots[i].shootInterval = 0;
        level1_slots[i].shootTimer    = 0;
        level1_slots[i].shootSpeed    = 0;
    }
    level1_script_index = 0;
}

static void LEVEL1_processScript(void) {
    while (level1_script_index < level1_script_len
           && level1_script_table[level1_script_index].frame == level1_frame)
    {
        const Level1ScriptItem* item = &level1_script_table[level1_script_index++];
        if (item->slot >= LEVEL1_ENEMY_SLOTS) continue;

        Level1EnemySlot* slot = &level1_slots[item->slot];

        switch (item->action) {
            case LEVEL1_ACTION_SPAWN:
                if (slot->enemy && slot->enemy->active)
                    ENEMY_deactivate(slot->enemy);
                slot->enemy           = ENEMYFACTORY_createEnemy(item->type, item->x, item->y);
                slot->shootInterval   = 0;
                slot->shootTimer      = 0;
                slot->shootSpeed      = 0;
                break;

            case LEVEL1_ACTION_SET_SHOOT_RATE:
                slot->shootInterval   = item->shootInterval;
                slot->shootTimer      = 0;
                slot->shootSpeed      = item->shootSpeed;
                break;

            case LEVEL1_ACTION_SHOOT_ONCE:
                if (slot->enemy && slot->enemy->active)
                    BULLET_enemyShoot_slasherDirection(slot->enemy, &player, item->shootSpeed);
                break;

            case LEVEL1_ACTION_DEACTIVATE:
                if (slot->enemy && slot->enemy->active)
                    ENEMY_deactivate(slot->enemy);
                slot->enemy           = NULL;
                slot->shootInterval   = 0;
                slot->shootTimer      = 0;
                slot->shootSpeed      = 0;
                break;

            case LEVEL1_ACTION_STOP_SHOOT:
                slot->shootInterval   = 0;
                slot->shootTimer      = 0;
                break;

            default:
                break;
        }
    }
}

static void LEVEL1_updateEnemyShooting(void) {
    for (u8 i = 0; i < LEVEL1_ENEMY_SLOTS; i++) {
        Level1EnemySlot* slot = &level1_slots[i];

        if (!slot->enemy || !slot->enemy->active) {
            slot->enemy = NULL;
            continue;
        }

        if (slot->shootInterval == 0) continue;

        if (slot->shootTimer == 0) {
            BULLET_enemyShoot_slasherDirection(slot->enemy, &player, slot->shootSpeed);
            slot->shootTimer = slot->shootInterval;
        } else {
            slot->shootTimer--;
        }
    }
}

// ========================================================
// SCRIPT PRINCIPAL
// ========================================================

void level1_script() {
    if (level1_frame == 1)   HUD_showStage(1);
    if (level1_frame == 100) HUD_dismissStage();

    if (level1_frame == WARP_DURATION) {
        LEVEL1_initExplosions();
        LEVEL1_initLasers();
    }

    // Processa eventos do script e tiro automático por slot
    LEVEL1_processScript();
    LEVEL1_updateEnemyShooting();

    if ((level1_frame & 3) == 0 && level1_frame > WARP_DURATION) {        
        LEVEL1_updateExplosions();
        LEVEL1_updateLasers();
        ENEMY_update();
    }

}

// u16 testCounter = 0;
// static const u16 cores_piscando[] = {
//     RGB24_TO_VDPCOLOR(0x009100), // Cor 1
//     RGB24_TO_VDPCOLOR(0x006800), // Cor 2
//     RGB24_TO_VDPCOLOR(0x00FF00), // Cor 3 (exemplo: verde claro)
//     RGB24_TO_VDPCOLOR(0x003300)  // Cor 4 (exemplo: verde escuro)
// };

// void TEST_doTests() {
//     VDP_setPlaneSize(64, 64, FALSE);
//
//     // Imagem original (esquerda) - carrega tiles na VRAM
//     VDP_drawImage(
//         VDP_BG_B,
//         &enemy_0008_background,
//         0, 0
//     );
//
//     PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
//     VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
//     Entity_add(NULL, TEST_update);
// }

// void TEST_update(void *ctx) {
//     testCounter++;
//     if (testCounter % 5 == 0) {
//     u16 indice = (testCounter >> 3) & 3;
//
//     PAL_setColor(2, cores_piscando[indice]);
//     }

//     // (testCounter >> 3) controla a velocidade (mais alto = mais lento)
//     // & 3 garante que o índice fique sempre entre 0 e 3

// }
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
#include "background_explosions.h"
#include "background_lasers.h"

// ========================================================
// SCRIPT DE INIMIGOS
// ========================================================

#define LEVEL1_ENEMY_SLOTS 20

typedef enum {
    LEVEL1_ACTION_SPAWN,
    LEVEL1_ACTION_SET_SHOOT_RATE,
    LEVEL1_ACTION_SHOOT_ONCE,
    LEVEL1_ACTION_DEACTIVATE,
    LEVEL1_ACTION_STOP_SHOOT,
    LEVEL1_ACTION_SHOOT_SLASHER_DIRECTION
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

static const Level1ScriptItem level1_script_table[] = {
    // --- WAVE 1: Direita ---
    { 500,  LEVEL1_ACTION_SPAWN,      0, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 520,  LEVEL1_ACTION_SET_SHOOT_RATE, 0, 0,            0,              0,   100, 5 },
    { 564,  LEVEL1_ACTION_SPAWN,      1, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 584,  LEVEL1_ACTION_SHOOT_SLASHER_DIRECTION, 1,    0,    0,            0,   0, 5 },
    { 628,  LEVEL1_ACTION_SPAWN,      2, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 648,  LEVEL1_ACTION_SHOOT_ONCE, 2, 0,            0,                    0,   0, 5 },
    { 692,  LEVEL1_ACTION_SPAWN,      3, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 712,  LEVEL1_ACTION_SHOOT_ONCE, 3, 0,            0,                    0,   0, 5 },
    { 756,  LEVEL1_ACTION_SPAWN,      4, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 776,  LEVEL1_ACTION_SHOOT_ONCE, 4, 0,            0,                    0,   0, 5 },
    { 820,  LEVEL1_ACTION_SPAWN,      5, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 840,  LEVEL1_ACTION_SHOOT_ONCE, 5, 0,            0,                    0,   0, 5 },
    { 884,  LEVEL1_ACTION_SPAWN,      6, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 904,  LEVEL1_ACTION_SHOOT_ONCE, 6, 0,            0,                    0,   0, 5 },
    { 948,  LEVEL1_ACTION_SPAWN,      7, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 968,  LEVEL1_ACTION_SHOOT_ONCE, 7, 0,            0,                    0,   0, 5 },
    { 1012, LEVEL1_ACTION_SPAWN,      8, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 1032, LEVEL1_ACTION_SHOOT_ONCE, 8, 0,            0,                    0,   0, 5 },
    { 1076, LEVEL1_ACTION_SPAWN,      9, ENEMY_TYPE_3, GAME_WINDOW_WIDTH-48, -16, 0, 0 },
    { 1096, LEVEL1_ACTION_SHOOT_ONCE, 9, 0,            0,                    0,   0, 5 },

    { 1100, LEVEL1_ACTION_STOP_SHOOT, 0, 0, 0, 0, 0, 0 },

    // --- WAVE 2: Esquerda ---
    { 1176, LEVEL1_ACTION_SPAWN,      10, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1196, LEVEL1_ACTION_SHOOT_ONCE, 10, 0,            0,  0,   0, 5 },
    { 1240, LEVEL1_ACTION_SPAWN,      11, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1260, LEVEL1_ACTION_SHOOT_ONCE, 11, 0,            0,  0,   0, 5 },
    { 1304, LEVEL1_ACTION_SPAWN,      12, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1324, LEVEL1_ACTION_SHOOT_ONCE, 12, 0,            0,  0,   0, 5 },
    { 1368, LEVEL1_ACTION_SPAWN,      13, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1388, LEVEL1_ACTION_SHOOT_ONCE, 13, 0,            0,  0,   0, 5 },
    { 1432, LEVEL1_ACTION_SPAWN,      14, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1452, LEVEL1_ACTION_SHOOT_ONCE, 14, 0,            0,  0,   0, 5 },
    { 1496, LEVEL1_ACTION_SPAWN,      15, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1516, LEVEL1_ACTION_SHOOT_ONCE, 15, 0,            0,  0,   0, 5 },
    { 1560, LEVEL1_ACTION_SPAWN,      16, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1580, LEVEL1_ACTION_SHOOT_ONCE, 16, 0,            0,  0,   0, 5 },
    { 1624, LEVEL1_ACTION_SPAWN,      17, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1644, LEVEL1_ACTION_SHOOT_ONCE, 17, 0,            0,  0,   0, 5 },
    { 1688, LEVEL1_ACTION_SPAWN,      18, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1708, LEVEL1_ACTION_SHOOT_ONCE, 18, 0,            0,  0,   0, 5 },
    { 1752, LEVEL1_ACTION_SPAWN,      19, ENEMY_TYPE_3, 48, -16, 0, 0 },
    { 1772, LEVEL1_ACTION_SHOOT_ONCE, 19, 0,            0,  0,   0, 5 },

    { 2500, LEVEL1_ACTION_STOP_SHOOT, 0, 0, 0, 0, 0, 0 }
};

static const int level1_script_len = sizeof(level1_script_table) / sizeof(level1_script_table[0]);

// ========================================================
// STATE & GLOBALS
// ========================================================

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
    BACKGROUND_LASERS_dispose();
    BACKGROUND_EXPLOSIONS_dispose();
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
    level1_frame++;
}

// ========================================================
// SCRIPT CORE FUNCTIONS
// ========================================================

static void LEVEL1_resetEnemySlots(void) {
    for (u8 i = 0; i < LEVEL1_ENEMY_SLOTS; i++) {
        level1_slots[i].enemy = NULL;
        level1_slots[i].shootInterval = 0;
        level1_slots[i].shootTimer = 0;
    }
    level1_script_index = 0;
}

static void LEVEL1_processScript(void) {
    while (level1_script_index < level1_script_len && level1_script_table[level1_script_index].frame == level1_frame) {
        const Level1ScriptItem* item = &level1_script_table[level1_script_index++];
        if (item->slot >= LEVEL1_ENEMY_SLOTS) continue;
        Level1EnemySlot* slot = &level1_slots[item->slot];

        switch (item->action) {
            case LEVEL1_ACTION_SPAWN:
                if (slot->enemy && slot->enemy->active) ENEMY_deactivate(slot->enemy);
                slot->enemy = ENEMYFACTORY_createEnemy(item->type, item->x, item->y);
                slot->shootInterval = 0;
                slot->shootTimer = 0;
                slot->shootSpeed = 0;
                break;

            case LEVEL1_ACTION_SET_SHOOT_RATE:
                slot->shootInterval = item->shootInterval;
                slot->shootTimer = 0;
                slot->shootSpeed = item->shootSpeed;
                break;

            case LEVEL1_ACTION_SHOOT_ONCE:
                if (slot->enemy && slot->enemy->active)
                    BULLET_enemyShoot(slot->enemy->bulletSprite, slot->enemy->x, slot->enemy->y, 0, 3);
                break;

            case LEVEL1_ACTION_SHOOT_SLASHER_DIRECTION:
                if (slot->enemy && slot->enemy->active)
                    BULLET_enemyShoot_slasherDirection(slot->enemy, &player, item->shootSpeed);
                break;

            case LEVEL1_ACTION_DEACTIVATE:
                if (slot->enemy && slot->enemy->active) ENEMY_deactivate(slot->enemy);
                slot->enemy = NULL;
                break;

            case LEVEL1_ACTION_STOP_SHOOT:
                slot->shootInterval = 0;
                break;
        }
    }
}

static void LEVEL1_updateEnemyShooting(void) {
    for (u8 i = 0; i < LEVEL1_ENEMY_SLOTS; i++) {
        Level1EnemySlot* slot = &level1_slots[i];
        if (!slot->enemy || !slot->enemy->active) continue;
        if (slot->shootInterval == 0) continue;

        if (slot->shootTimer == 0) {
            //BULLET_enemyShoot(slot->enemy->bulletSprite, slot->enemy->x, slot->enemy->y, 0, slot->shootSpeed);
            BULLET_enemyShoot_slasherDirection(slot->enemy, &player, slot->shootSpeed);
            slot->shootTimer = slot->shootInterval;
        } else {
            slot->shootTimer--;
        }
    }
}

// ========================================================
// MAIN SCRIPT LOOP
// ========================================================

void level1_script() {
    if (level1_frame == 1)   HUD_showStage(1);
    if (level1_frame == 100) HUD_dismissStage();

    if (level1_frame == WARP_DURATION + 20) {
        BACKGROUND_EXPLOSIONS_init();
        BACKGROUND_LASERS_init();
    }

    LEVEL1_processScript();    

    // --- LOAD BALANCER (Distribuição de Carga) ---
    if (level1_frame > WARP_DURATION + 20) {
        BULLET_updateAll();
        COLLISION_checkAllCollisions();        
        LEVEL1_updateEnemyShooting();

        u16 phase = level1_frame % 4; 

        switch (phase) {
            case 0:
                ENEMY_update(); 
                break;

            case 1:
                BACKGROUND_EXPLOSIONS_update();
                break;

            case 2:
                BACKGROUND_LASERS_update();
                break;

            case 3:
                // Janela livre para evitar picos de CPU                
                break;
        }
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
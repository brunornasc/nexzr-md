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
#include "game_script.h"

// ========================================================
// CONFIGURAÇÕES DA FASE
// ========================================================

#define LEVEL1_ENEMY_SLOTS 20
#define ENEMY3_SHOOT_INTERVAL 80
/** ~2 s a 60 Hz (NTSC); em PAL ~2,4 s — ajusta se quiseres tempo exacto por região */
#define LEVEL1_DEATH_RESTART_DELAY_FRAMES 30

static int l1_script_index = 0;
static EnemySlot l1_slots[LEVEL1_ENEMY_SLOTS];
static bool level1_restart_pending;
static u16 level1_death_wait_frames;

static const ScriptItem level1_script_table[] = {
    
};

static const ScriptItem level1_script_table_[] = {
    // --- WAVE 1: Direita ---
    // FRAME, ACTION,                               SLOT,           ENEMY_TYPE,             X,                              Y,              SHOOT_INTERVAL,         SHOOT_SPEED
    { 500,  ACTION_SPAWN,                           0,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },    
    { 520,  ACTION_SET_SHOOT_RATE,                  0,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 521,  ACTION_SET_LINEAR_MOVEMENT,             0,              0,                      0,                              3,              0,                      0 },

    { 564,  ACTION_SPAWN,                           1,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 584,  ACTION_SET_LINEAR_MOVEMENT,             1,              0,                      0,                              3,              0,                      0 },
    { 584,  ACTION_SET_SHOOT_RATE,                  1,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 628,  ACTION_SPAWN,                           2,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 629,  ACTION_SET_LINEAR_MOVEMENT,             2,              0,                      0,                              3,              0,                      0 },
    { 648,  ACTION_SET_SHOOT_RATE,                  2,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 692,  ACTION_SPAWN,                           3,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 693,  ACTION_SET_LINEAR_MOVEMENT,             3,              0,                      0,                              3,              0,                      0 },
    { 712,  ACTION_SET_SHOOT_RATE,                  3,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 756,  ACTION_SPAWN,                           4,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 757,  ACTION_SET_LINEAR_MOVEMENT,             4,              0,                      0,                              3,              0,                      0 },
    { 776,  ACTION_SET_SHOOT_RATE,                  4,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 820,  ACTION_SPAWN,                           5,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 821,  ACTION_SET_LINEAR_MOVEMENT,             5,              0,                      0,                              3,              0,                      0 },
    { 840,  ACTION_SET_SHOOT_RATE,                  5,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 884,  ACTION_SPAWN,                           6,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 885,  ACTION_SET_LINEAR_MOVEMENT,             6,              0,                      0,                              3,              0,                      0 },
    { 904,  ACTION_SET_SHOOT_RATE,                  6,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 948,  ACTION_SPAWN,                           7,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 949,  ACTION_SET_LINEAR_MOVEMENT,             7,              0,                      0,                              3,              0,                      0 },
    { 968,  ACTION_SET_SHOOT_RATE,                  7,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1012, ACTION_SPAWN,                           8,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 1013, ACTION_SET_LINEAR_MOVEMENT,             8,              0,                      0,                              3,              0,                      0 },
    { 1032, ACTION_SET_SHOOT_RATE,                  8,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1076, ACTION_SPAWN,                           9,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH-48,         -16,              0,                      0 },
    { 1077, ACTION_SET_LINEAR_MOVEMENT,             9,              0,                      0,                              3,              0,                      0 },
    { 1096, ACTION_SET_SHOOT_RATE,                  9,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
            
    { 1100, ACTION_STOP_SHOOT,                      0,              0,                      0,                              0,              0,                      0 },
            
    // --- WAVE 2: Esquerda ---         
    { 1176, ACTION_SPAWN,                           0,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1177, ACTION_SET_LINEAR_MOVEMENT,             0,              0,                      0,                              3,              0,                      0 },
    { 1196, ACTION_SET_SHOOT_RATE,                  0,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    

    { 1240, ACTION_SPAWN,                           1,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1241, ACTION_SET_LINEAR_MOVEMENT,             1,              0,                      0,                              3,              0,                      0 },
    { 1260, ACTION_SET_SHOOT_RATE,                  1,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1304, ACTION_SPAWN,                           2,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1305, ACTION_SET_LINEAR_MOVEMENT,             2,              0,                      0,                              3,              0,                      0 },
    { 1324, ACTION_SET_SHOOT_RATE,                  2,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },    

    { 1368, ACTION_SPAWN,                           3,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1369, ACTION_SET_LINEAR_MOVEMENT,             3,              0,                      0,                              3,              0,                      0 },
    { 1388, ACTION_SET_SHOOT_RATE,                  3,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },

    { 1432, ACTION_SPAWN,                           4,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1433, ACTION_SET_LINEAR_MOVEMENT,             4,              0,                      0,                              3,              0,                      0 },
    { 1452, ACTION_SET_SHOOT_RATE,                  4,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    

    { 1496, ACTION_SPAWN,                           5,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1497, ACTION_SET_LINEAR_MOVEMENT,             5,              0,                      0,                              3,              0,                      0 },
    { 1516, ACTION_SET_SHOOT_RATE,                  5,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1560, ACTION_SPAWN,                           6,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1561, ACTION_SET_LINEAR_MOVEMENT,             6,              0,                      0,                              3,              0,                      0 },
    { 1580, ACTION_SET_SHOOT_RATE,                  6,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1624, ACTION_SPAWN,                           7,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1625, ACTION_SET_LINEAR_MOVEMENT,             7,              0,                      0,                              3,              0,                      0 },
    { 1644, ACTION_SET_SHOOT_RATE,                  7,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1686, ACTION_SPAWN,                           8,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1687, ACTION_SET_LINEAR_MOVEMENT,             8,              0,                      0,                              3,              0,                      0 },
    { 1726, ACTION_SET_SHOOT_RATE,                  8,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 1746, ACTION_SPAWN,                           9,              ENEMY_TYPE_1,          48,                            -16,              0,                      0 },
    { 1747, ACTION_SET_LINEAR_MOVEMENT,             9,              0,                      0,                              3,              0,                      0 },
    { 1780, ACTION_SET_SHOOT_RATE,                  9,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    
    { 2000, ACTION_STOP_SHOOT,                      0,              0,                      0,                              0,              0,                      0 },
                     
    // WAVE 3           
    { 2500,  ACTION_SPAWN,                          0,              ENEMY_TYPE_9,           40,                           -20,              0,                      0 },
    { 2500,  ACTION_SET_MOVE_TO_PLAYER,             0,              0,                      0,                              0,              0,                      2 },

};

static const int level1_script_len = sizeof(level1_script_table) / sizeof(level1_script_table[0]);

// ========================================================
// STATE & GLOBALS
// ========================================================

Entity* level1Entity;
unsigned long level1_frame;

// ========================================================
// FORWARD DECLARATIONS
// ========================================================

void level1_joyEventHandler(u16 joy, u16 changed, u16 state);
void level1_update(void* context);
void level1_dispose();
void level1_script();

// ========================================================
// INIT / DISPOSE
// ========================================================

void Level1_init() {
    currentLevel = LEVEL_1;
    Background_init();
    HUD_init();
    ENEMY_initializeAll();
    VDP_setHilightShadow(1); 
    
    // Inicializa o Motor de Script
    SCRIPT_init(l1_slots, LEVEL1_ENEMY_SLOTS);
    l1_script_index = 0;
    level1_restart_pending = false;
    level1_death_wait_frames = 0;

    level1Entity = Entity_add(NULL, level1_update);

    XGM_startPlay(track1);
    PLAYER_init(&player);
    Game_setJoyHandler(level1_joyEventHandler);
    level1_frame = 0;
}

void level1_dispose() {
    level1_restart_pending = false;
    level1_death_wait_frames = 0;

    // Para a música
    XGM_stopPlay();

    // Remove o level antes de outras entidades — senão swaps no array invalidam level1Entity->index
    if (level1Entity) {
        Entity_removeEntity(level1Entity->index);
        level1Entity = NULL;
    }

    PLAYER_dispose(&player);
    
    // Desativa todos os inimigos
    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            ENEMY_deactivate(&enemies[i]);
        }
    }
    
    // Reseta o pool de balas (limpa todas as balas ativas)
    BULLET_setup_pool();
    
    // Reseta o Motor de Script (limpa os slots de inimigos)
    SCRIPT_init(l1_slots, LEVEL1_ENEMY_SLOTS);
    
    // Limpa as variáveis estáticas de estado do level
    l1_script_index = 0;
    level1_frame = 0;
    
    // Limpa o HUD
    HUD_clear();
    
    // Limpa o background (stars e estado)
    Background_dispose();
    
    // Limpa os sub-sistemas de efeitos
    BACKGROUND_LASERS_dispose();
    BACKGROUND_EXPLOSIONS_dispose();
    
    // Remove o handler de entrada
    Game_setJoyHandler(NULL);
    
    // Limpa todas as entidades restantes (garante estado limpo)
    Entity_clearAll();
}

void LEVEL1_restart() {
    level1_dispose();
    Level1_init();
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

    if (level1_restart_pending) {
        level1_restart_pending = false;
        LEVEL1_restart();
        return;
    }

    level1_script();
    level1_frame++;
}

// ========================================================
// MAIN SCRIPT LOOP
// ========================================================

void level1_script() {
    if (player.destroying) {
        if (level1_death_wait_frames < LEVEL1_DEATH_RESTART_DELAY_FRAMES) {
            level1_death_wait_frames++;
            return;
        }
        level1_restart_pending = true;
        return;
    }
    level1_death_wait_frames = 0;

    if (level1_frame == 1)   HUD_showStage(1);
    if (level1_frame == 100) HUD_dismissStage();

    if (level1_frame == WARP_DURATION + 20) {
        BACKGROUND_EXPLOSIONS_init();
        BACKGROUND_LASERS_init();
    }

    // Processa o script da fase usando o Motor modular
    SCRIPT_process(l1_slots, LEVEL1_ENEMY_SLOTS, level1_script_table, level1_script_len, level1_frame, &l1_script_index);

    // --- LOAD BALANCER (Distribuição de Carga) ---
    if (level1_frame > WARP_DURATION + 20) {
        BULLET_updateAll();
        COLLISION_checkAllCollisions();        
        
        // Update de tiro movido para o Motor modular
        SCRIPT_updateShooting(l1_slots, LEVEL1_ENEMY_SLOTS);

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


    // test area
    if (level1_frame == 500) {
        //PAL_setPalette(BACKGROUND_PALLETE, enemy_0008_001.palette->data, DMA);
        Enemy teste;
        teste.x = 0;
        teste.y = 0;
        teste.type = ENEMY_TYPE_8;
        teste.accentColors[0] = RGB24_TO_VDPCOLOR(0x009100);
        teste.accentColors[1] = RGB24_TO_VDPCOLOR(0x006800);
        teste.accentColors[2] = RGB24_TO_VDPCOLOR(0x00FF00);
        teste.accentColors[3] = RGB24_TO_VDPCOLOR(0x003300);
        teste.accentColors[4] = RGB24_TO_VDPCOLOR(0x000000);
        teste.paletteAccentIndex = 2;
        teste.useMiscPalette = true;

        ENEMY_create(&teste);
        // PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
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
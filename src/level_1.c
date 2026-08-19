#include "level_1.h"
#include "game_script.h"
#include "background.h"
#include "game.h"
#include "player.h"
#include "i18n.h"
#include "hud.h"
#include "entitymanager.h"
#include "bullet.h"
#include "collision.h"
#include "resources.h"
#include "sounds.h"

#define LEVEL1_ENEMY_SLOTS 20
#define ENEMY3_SHOOT_INTERVAL 100
#define LEVEL1_DEATH_RESTART_DELAY_FRAMES 30

static int l1_script_index = 0;
static EnemySlot l1_slots[LEVEL1_ENEMY_SLOTS];
static bool level1_restart_pending;
static u16 level1_death_wait_frames;

// limitação escrota de 1 acao por frame, mas é o que dá pra fazer por enquanto

static const ScriptItem level1_script_table[] = {
    // { 501,  ACTION_SPAWN,                           0,              ENEMY_TYPE_TELEPORT_SMALL,           50,         50,              0,                      0 },
    { 520,  ACTION_SPAWN,                           0,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-50,         -16,              0,                      0 },
    { 530,  ACTION_SET_SHOOT_RATE,                  0,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 531,  ACTION_SET_LINEAR_MOVEMENT,             0,              0,                      0,                              3,              0,                      0 },
    
    { 600,  ACTION_SPAWN,                           1,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-75,         -16,              0,                      0 },
    { 610,  ACTION_SET_SHOOT_RATE,                  1,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 611,  ACTION_SET_LINEAR_MOVEMENT,             1,              0,                      0,                              3,              0,                      0 },

    { 660,  ACTION_SPAWN,                           2,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-100,         -16,              0,                      0 },
    { 670,  ACTION_SET_SHOOT_RATE,                  2,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 671,  ACTION_SET_LINEAR_MOVEMENT,             2,              0,                      0,                              3,              0,                      0 },

    { 720,  ACTION_SPAWN,                           3,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-125,         -16,              0,                      0 },
    { 730,  ACTION_SET_SHOOT_RATE,                  3,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 731,  ACTION_SET_LINEAR_MOVEMENT,             3,              0,                      0,                              3,              0,                      0 },

    { 780,  ACTION_SPAWN,                           4,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-150,         -16,              0,                     0 },
    { 790,  ACTION_SET_SHOOT_RATE,                  4,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 791,  ACTION_SET_LINEAR_MOVEMENT,             4,              0,                      0,                              3,              0,                      0 },

    { 840,  ACTION_SPAWN,                           5,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-175,         -16,              0,                      0 },
    { 850,  ACTION_SET_SHOOT_RATE,                  5,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 851,  ACTION_SET_LINEAR_MOVEMENT,             5,              0,                      0,                              3,              0,                      0 },
    
    { 900,  ACTION_SPAWN,                           6,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-200,         -16,              0,                      0 },
    { 910,  ACTION_SET_SHOOT_RATE,                  6,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 911,  ACTION_SET_LINEAR_MOVEMENT,             6,              0,                      0,                              3,              0,                      0 },

    { 960,  ACTION_SPAWN,                           7,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-225,         -16,              0,                      0 },
    { 970,  ACTION_SET_SHOOT_RATE,                  7,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 971,  ACTION_SET_LINEAR_MOVEMENT,             7,              0,                      0,                              3,              0,                      0 },

    { 1020,  ACTION_SPAWN,                          8,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-250,         -16,              0,                      0 },
    { 1030,  ACTION_SET_SHOOT_RATE,                  8,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1031,  ACTION_SET_LINEAR_MOVEMENT,             8,              0,                      0,                              3,              0,                      0 },

    { 1080,  ACTION_SPAWN,                           9,              ENEMY_TYPE_3,           GAME_WINDOW_WIDTH-275,         -16,              0,                     0 },
    { 1090,  ACTION_SET_SHOOT_RATE,                  9,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1091,  ACTION_SET_LINEAR_MOVEMENT,             9,              0,                      0,                              3,              0,                      0 },

    //-- segunda onda

    { 1139,  ACTION_STOP_SHOOT,                       0,              0,          0,        0,              0,                      0 },

    { 1140,  ACTION_SPAWN,                           10,              ENEMY_TYPE_3,           25,         -16,              0,                      0 },
    { 1150,  ACTION_SET_SHOOT_RATE,                  10,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1151,  ACTION_SET_LINEAR_MOVEMENT,             10,              0,                      0,                              3,              0,                      0 },

    { 1200,  ACTION_SPAWN,                           11,              ENEMY_TYPE_3,           50,         -16,              0,                      0 },
    { 1210,  ACTION_SET_SHOOT_RATE,                  11,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1211,  ACTION_SET_LINEAR_MOVEMENT,             11,              0,                      0,                              3,              0,                      0 },

    { 1260,  ACTION_SPAWN,                           12,              ENEMY_TYPE_3,           75,         -16,              0,                      0 },
    { 1270,  ACTION_SET_SHOOT_RATE,                  12,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1271,  ACTION_SET_LINEAR_MOVEMENT,             12,              0,                      0,                              3,              0,                      0 },

    { 1320,  ACTION_SPAWN,                           13,              ENEMY_TYPE_3,           100,         -16,              0,                      0 },
    { 1330,  ACTION_SET_SHOOT_RATE,                  13,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1331,  ACTION_SET_LINEAR_MOVEMENT,             13,              0,                      0,                              3,              0,                      0 },

    { 1380,  ACTION_SPAWN,                           14,              ENEMY_TYPE_3,           125,         -16,              0,                      0 },
    { 1390,  ACTION_SET_SHOOT_RATE,                  14,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1391,  ACTION_SET_LINEAR_MOVEMENT,             14,              0,                      0,                              3,              0,                      0 },

    { 1440,  ACTION_SPAWN,                           15,              ENEMY_TYPE_3,           150,         -16,              0,                     0 },
    { 1450,  ACTION_SET_SHOOT_RATE,                  15,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1451,  ACTION_SET_LINEAR_MOVEMENT,             15,              0,                      0,                              3,              0,                      0 },

    { 1500,  ACTION_SPAWN,                           16,              ENEMY_TYPE_3,           175,         -16,              0,                      0 },
    { 1510,  ACTION_SET_SHOOT_RATE,                  16,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1511,  ACTION_SET_LINEAR_MOVEMENT,             16,              0,                      0,                              3,              0,                      0 },
    
    { 1560,  ACTION_SPAWN,                           17,              ENEMY_TYPE_3,           200,         -16,              0,                      0 },
    { 1570,  ACTION_SET_SHOOT_RATE,                  17,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1571,  ACTION_SET_LINEAR_MOVEMENT,             17,              0,                      0,                              3,              0,                      0 },

    { 1620,  ACTION_SPAWN,                           18,              ENEMY_TYPE_3,           225,         -16,              0,                      0 },
    { 1630,  ACTION_SET_SHOOT_RATE,                  18,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1631,  ACTION_SET_LINEAR_MOVEMENT,             18,              0,                      0,                              3,              0,                      0 },

    { 1680,  ACTION_SPAWN,                          19,              ENEMY_TYPE_3,           250,         -16,              0,                      0 },
    { 1690,  ACTION_SET_SHOOT_RATE,                  19,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1691,  ACTION_SET_LINEAR_MOVEMENT,             19,              0,                      0,                              3,              0,                      0 },

    { 1740,  ACTION_SPAWN,                           0,              ENEMY_TYPE_3,          275,         -16,              0,                     0 },
    { 1750,  ACTION_SET_SHOOT_RATE,                  0,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  5 },
    { 1751,  ACTION_SET_LINEAR_MOVEMENT,             0,              0,                      0,                              3,              0,                      0 },


    // 3 onda (no futuro vou colocar x e y randomico :)  )

    { 2489,  ACTION_STOP_SHOOT,                       0,              0,          0,        0,              0,                      0 },

    { 2490,  ACTION_SPAWN,                           0,              ENEMY_TYPE_TELEPORT_SMALL,           50,         50,              0,                      0 },
    { 2500,  ACTION_SPAWN,                           0,              ENEMY_TYPE_1,          50,         50,              0,                     0 },    
    { 2506,  ACTION_SET_LINEAR_MOVEMENT,             0,              0,                      0,                              4,              0,                      0 },

    { 2510,  ACTION_SPAWN,                           1,              ENEMY_TYPE_TELEPORT_SMALL,           150,                           150,              0,                      0 },
    { 2520,  ACTION_SPAWN,                           1,              ENEMY_TYPE_1,                        150,                           150,              0,                      0 },    
    { 2526,  ACTION_SET_LINEAR_MOVEMENT,             1,              0,                                    0,                              4,              0,                      0 },

    { 2527,  ACTION_SET_SHOOT_RATE,                  0,              0,                      0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    { 2570,  ACTION_SPAWN,                           2,              ENEMY_TYPE_TELEPORT_SMALL,           213,                           10,              0,                      0 },
    { 2580,  ACTION_SPAWN,                           2,              ENEMY_TYPE_1,                        213,                           10,              0,                      0 },
    { 2586,  ACTION_SET_LINEAR_MOVEMENT,             2,              0,                                    0,                              3,              0,                      0 },

    { 2587,  ACTION_SET_SHOOT_RATE,                  1,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    { 2600,  ACTION_SPAWN,                           3,              ENEMY_TYPE_TELEPORT_SMALL,           80,                           75,              0,                      0 },
    { 2610,  ACTION_SPAWN,                           3,              ENEMY_TYPE_1,                        80,                           75,              0,                      0 },    
    { 2616,  ACTION_SET_LINEAR_MOVEMENT,             3,              0,                                    0,                              4,              0,                      0 },

    { 2617,  ACTION_SET_SHOOT_RATE,                  2,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    { 2630,  ACTION_SPAWN,                           4,              ENEMY_TYPE_TELEPORT_SMALL,           180,                           175,              0,                      0 },
    { 2640,  ACTION_SPAWN,                           4,              ENEMY_TYPE_1,                        180,                           175,              0,                      0 },    
    { 2646,  ACTION_SET_LINEAR_MOVEMENT,             4,              0,                                    0,                              3,              0,                      0 },

    { 2647,  ACTION_SET_SHOOT_RATE,                  3,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    { 2680,  ACTION_SPAWN,                           5,              ENEMY_TYPE_TELEPORT_SMALL,           10,                           200,              0,                      0 },
    { 2690,  ACTION_SPAWN,                           5,              ENEMY_TYPE_1,                        10,                           200,              0,                      0 },    
    { 2696,  ACTION_SET_LINEAR_MOVEMENT,             5,              0,                                    0,                              3,              0,                      0 },

    { 2697,  ACTION_SET_SHOOT_RATE,                  4,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    { 2720,  ACTION_SET_SHOOT_RATE,                  5,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    // paradinha pra acalmar o coracao
    { 3195,  ACTION_STOP_SHOOT,                       0,              0,          0,        0,              0,                      0 },
    // emboscada

    { 3200,  ACTION_SPAWN,                           0,              ENEMY_TYPE_TELEPORT_SMALL,                               50,                           25,              0,                      0 },
    { 3201,  ACTION_SPAWN,                           1,              ENEMY_TYPE_TELEPORT_SMALL,           GAME_WINDOW_WIDTH - 50,      GAME_WINDOW_HEIGHT - 50,              0,                      0 },
    { 3202,  ACTION_SPAWN,                           2,              ENEMY_TYPE_TELEPORT_SMALL,                               50,      GAME_WINDOW_HEIGHT - 50,              0,                      0 },
    { 3203,  ACTION_SPAWN,                           3,              ENEMY_TYPE_TELEPORT_SMALL,           GAME_WINDOW_WIDTH - 50,                           25,              0,                      0 },

    { 3210,  ACTION_SPAWN,                           0,              ENEMY_TYPE_1,                               50,                           25,              0,                      0 },
    { 3211,  ACTION_SPAWN,                           1,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH - 50,      GAME_WINDOW_HEIGHT - 50,              0,                      0 },
    { 3212,  ACTION_SPAWN,                           2,              ENEMY_TYPE_1,                               50,      GAME_WINDOW_HEIGHT - 50,              0,                      0 },
    { 3213,  ACTION_SPAWN,                           3,              ENEMY_TYPE_1,           GAME_WINDOW_WIDTH - 50,                           25,              0,                      0 },

    { 3214,  ACTION_SET_SHOOT_RATE,                      0,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },
    { 3215,  ACTION_SET_SHOOT_RATE,                      1,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },
    { 3216,  ACTION_SET_SHOOT_RATE,                      2,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },
    { 3217,  ACTION_SET_SHOOT_RATE,                      3,              0,                                    0,                              0,              ENEMY3_SHOOT_INTERVAL,  6 },

    { 3218,  ACTION_SET_LINEAR_MOVEMENT,             0,              0,                                    0,                              5,              0,                      0 },
    { 3219,  ACTION_SET_LINEAR_MOVEMENT,             1,              0,                                    0,                              5,              0,                      0 },
    { 3220,  ACTION_SET_LINEAR_MOVEMENT,             2,              0,                                    0,                              5,              0,                      0 },
    { 3221,  ACTION_SET_LINEAR_MOVEMENT,             3,              0,                                    0,                              5,              0,                      0 },

    // kamikaze time!
    { 3450,  ACTION_STOP_SHOOT,                       0,              0,          0,        0,              0,                      0 },

    { 3500,  ACTION_SPAWN,                           4,              ENEMY_TYPE_4,                               50,                           -16,              0,                      0 },
    { 3501,  ACTION_SET_MOVE_TO_PLAYER,              4,                         0,                                0,                             0,              0,                      3 },

    { 3550,  ACTION_SPAWN,                           5,              ENEMY_TYPE_4,                                0,                           -16,              0,                      0 },
    { 3551,  ACTION_SET_MOVE_TO_PLAYER,              5,                         0,                                0,                             0,              0,                      3 },

    { 3600,  ACTION_SPAWN,                           6,              ENEMY_TYPE_4,                              150,                           -16,              0,                      0 },
    { 3601,  ACTION_SET_MOVE_TO_PLAYER,              6,                         0,                                0,                             0,              0,                      3 },

    { 3650,  ACTION_SPAWN,                           7,              ENEMY_TYPE_4,                              300,                           -16,              0,                      0 },
    { 3651,  ACTION_SET_MOVE_TO_PLAYER,              7,                         0,                                0,                             0,              0,                      3 },

    { 3700,  ACTION_SPAWN,                           8,              ENEMY_TYPE_4,                               75,                           -16,              0,                      0 },
    { 3701,  ACTION_SET_MOVE_TO_PLAYER,              8,                         0,                                0,                             0,              0,                      3 },

    { 3750,  ACTION_SPAWN,                           9,              ENEMY_TYPE_4,                              200,                           -16,              0,                      0 },
    { 3751,  ACTION_SET_MOVE_TO_PLAYER,              9,                         0,                                0,                             0,              0,                      3 },

    { 3800,  ACTION_SPAWN,                           10,             ENEMY_TYPE_4,                              250,                           -16,              0,                      0 },
    { 3801,  ACTION_SET_MOVE_TO_PLAYER,              10,                        0,                                0,                             0,              0,                      3 },

    { 3850,  ACTION_SPAWN,                           0,              ENEMY_TYPE_4,                               10,                           -16,              0,                      0 },
    { 3851,  ACTION_SET_MOVE_TO_PLAYER,              0,                         0,                                0,                             0,              0,                      3 },
    
    { 3900,  ACTION_SPAWN,                           1,              ENEMY_TYPE_4,                               85,                           -16,              0,                      0 },
    { 3901,  ACTION_SET_MOVE_TO_PLAYER,              1,                         0,                                0,                             0,              0,                      3 },
    
    
};

static const int level1_script_len = sizeof(level1_script_table) / sizeof(level1_script_table[0]);

static Entity* level1Entity;
static unsigned long level1_frame;

static void level1_joyEventHandler(u16 joy, u16 changed, u16 state);
static void level1_update(void* context);
static void level1_dispose();
static void level1_script();

void Level1_init() {
    currentLevel = LEVEL_1;
    Background_init();
    HUD_init();
    ENEMY_initializeAll();
    VDP_setHilightShadow(0);

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

static void level1_dispose() {
    level1_restart_pending = false;
    level1_death_wait_frames = 0;

    XGM_stopPlay();

    if (level1Entity) {
        Entity_removeEntity(level1Entity->index);
        level1Entity = NULL;
    }

    PLAYER_dispose(&player);

    for (u8 i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            ENEMY_deactivate(&enemies[i]);
        }
    }

    BULLET_setup_pool();
    SCRIPT_init(l1_slots, LEVEL1_ENEMY_SLOTS);
    l1_script_index = 0;
    level1_frame = 0;

    HUD_clear();
    Background_dispose();
    BACKGROUND_LASERS_dispose();
    BACKGROUND_EXPLOSIONS_dispose();

    Game_setJoyHandler(NULL);
    Entity_clearAll();
}

static void LEVEL1_restart() {
    level1_dispose();
    Level1_init();
}

static void level1_joyEventHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state & BUTTON_START) {
            Game_pause();
            HUD_showPaused();
        }
    }
}

static void level1_update(void* context) {
    if (Game_isPaused()) return;

    if (level1_restart_pending) {
        level1_restart_pending = false;
        LEVEL1_restart();
        return;
    }

    level1_script();
    level1_frame++;
}

static void level1_script() {
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
        // VDP_setHilightShadow(1);
    }

    SCRIPT_process(l1_slots,
         LEVEL1_ENEMY_SLOTS,
         level1_script_table,
         level1_script_len,
         level1_frame,
         &l1_script_index);

    if (level1_frame > WARP_DURATION + 20) {
        BULLET_updateAll();
        COLLISION_checkAllCollisions();

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
                break;
        }
    }

    // if (level1_frame == 500) {
    //     Enemy teste;
    //     teste.x = 0;
    //     teste.y = -120;
    //     teste.type = ENEMY_TYPE_8;
    //     teste.accentColors[0] = RGB24_TO_VDPCOLOR(0x009100);
    //     teste.accentColors[1] = RGB24_TO_VDPCOLOR(0x006800);
    //     teste.accentColors[2] = RGB24_TO_VDPCOLOR(0x00FF00);
    //     teste.accentColors[3] = RGB24_TO_VDPCOLOR(0x003300);
    //     teste.accentColors[4] = RGB24_TO_VDPCOLOR(0x000000);
    //     teste.paletteAccentIndex = 2;
    //     teste.useMiscPalette = true;

    //     ENEMY_create(&teste);        
    // }
}

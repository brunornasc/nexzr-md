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

#define MAX_EXPLOSIONS 3
#define EXPLOSION_ANIMATION_FRAMES 20 //1 % 3 segundo
#define EXPLOSION_COOLDOWN  (30)
#define EXPLOSION_SIZE     16
#define EXPLOSION_MAX_X  (GAME_WINDOW_WIDTH  > EXPLOSION_SIZE ? GAME_WINDOW_WIDTH  - EXPLOSION_SIZE : 1)
#define EXPLOSION_MAX_Y  (GAME_WINDOW_HEIGHT > EXPLOSION_SIZE ? GAME_WINDOW_HEIGHT - EXPLOSION_SIZE : 1)

#define MAX_LASERS 3
#define LASER_DURATION 4
#define LASER_COOLDOWN 15
#define LASER_MAX_LENGTH 6  // em tiles (6*8 = 48px)
#define LASER_TILE_INDEX 1  // Índice base para os tiles dos lasers (usa 1-20: 5 lasers * 4 ângulos)

typedef struct {
  s16 x;
  s16 y;
  Sprite* sprite;
  s8 frameIndex;
} Explosion;

typedef struct {
    s16 tileX, tileY;  // posição em tiles
    u8 length;         // comprimento em tiles (1-6)
    u8 angle;          // 0=horizontal, 1=vertical, 2=diagonal \, 3=diagonal /
    u8 colorIndex;     // índice da cor (2, 5 ou 8)
    u8 tileVramIndex;  // índice único na VRAM para este laser
    s8 timer;          // negativo = cooldown, positivo = visível
} Laser;

Laser lasers[MAX_LASERS];
Explosion explosions[MAX_EXPLOSIONS];
Entity* level1Entity;
unsigned long level1_frame = 0;
Enemy *enemy1;
Enemy *enemy2;
Enemy *enemy3;
Enemy *enemy4;

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


void level1_joyEventHandler(u16 joy, u16 changed, u16 state) ;
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

void Level1_init() {
  currentLevel = LEVEL_1;
  Background_init();
  HUD_init();
  ENEMY_initializeAll();      
  level1Entity = Entity_add(NULL, level1_update);

  XGM_startPlay(track1);  
  PLAYER_init(&player);
  Game_setJoyHandler(level1_joyEventHandler);
}

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state & BUTTON_START) {
            Game_pause();
            HUD_showPaused();
        }
    }
}

void level1_update(void* context) {
  if (Game_isPaused()) return;
  
  level1_script();
  BULLET_updateAll();
  level1_frame++;
}

void level1_dispose() {
  Entity_removeEntity(level1Entity->index);
  LEVEL1_disposeLasers();
  LEVEL1_disposeExplosions(); 
}

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

      // -------- COOLDOWN (Estado Negativo) --------
      if (e->frameIndex < 0) {
          e->frameIndex++;
          continue;
      }

      // -------- SPAWN (Estado Zero) --------
      if (e->frameIndex == 0) {
          e->x = random() % EXPLOSION_MAX_X;
          e->y = random() % EXPLOSION_MAX_Y;

          SPR_setPosition(e->sprite, e->x, e->y);
          
          // Inicia a animação automática do SGDK
          SPR_setAnim(e->sprite, 0);
          SPR_setHFlip(e->sprite, e->x % 2);          
          SPR_setVFlip(e->sprite, e->y % 2); 
          SPR_setVisibility(e->sprite, VISIBLE);
          SPR_setAlwaysAtBottom(e->sprite);

          // Começamos a contar o tempo de vida da explosão
          e->frameIndex = 1; 
          continue;
      }

      if (e->frameIndex > 0) {
          e->frameIndex++;

          // Se o tempo de vida atingir o limite definido
          if (e->frameIndex >= EXPLOSION_ANIMATION_FRAMES) {
              SPR_setVisibility(e->sprite, HIDDEN);
              
              // Reset para cooldown aleatório
              e->frameIndex = -(random() % (EXPLOSION_COOLDOWN << 1));
              if (e->frameIndex == 0) e->frameIndex = -1;
          }
      }
  }
}

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
        lasers[i].timer = -(random() % LASER_COOLDOWN);
        lasers[i].length = 0;
        lasers[i].angle = 0;
        lasers[i].colorIndex = 5;  // default
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
            l->tileX = random() % 40;  // 320/8 = 40 tiles
            l->tileY = random() % 28;  // 224/8 = 28 tiles
            
            // Comprimento aleatório (1-6 tiles)
            l->length = 1 + (random() % LASER_MAX_LENGTH);
            
            // Ângulo aleatório (0-3)
            l->angle = random() % 4;
            
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
                    case 0: // Horizontal
                        x += j;
                        break;
                    case 1: // Vertical
                        y += j;
                        break;
                    case 2: // Diagonal (backslash)
                        x += j;
                        y += j;
                        break;
                    case 3: // Diagonal (forward slash)
                        x += j;
                        y -= j;
                        break;
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
                    // Limpa com tile vazio
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

void level1_script() {
  if (level1_frame == 1) {
    HUD_showStage(1);
  }

  if (level1_frame == 100) {
    HUD_dismissStage();
  }

  if (level1_frame == WARP_DURATION) {
    LEVEL1_initExplosions();
    LEVEL1_initLasers();
  }

   if (level1_frame == 500) {
     enemy1 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, GAME_WINDOW_WIDTH - 80, -8);
   }

    if (level1_frame == 540) {
        enemy2 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, GAME_WINDOW_WIDTH - 80, -8);
    }


    if (level1_frame == 560) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 5);
    }

    if (level1_frame == 580) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy2, &player, 5);

        enemy3 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, GAME_WINDOW_WIDTH - 80, -8);
    }

    if (level1_frame == 600) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy3, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy2, &player, 6);

    }

    if (level1_frame == 620) {
        enemy4 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, GAME_WINDOW_WIDTH - 80, -8);
    }

    if (level1_frame == 640) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 6);
        BULLET_enemyShoot_slasherDirection(enemy3, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy2, &player, 6);
        BULLET_enemyShoot_slasherDirection(enemy4, &player, 5);
    }

    if (level1_frame == 1050) {
        if (enemy1) ENEMY_deactivate(enemy1);
        if (enemy2) ENEMY_deactivate(enemy2);
        if (enemy3) ENEMY_deactivate(enemy3);
        if (enemy4) ENEMY_deactivate(enemy4);

        enemy1 = NULL;
        enemy2 = NULL;
        enemy3 = NULL;
        enemy4 = NULL;
    }

    if (level1_frame == 1100) {
        enemy1 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, 80, -8);
    }

    if (level1_frame == 1140) {
        enemy2 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, 80, -8);
    }


    if (level1_frame == 1180) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 5);
    }

    if (level1_frame == 1220) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy2, &player, 5);

        enemy3 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, 80, -8);
    }

    if (level1_frame == 1260) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy3, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy2, &player, 6);

    }

    if (level1_frame == 1300) {
        enemy4 = ENEMYFACTORY_createEnemy(ENEMY_TYPE_1, 80, -8);
    }

    if (level1_frame == 1340) {
        BULLET_enemyShoot_slasherDirection(enemy1, &player, 6);
        BULLET_enemyShoot_slasherDirection(enemy3, &player, 5);
        BULLET_enemyShoot_slasherDirection(enemy2, &player, 6);
        BULLET_enemyShoot_slasherDirection(enemy4, &player, 5);
    }




/*
    if (level1_frame == 300) {
        // Carrega a imagem original (esquerda)
        VDP_drawImageEx(
            VDP_BG_A,
            &enemy_0008_background,
            TILE_ATTR_FULL(MISC_PALLETE, FALSE, FALSE, FALSE, TILE_USER_INDEX),
            0, 0,
            TRUE,   // Carrega tiles na VRAM
            DMA
        );

        PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
        PAL_setPalette(ENEMY_PALLETE, enemy_0003.palette->data, DMA);
        PAL_setPalette(SLASHER_PALLETE, slasher.palette->data, DMA);
    }
*/
  if ((level1_frame & 3) == 0 && level1_frame > WARP_DURATION) {
    ENEMY_update(); 
    LEVEL1_updateExplosions();
    LEVEL1_updateLasers();
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
    
//     // Imagem original (esquerda) - carrega tiles na VRAM
//     VDP_drawImage(
//         VDP_BG_B,
//         &enemy_0008_background,
//         0, 0
//     );
    
//     PAL_setColor(0, RGB24_TO_VDPCOLOR(0x000000));
//     VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
//     Entity_add(NULL, TEST_update);
// }

// void TEST_update(void *ctx) {
//     testCounter++;
//     if (testCounter % 5 == 0) {
//     u16 indice = (testCounter >> 3) & 3;
    
//     PAL_setColor(2, cores_piscando[indice]);     
//     }

//     // (testCounter >> 3) controla a velocidade (mais alto = mais lento)
//     // & 3 garante que o índice fique sempre entre 0 e 3

// }
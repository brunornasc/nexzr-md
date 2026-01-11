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

#define MAX_EXPLOSIONS 3
#define EXPLOSION_ANIMATION_FRAMES 20 //1 % 3 segundo
#define EXPLOSION_COOLDOWN  (30)
#define EXPLOSION_SIZE     16
#define EXPLOSION_MAX_X  (GAME_WINDOW_WIDTH  > EXPLOSION_SIZE ? GAME_WINDOW_WIDTH  - EXPLOSION_SIZE : 1)
#define EXPLOSION_MAX_Y  (GAME_WINDOW_HEIGHT > EXPLOSION_SIZE ? GAME_WINDOW_HEIGHT - EXPLOSION_SIZE : 1)

typedef struct {
  s16 x;
  s16 y;
  Sprite* sprite;
  s8 frameIndex;
} Explosion;

Explosion explosions[MAX_EXPLOSIONS];
Entity* level1Entity;
unsigned long level1_frame = 0;
Enemy *enemy1;

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) ;
void level1_update(void* context);
void level1_dispose();
void level1_script();
void LEVEL1_initExplosions();
void LEVEL1_updateExplosions();

void Level1_init() {  
  Background_init();
  PLAYER_init(&player);
  Game_setJoyHandler(level1_joyEventHandler);
  Characters_prepareToPrint();

  currentLevel = LEVEL_1;

  HUD_init();
  ENEMY_initializeAll();  
  
  level1Entity = Entity_add(NULL, level1_update);  
  XGM_startPlay(track2);
  
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

  for (u8 i = 0; i < MAX_EXPLOSIONS; i++) {
      if (explosions[i].sprite != NULL) {
          SPR_releaseSprite(explosions[i].sprite);
          explosions[i].sprite = NULL;
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
  }

  if (level1_frame == 1200) {
    Enemy e;
    e.x = 100;
    e.y = 0;
    e.width = 16;
    e.height = 16;
    e.y_speed = 3;
    e.x_speed = 0;
    e.spriteIndex = 0;
    e.type = ENEMY_TYPE_3;
    e.active = true;
    e.inverted = false;
    e.health = 10;
    e.sprite = &enemy_0003;
    e.bulletSprite = &enemy_bullet_001;

    enemy1 = ENEMY_create(&e);
    ENEMY_shoot(enemy1, enemy1->bulletSprite, 0, 4);
  }

  if (level1_frame % 3 == 0 && level1_frame > WARP_DURATION){
    ENEMY_update(); 
    LEVEL1_updateExplosions();
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
              e->frameIndex = -(random() % EXPLOSION_COOLDOWN * 2);
              if (e->frameIndex == 0) e->frameIndex = -1;
          }
      }
  }
}
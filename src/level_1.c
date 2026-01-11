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

Entity* level1Entity;
unsigned long level1_frame = 0;
Enemy *enemy1;

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) ;
void level1_update(void* context);
void level1_dispose();
void level1_script();

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
}

void level1_script() {
  if (level1_frame == 1) {
    HUD_showStage(1);
  }

  if (level1_frame == 100) {
    HUD_dismissStage();
  }

  if (level1_frame == 120) {
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

  if (level1_frame % 3 == 0)
    ENEMY_update();  
}
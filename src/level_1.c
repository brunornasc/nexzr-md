#include "level_1.h"
#include "background.h"
#include "game.h"
#include "player.h"
#include "i18n.h"
#include "hud.h"
#include "entitymanager.h"
#include "enemies.h"
#include "bullet.h"

Entity* level1Entity;
unsigned long level1_frame = 0;

Enemy enemies[1];

void level1_joyEventHandler(u16 joy, u16 changed, u16 state) ;
void level1_update(void* context);
void level1_dispose();
void level1_enemyTest();

void Level1_init() {
  Background_init();
  PLAYER_init(&player);
  Game_setJoyHandler(level1_joyEventHandler);
  Characters_prepareToPrint();

  currentLevel = LEVEL_1;

  HUD_init();

  level1_enemyTest();
  level1Entity = Entity_add(NULL, level1_update);
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
  enemies[0].y++;
  SPR_setPosition(enemies[0].sprite, enemies[0].x, enemies[0].y);
  Bullet_updateAll();

  level1_frame++;
}

void level1_dispose() {
  Entity_removeEntity(level1Entity->index);
}

void level1_enemyTest() {
  enemies[0].x = 0;
  enemies[0].y = 0;
  enemies[0].width = 16;
  enemies[0].height = 16;
  enemies[0].speed = 1;
  enemies[0].spriteIndex = 0;
  enemies[0].type = ENEMY_TYPE_1;
  enemies[0].active = true;
  enemies[0].inverted = false;

  PAL_setPalette(ENEMY_PALLETE, enemy_0006.palette->data, DMA);
  enemies[0].sprite = SPR_addSprite(&enemy_0006, enemies[0].x, enemies[0].y, TILE_ATTR(ENEMY_PALLETE, FALSE, FALSE, FALSE));

  SPR_setAnim(enemies[0].sprite, 0);
}
#include "player.h"
#include "game.h"
#include "entitymanager.h"
#include "resources.h"
#include "bullet.h"

#define SLASHER_IDLE   0
#define SLASHER_MOVING 1
#define MOVE_HOLD_FRAME1 2
#define MOVE_HOLD_FRAME2 3

#define SLASHER_VELOCITY 2

void constraint_animation(Player* p);

void PLAYER_init(Player* p) {
    p->x = (GAME_WINDOW_WIDTH / 2) - 16;
    p->y = (GAME_WINDOW_HEIGHT) - 64;
    p->moveFrame = 0;
    p->frameCounter = 0;

    p->sprite = SPR_addSprite(&slasher, p->x, p->y, TILE_ATTR(SLASHER_PALLETE, FALSE, FALSE, FALSE));
    SPR_setAnim(p->sprite, SLASHER_IDLE);

    Entity_add(p, PLAYER_handleInput);
}

void PLAYER_handleInput(void* context) {
    if (Game_isPaused()) return;

    Player* p = (Player*) context;
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_RIGHT) {
        if (p->x < (GAME_WINDOW_WIDTH - 32))
            p->x += SLASHER_VELOCITY;

        SPR_setHFlip(p->sprite, FALSE);

        if (p->sprite->animInd != SLASHER_MOVING)
            SPR_setAnim(p->sprite, SLASHER_MOVING);

        constraint_animation(p);
    }
    else if (value & BUTTON_LEFT) {
        if (p->x > 0)
            p->x -= SLASHER_VELOCITY;

        SPR_setHFlip(p->sprite, TRUE);
        if (p->sprite->animInd != SLASHER_MOVING)
            SPR_setAnim(p->sprite, SLASHER_MOVING);

        constraint_animation(p);
    }
    else {
        p->moveFrame = 0;
        SPR_setAnim(p->sprite, SLASHER_IDLE);
    }

    if (value & BUTTON_UP) {
        if (p->y > 0)
           p->y -= SLASHER_VELOCITY;
    }
    if (value & BUTTON_DOWN) {
        if (p->y < (GAME_WINDOW_HEIGHT - 48))
            p->y += SLASHER_VELOCITY;
    }

    if (value & BUTTON_A) {
        BULLET_slasherShoot(p->x + 8, p->y);
    }

    SPR_setPosition(p->sprite, p->x, p->y);
}

void constraint_animation(Player* p) {
    if (++p->frameCounter >= 5) {
        p->frameCounter = 0;
        p->moveFrame++;

        if (p->moveFrame > MOVE_HOLD_FRAME2)
            p->moveFrame = MOVE_HOLD_FRAME1;

        SPR_setFrame(p->sprite, p->moveFrame);
    }
}
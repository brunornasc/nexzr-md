#include "player.h"
#include "entitymanager.h"
#include "resources.h"

#define SLASHER_IDLE   0
#define SLASHER_MOVING 1
#define MOVE_HOLD_FRAME1 2
#define MOVE_HOLD_FRAME2 3

void PLAYER_init(Player* p) {
    p->x = 0;
    p->y = 0;
    p->moveFrame = 0;
    p->frameCounter = 0;

    p->sprite = SPR_addSprite(&slasher, p->x, p->y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
    SPR_setAnim(p->sprite, SLASHER_IDLE);

    Entity_add(p, PLAYER_handleInput);
    Entity_add(p, PLAYER_update);
}

void PLAYER_handleInput(void* context) {
    Player* p = (Player*) context;
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_RIGHT) {
        p->x++;
        SPR_setHFlip(p->sprite, FALSE);
        if (p->sprite->animInd != SLASHER_MOVING)
            SPR_setAnim(p->sprite, SLASHER_MOVING);

        if (++p->frameCounter >= 5) {
            p->frameCounter = 0;
            p->moveFrame++;
            if (p->moveFrame > MOVE_HOLD_FRAME2) p->moveFrame = MOVE_HOLD_FRAME1;
            SPR_setFrame(p->sprite, p->moveFrame);
        }
    }
    else if (value & BUTTON_LEFT) {
        p->x--;
        SPR_setHFlip(p->sprite, TRUE);
        if (p->sprite->animInd != SLASHER_MOVING)
            SPR_setAnim(p->sprite, SLASHER_MOVING);

        if (++p->frameCounter >= 5) {
            p->frameCounter = 0;
            p->moveFrame++;
            if (p->moveFrame > MOVE_HOLD_FRAME2) p->moveFrame = MOVE_HOLD_FRAME1;
            SPR_setFrame(p->sprite, p->moveFrame);
        }
    }
    else {
        p->moveFrame = 0;
        SPR_setAnim(p->sprite, SLASHER_IDLE);
    }

    if (value & BUTTON_UP)    p->y--;
    if (value & BUTTON_DOWN)  p->y++;
}

void PLAYER_update(void* context) {
    Player* p = (Player*) context;
    SPR_setPosition(p->sprite, p->x, p->y);
}

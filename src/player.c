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

typedef struct {
    Sprite* sprite;
    int x, y;
    int moveFrame;
    unsigned long frameCounter;
} Player_animation;

Player_animation player_animation;

void PLAYERinit_animation(Player* p);
void PLAYER_update_init_animation();
void constraint_animation(Player* p);

void PLAYER_init(Player* p) {
    PLAYERinit_animation(p);
    /*
    
    p->x = (GAME_WINDOW_WIDTH / 2) - 16;
    p->y = (GAME_WINDOW_HEIGHT) - 64;
    p->moveFrame = 0;
    p->frameCounter = 0;

    p->sprite = SPR_addSprite(&slasher, p->x, p->y, TILE_ATTR(SLASHER_PALLETE, FALSE, FALSE, FALSE));
    SPR_setAnim(p->sprite, SLASHER_IDLE);

    Entity_add(p, PLAYER_handleInput);
    */
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

void PLAYER_gotHit(u8 damage) {
    // Implement player damage handling here
}

void PLAYER_dispose(Player* p) {
    if (p->sprite) {
        SPR_releaseSprite(p->sprite);
        p->sprite = NULL;
    }

    Entity_removeByContext(p);
}

 void PLAYERinit_animation(Player* p) {

    // carrega slasher_big_0 de fora da tela embaixo a esquerda do centro, ele vai subindo ate quase o fim da tela, começa a diminuir e vai descendo ate

    // p->x = (GAME_WINDOW_WIDTH / 2) - 16;

    // p->y = (GAME_WINDOW_HEIGHT) - 64;
    player_animation.frameCounter = 0;
    player_animation.x = 0;
    player_animation.y = GAME_WINDOW_HEIGHT;
    player_animation.sprite = SPR_addSprite(&slasher_big_0, player_animation.x, player_animation.y, TILE_ATTR(SLASHER_PALLETE, FALSE, FALSE, FALSE));

    Entity_add(p, PLAYER_update_init_animation);
} 
void PLAYER_update_init_animation(void* context) {
    player_animation.frameCounter++;

    const s16 targetFinalX = (GAME_WINDOW_WIDTH / 2) - 16;  // 144
    const s16 targetFinalY = (GAME_WINDOW_HEIGHT) - 64;     // 176

    // --- FASE 1: Subida (BIG_0: 96x148) ---
    if (player_animation.frameCounter < 45) {
        player_animation.y -= 5; 
        player_animation.x += 3;
    }
    // --- FASE 2: Transição BIG_2 (96x152) ---
    else if (player_animation.frameCounter < 60) {
        if (player_animation.frameCounter == 45) {
            SPR_setDefinition(player_animation.sprite, &slasher_big_2);
            player_animation.y += 4; 
        }
        player_animation.y -= 1; 
    }
    // --- FASE 3: Topo (BIG_3: 88x120) ---
    else if (player_animation.frameCounter < 90) {
        if (player_animation.frameCounter == 60) {
            SPR_setDefinition(player_animation.sprite, &slasher_big_3);
            player_animation.x += 4; 
            player_animation.y += 16;
        }
        s16 middleX = (GAME_WINDOW_WIDTH / 2) - 44;
        if (player_animation.x > middleX) player_animation.x -= 2;
        else if (player_animation.x < middleX) player_animation.x += 2;
    }
    // --- FASE 4: Descida Controlada (BIG_4: 80x96) ---
    else if (player_animation.frameCounter < 115) {
        if (player_animation.frameCounter == 90) {
            SPR_setDefinition(player_animation.sprite, &slasher_big_4);
            player_animation.x += 4;
            player_animation.y += 12;
        }
        // Só desce se ainda não passou do alvo final
        if (player_animation.y < targetFinalY) player_animation.y += 2;
    }
    // --- FASE 5: Descida (BIG_5: 48x64) ---
    else if (player_animation.frameCounter < 140) {
        if (player_animation.frameCounter == 115) {
            SPR_setDefinition(player_animation.sprite, &slasher_big_5);
            player_animation.x += 16;
            player_animation.y += 16;
        }
        // Desce com limite
        if (player_animation.y < targetFinalY) player_animation.y += 2;
    }
    // --- FASE 6: Transição Final para SLASHER (32x32) ---
    else if (player_animation.frameCounter < 170) {
        if (player_animation.frameCounter == 140) {
            SPR_setDefinition(player_animation.sprite, &slasher);
            player_animation.x += 8;
            player_animation.y += 16;
        }

        // PERSEGUIÇÃO DO ALVO (Smooth transition)
        // Se estiver muito longe do X final, move rápido, se estiver perto, move devagar
        if (player_animation.x < targetFinalX) player_animation.x++;
        else if (player_animation.x > targetFinalX) player_animation.x--;

        // Se estiver abaixo do alvo, ele sobe (corrige o "indo mto pra baixo")
        // Se estiver acima, ele desce
        if (player_animation.y < targetFinalY) player_animation.y += 1;
        else if (player_animation.y > targetFinalY) player_animation.y -= 1;
    }
    // --- ENCERRAMENTO ---
    else {
        SPR_releaseSprite(player_animation.sprite);
        Entity_removeByContext(context);

        Player* p = (Player*) context;
        // Agora player_animation.y já deve estar em targetFinalY ou muito perto
        p->x = targetFinalX;
        p->y = targetFinalY;
        p->moveFrame = 0;
        p->frameCounter = 0;
        p->sprite = SPR_addSprite(&slasher, p->x, p->y, TILE_ATTR(SLASHER_PALLETE, FALSE, FALSE, FALSE));
        
        SPR_setAnim(p->sprite, SLASHER_IDLE);
        SPR_setAlwaysOnTop(p->sprite);
        Entity_add(p, PLAYER_handleInput);
        return;
    }

    SPR_setPosition(player_animation.sprite, player_animation.x, player_animation.y);
}
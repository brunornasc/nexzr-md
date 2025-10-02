#include "test.h"
#include "game.h"
#include "resources.h"
#include "entitymanager.h"

#define STAR_COUNT 20
#define MAX_STAR_HEIGHT 5
#define WARP_DURATION 180
#define DEACELERATION_FRAMES_ANIM 7

typedef struct {
    Sprite* spr[MAX_STAR_HEIGHT];
    int x, y;
    u8 size;
    u8 speed;
    u8 colorFrame;
    bool done;
    u8 decelCounter;
} Star;

static Star stars[STAR_COUNT];
static u8 warpTimer = 0;
static bool isWarping = true;
static bool isDeacelerating = false;
static u8 deAceleratedStarsCount = 0;
static Entity* backgroundTask;

void update_test(void* context);
void test_joyEventHandler(u16 joy, u16 changed, u16 state);

// inicializa aleatoriamente
void initialize_test() {
    Game_resetScreen();
    currentFrame = 0;

    Characters_prepareToPrint();

    PAL_setPalette(SLASHER_PALLETE, star_warp.palette->data, DMA); // mudar depois

    for (int i = 0; i < STAR_COUNT; i++) {
        u8 size = (random() % MAX_STAR_HEIGHT) + 1;
        u8 speed = 3 + size * 2 + (random() % 2);
        u8 colorFrame = random() % 3;

        int x = random() % 320;
        int y = random() % 224;

        for (int j = 0; j < size; j++) {
            stars[i].spr[j] = SPR_addSprite(&star_warp, x, y + j * 8, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            SPR_setFrame(stars[i].spr[j], colorFrame);
        }

        stars[i].x = x;
        stars[i].y = y;
        stars[i].size = size;
        stars[i].speed = speed;
        stars[i].colorFrame = colorFrame;
        stars[i].done = false;
        stars[i].decelCounter = DEACELERATION_FRAMES_ANIM;
    }

    backgroundTask = Entity_add(NULL, update_test);
    Game_setJoyHandler(test_joyEventHandler);
}

void Background_stop() {
    backgroundTask->active = false;
}

void Background_resume() {
    backgroundTask->active = true;
}

bool Background_isRunning() {
    return backgroundTask->active;
}

void test_joyEventHandler(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state & BUTTON_START) {
            game_paused = !game_paused;

            if (game_paused) {
                Characters_print("PAUSED", 17, 13, FONT_ACTIVE);

            } else {
                VDP_clearPlane(BG_B, TRUE);

            }
        }
    }
}

void update_test(void* context) {
    if (game_paused) return;
    if (currentFrame % 3 != 0 && !isDeacelerating) return; // faço atualização a cada 3 frames melhora performance

    for (int i = 0; i < STAR_COUNT; i++) {
        Star* s = &stars[i];

        // atualiza posição com a velocidade
        s->y += s->speed;
        // se passar do tamanho da tela atualiza y pro topo
        if (s->y > GAME_WINDOW_HEIGHT) {
            s->y = GAME_WINDOW_START_POSITION_TOP;
            s->x = random() % GAME_WINDOW_WIDTH;
        }

        int y = s->y;

        for (int j = 0; j < s->size; j++) {
            if (s->spr[j]) {
                SPR_setPosition(s->spr[j], s->x, y);
            }
            // coloca os sprites um atras do outro
            y -= 8;
        }

        // fazer a animacao diminuindo e diminuir a velocidade e deixar
        if (isDeacelerating) {
            if (s->decelCounter > 0) s->decelCounter--;

            if (s->decelCounter == 0) {
                if (s->size > 1) {
                    SPR_releaseSprite(s->spr[s->size - 1]);
                    s->spr[s->size - 1] = NULL;
                    s->size--;
                    s->speed = 2 + s->size;

                } else if (!s->done) {
                    SPR_setAnimAndFrame(s->spr[0], 1, s->colorFrame);
                    s->speed = (random() % 2) + 1;
                    s->done = true;
                    deAceleratedStarsCount++;
                }

                s->decelCounter = DEACELERATION_FRAMES_ANIM; // reinicia o contador da própria estrela
            }
        }

        // fim da animação
        if (deAceleratedStarsCount >= STAR_COUNT) {
            isDeacelerating = false;
        }
    }

    if (isWarping) {
        warpTimer++;

        if (warpTimer > WARP_DURATION){
            isDeacelerating = true;
            isWarping = false;
        }
    }
}

#include "test.h"
#include "game.h"
#include "resources.h"
#include "entitymanager.h"

#define STAR_COUNT 20
#define MAX_STAR_HEIGHT 4   // número máximo de blocos (linhas) por estrela

typedef struct {
    Sprite* spr[MAX_STAR_HEIGHT];
    int x, y;
    u8 size;       // altura em blocos 1..MAX_STAR_HEIGHT
    u8 speed;
    u8 colorFrame;
} Star;

static Star stars[STAR_COUNT];

void update_test(void* context);

// inicializa aleatoriamente
void initialize_test() {
    Game_resetScreen();
    currentFrame = 0;

    PAL_setPalette(PAL0, star_warp.palette->data, DMA);

    for (int i = 0; i < STAR_COUNT; i++) {
        // tamanho aleatório de 1 a MAX_STAR_HEIGHT
        u8 size = (random() % MAX_STAR_HEIGHT) + 1;

        // velocidade proporcional (maiores = mais rápidos)
        u8 speed = 3 + size * 2 + (random() % 2);

        // cor aleatória (0,1,2 = frames diferentes)
        u8 colorFrame = random() % 3;

        // posição inicial
        int x = random() % 320;
        int y = random() % 224;

        // cria cada parte da estrela (empilhando blocos de 8 px)
        for (int j = 0; j < size; j++) {
            stars[i].spr[j] = SPR_addSprite(&star_warp, x, y + j * 8, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            SPR_setFrame(stars[i].spr[j], colorFrame);
        }

        stars[i].x = x;
        stars[i].y = y;
        stars[i].size = size;
        stars[i].speed = speed;
        stars[i].colorFrame = colorFrame;
    }

    Entity_add(NULL, update_test);
}

// update global
void update_test(void* context) {
    for (int i = 0; i < STAR_COUNT; i++) {
        Star* s = &stars[i];

        s->y += s->speed;

        // se passar da tela, reaparece no topo
        if (s->y > 224) {
            s->y = -8;
            s->x = random() % 320;
            s->size = (random() % MAX_STAR_HEIGHT) + 1;
            s->speed = 3 + s->size * 2 + (random() % 2);
            s->colorFrame = random() % 3;

            // recria os sprites conforme o novo tamanho
            for (int j = 0; j < s->size; j++) {
                if (!s->spr[j]) {
                    s->spr[j] = SPR_addSprite(&star_warp, s->x, s->y + j * 8, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
                }
                SPR_setFrame(s->spr[j], s->colorFrame);
            }
        }

        // atualiza posição
        int y = s->y;
        for (int j = 0; j < s->size; j++) {
            SPR_setPosition(s->spr[j], s->x, y);
            y += 8;
        }
    }
}

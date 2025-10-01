#include "game.h"

//Player player;

int main() {

    Game_init();

//    PAL_setPalette(PAL2, slasher.palette->data, DMA);

//    PLAYER_init(&player);

    while(true) {
        Game_update();
    }

    return 0;
}

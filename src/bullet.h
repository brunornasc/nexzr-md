#ifndef BULLET_H
#define BULLET_H

    #include <genesis.h>

    #define MAX_BULLETS 20

    typedef struct {
        s16 x, y;
        s16 velX, velY;
        bool active;
        Sprite* sprite;
    } Bullet;

    // Inicializa o pool uma única vez no início do jogo
    void BULLET_setup_pool();
    void BULLET_slasherShoot(s16 posX, s16 posY);
    void BULLET_deactivate(u8 array_index);
    void Bullet_updateAll();
#endif
#ifndef UTILS_H
#define UTILS_H

#include <genesis.h>

void UTILS_wait(u8 seconds);
void UTILS_loadAndFadeIn(Image img, u8 seconds);
void UTILS_fadeOut(u8 seconds);
void UTILS_drawImageMirroredH(VDPPlane plane, const Image* img, u16 paletteIndex, 
                         u16 startTileIndex, s16 destX, s16 destY);

#endif
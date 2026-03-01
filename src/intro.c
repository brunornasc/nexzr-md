#include "intro.h"
#include "utils.h"

#include <stdarg.h>
#include "resources.h"

void INTRO_init(void (*function)()) {
    Game_resetScreen();
    INTRO_loadSequentially(&naxat, &intro, &intro2, NULL);

    if (function) function();
}

void INTRO_loadSequentially(const Image *first, ...) {
    va_list args;
    const Image *current = first;

    va_start(args, first);

    while (current != NULL) {
        Game_resetScreen();
        
        UTILS_loadAndFadeIn(*current, 1);
        UTILS_wait(2);
        UTILS_fadeOut(1);

        current = va_arg(args, const Image *);
    }

    va_end(args);
}
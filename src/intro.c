#include "intro.h"
#include "utils.h"

void Intro_init(void (*function)()) {
  Game_resetScreen();

  Utils_loadAndFadeIn(naxat, 1);
  Utils_wait(2);
  Utils_fadeOut(1);

  Utils_loadAndFadeIn(intro, 1);
  Utils_wait(2);
  Utils_fadeOut(1);

  function();
}

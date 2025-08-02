#ifndef TERM_H_
#define TERM_H_

#include <SDL2/SDL.h>

#define MODE_ECHO 1

struct DisplayData {
  char* screen;
  int columns;
  int rows;
  int cursorx;
  int cursory;
  int fgcolor;
  int bgcolor;
  int mode;
};

void perform(struct DisplayData* display, const char* buffer);
void draw(SDL_Renderer* ren, const struct DisplayData* display);
void drawCursor(SDL_Renderer* ren, const struct DisplayData* display);
int getModeFlag(const struct DisplayData* display, int mode);

#endif // TERM_H_

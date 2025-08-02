#include <stdio.h>
#include <unistd.h>
#include <pty.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "parser.h"
#include "term.h"

extern const char* numberSymbols;

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* win = SDL_CreateWindow("SDL Terminal!", 0, 0, 1200, 900, SDL_WINDOW_SHOWN);
  SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  Queue queue = { .head = NULL, .tail = NULL };
  char buffer[10000];

  struct DisplayData display = (struct DisplayData) {
    .screen = malloc(sizeof(char) * 60 * 45),
    .columns = 60,
    .rows = 45,
    .cursorx = 0,
    .cursory = 0,
    .fgcolor = 0,
    .bgcolor = 0
  };

  struct winsize size = { .ws_row = display.rows, .ws_col = display.columns };

  int m;

  int me = forkpty(&m, NULL, NULL, &size);

  if(me == 0) {
    system("/bin/sh");
    exit(0);
  }

  fcntl(m, F_SETFL, O_NONBLOCK);

  for(int i = 0; i < display.columns * display.rows; i++)
    display.screen[i] = '\0';
  const Uint8* keyboard = SDL_GetKeyboardState(NULL);

  SDL_Event e;
  int running = 1;

  while(running && !waitpid(me, NULL, WNOHANG)) {
    usleep(10000);
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT)
        running = 0;

      if(e.type == SDL_KEYDOWN) {
        int pressed = e.key.keysym.scancode;

        if(pressed >= SDL_SCANCODE_A && pressed <= SDL_SCANCODE_Z){
          int offset = pressed - SDL_SCANCODE_A;

          char out = offset + (keyboard[SDL_SCANCODE_LSHIFT] || keyboard[SDL_SCANCODE_RSHIFT] ? 'A' : 'a');
          write(m, &out, 1);
        } if (pressed >= SDL_SCANCODE_1 && pressed <= SDL_SCANCODE_0) {
          int offset = pressed - SDL_SCANCODE_1;

          if(!keyboard[SDL_SCANCODE_LSHIFT]) {
          char out = (offset + 1) % 10 + '0';
          write(m, &out, 1);
          } else {
            write(m, numberSymbols+offset, 1);
          }
        } if (pressed == SDL_SCANCODE_RETURN) {
          const char* out = "\n";
          write(m, out, 1);
        } if (pressed == SDL_SCANCODE_SPACE) {
          const char* out = " ";
          write(m, out, 1);
        }
      }
    }

    int length = read(m, buffer, 10000);
    buffer[length] = '\0';
    bufferToQueue(&queue, buffer, length);

    while(!isEmpty(&queue)) {
      char nBuffer[1500];
      int length = 1500;
      dequeue(&queue, nBuffer, &length);

      perform(&display, nBuffer);
    }

    draw(ren, &display);
    drawCursor(ren, &display);
    SDL_RenderPresent(ren);
  }

  return 0;
}

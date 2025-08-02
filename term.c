#include "term.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"

#define SOURCE_RECT_SIDE_LENGTH 20
#define SOURCE_RECT_COLUMN_SIZE 15
#define DEST_RECT_SIDE_LENGTH 20

#define CURSOR_FLICKER_TIME 25

const char* numberSymbols = "!@#$|-&*()";

inline int getModeFlag(const struct DisplayData* display, int mode) {
  return (display->mode & mode) > 0;
}

void advanceRow(struct DisplayData* display) {
  if(display->cursory == display->rows - 1) {
    for(int i = 0; i < display->rows - 1; i++) {
      for(int j = 0; j < display->columns; j++) {
        display->screen[i*display->columns + j] = display->screen[(i+1)*display->columns+j];
      }
    }

    for(int j = 0; j < display->columns; j++) {
      const int bottomr = (display->rows - 1) * display->columns;
      display->screen[bottomr + j] = '\0';
    }
  } else {
    display->cursory++;
  }
}

void perform(struct DisplayData* display, const char* buffer) {
  int length = strlen(buffer);

  if(length == 0) return;

  if(buffer[0] != ASCII_ESCAPE) {
    for(int i = 0; i < length; i++) {
      if(buffer[i] == '\r') display->cursorx = 0;
      else if(buffer[i] == '\n') {
        display->cursorx = 0;
        advanceRow(display);
      } else {
        display->screen[display->cursory * display->columns + display->cursorx] = buffer[i];
        display->cursorx++;
        if(display->cursorx >= display->columns) {
          display->cursorx = 0;
          advanceRow(display);
        }

        if(display->cursory >= display->rows) {
          display->cursory = 0;
        }
      }
    }
  } else if(buffer[1] == '[') {
    int n1, n2;

    switch(buffer[length-1]) {
    case 'A':
      n1 = parseNumber(buffer, 2, length-2, NULL);
      display->cursory = display->cursory >= n1 ? display->cursory - n1 : 0;
      break;
    case 'B':
      n1 = parseNumber(buffer, 2, length-2, NULL);
      display->cursory = display->cursory + n1 < display->rows ?
        display->cursory + n1 : display->rows - 1;
      break;
    case 'D':
      n1 = parseNumber(buffer, 2, length-2, NULL);
      display->cursorx = display->cursorx >= n1 ? display->cursorx - n1 : 0;
      break;
    case 'C':
      n1 = parseNumber(buffer, 2, length-2, NULL);
      display->cursorx = display->cursorx + n1 < display->columns ?
        display->cursorx + n1 : display->columns - 1;
      break;
    default:
      break;
    }
  }
}


SDL_Texture* loadTexture(SDL_Renderer* ren, char* filename) {
  SDL_Texture* newTexture = NULL;

  SDL_Surface* loadedSurface = IMG_Load(filename);
  if(!loadedSurface) {
    printf("Unable to load image: %s! SDL_image Error: %s\n", filename, IMG_GetError());
  } else {
    newTexture = SDL_CreateTextureFromSurface(ren, loadedSurface);

    if(!newTexture) {
      printf("Unable to load texture from: %s! SDL Error: %s\n", filename, SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);
  }

  return newTexture;
}


void draw(SDL_Renderer* ren, const struct DisplayData* display) {
  static SDL_Texture* font = NULL;

  static SDL_Rect sourceRect = (SDL_Rect) { .w = SOURCE_RECT_SIDE_LENGTH, .h = SOURCE_RECT_SIDE_LENGTH };
  static SDL_Rect destRect = (SDL_Rect) {.w = DEST_RECT_SIDE_LENGTH, .h = DEST_RECT_SIDE_LENGTH };

  if(font == NULL) {
    font = loadTexture(ren, "assets/fontno.png");
  }

  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  for(int i = 0; i < display->rows; i++)
    for(int j = 0; j < display->columns; j++) {
      unsigned char current = display->screen[i*display->columns + j];

      if(current > 0) {
        sourceRect.x =
          (current - 32) % SOURCE_RECT_COLUMN_SIZE * SOURCE_RECT_SIDE_LENGTH;
        sourceRect.y =
          (current - 32) / SOURCE_RECT_COLUMN_SIZE * SOURCE_RECT_SIDE_LENGTH;

        destRect.x = j * DEST_RECT_SIDE_LENGTH;
        destRect.y = i * DEST_RECT_SIDE_LENGTH;

        SDL_RenderCopy(ren, font, &sourceRect, &destRect);
      }
    }

}

void drawCursor(SDL_Renderer* ren, const struct DisplayData* display) {
  static int timer = 0;
  static SDL_Rect cursorRect = (SDL_Rect) {.w = DEST_RECT_SIDE_LENGTH, .h = DEST_RECT_SIDE_LENGTH };

  if(timer / CURSOR_FLICKER_TIME == 0) {
    cursorRect.x = display->cursorx * DEST_RECT_SIDE_LENGTH;
    cursorRect.y = display->cursory * DEST_RECT_SIDE_LENGTH;

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderFillRect(ren, &cursorRect);
  }

  timer = (timer + 1) % (2 * CURSOR_FLICKER_TIME);
}

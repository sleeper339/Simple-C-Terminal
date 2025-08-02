#include <stdio.h>
#include "parser.h"

int isAlphaNum(char input) {
  return (input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z');
}

int isNumber(char input) {
  return input >= '0' && input <= '9';
}

int parseNumber(const char* buffer, int start, int length, int* status) {
  int accum = 0;

  if (start >= length || !isNumber(buffer[start])) {
    if(status != NULL) *status = 0;
  } else {
    while(isNumber(buffer[start])) {
      accum *= 10;
      accum += buffer[start] - '0';
      start++;
    }
    if(status != NULL) *status = 1;

    return accum;
  }

  return 0;
}

void bufferToQueue(Queue* queue, const char* buffer, int length) {
  for(int i = 0; i < length;) {
    int counter = i;
    while(buffer[counter] != ASCII_ESCAPE) counter++;

    if(counter != i) {
      enqueue(queue, buffer + i, counter - i);
      i = counter;
    } else {
      switch(buffer[i+1]) {
      case '[':
        counter = 0;
        while(!isAlphaNum(buffer[i+counter])) counter++;
        enqueue(queue, buffer + i, counter);
        i += counter+1;
        break;
      case '=':
      case '>':
      case 'N':
      case 'O':
      case 'D':
      case 'M':
      case 'E':
      case '7':
      case '8':
      case 'H':
      case 'c':
        enqueue(queue, buffer + i, 2);
        i += 3;
      default:
        enqueue(queue, buffer + i, 3);
        i += 4;
        break;
      }
    }
  }
}

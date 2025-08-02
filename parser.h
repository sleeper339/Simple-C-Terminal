#ifndef PARSER_H_
#define PARSER_H_

#include "queue.h"

#define ASCII_NEWLINE '\n'
#define ASCII_RETURN '\r'
#define ASCII_ESCAPE 27

int parseNumber(const char* buffer, int start, int length, int* status);
void bufferToQueue(Queue* queue, const char* buffer, int length);

#endif // PARSER_H_

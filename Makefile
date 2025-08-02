##
#
#
# @file
# @version 0.1

CC = clang
CFLAGS = -g -lSDL2 -lSDL2_image -Wall
CFILES = $(wildcard *.c)
HEADERS = $(wildcard *.h)

build/sdlsh: $(HEADERS) $(CFILES)
	$(CC) $(CFILES) $(CFLAGS) -o build/sdlsh

run: build/sdlsh
	build/sdlsh

install: build/sdlsh
	cp build/sdlsh /usr/local/bin

uninstall: /usr/local/bin/sdlsh
	rm /usr/local/bin/sdlsh

# end

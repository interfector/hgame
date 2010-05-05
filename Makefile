# hgame

VERSION = 0.0.1

# includes and libs
LIBS = -ldl 

# flags
CFLAGS = -Wall -O3 ${LIBS} -DVERSION=\"${VERSION}\" -I./include -fno-stack-protector

SRC = src/main.c src/util.c src/hgame.c src/module.c
BIN = hgame

all:
	gcc -o $(BIN) $(SRC) $(CFLAGS)

clean:
	rm -f $(BIN)

install: all
	cp -f $(BIN) /usr/local/bin/
	mkdir -p ~/.hgame
	mkdir -p ~/.hgame/bin
	mkdir -p ~/.hgame/files
	mkdir -p ~/.hgame/devices
	mkdir -p ~/.hgame/missions
	cp -r example/bin/*.so ~/.hgame/bin
	cp -r example/devices/* ~/.hgame/devices
	cp -r example/missions/* ~/.hgame/missions
	chown -R 1000:1000 ~/.hgame

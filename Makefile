# hgame

VERSION = 0.0.1-pre-alpha

# includes and libs
LIBS = -ldl -lpthread -lssl -lreadline

# flags
CFLAGS = -Wall -O3 ${LIBS} -DVERSION=\"${VERSION}\" -I./include -fno-stack-protector

SRC = src/main.c src/util.c src/hgame.c src/module.c
BIN = hgame

all:
	gcc -o $(BIN) $(SRC) $(CFLAGS)

clean:
	rm -rf ~/.hgame/bin
	rm -rf ~/.hgame/devices
	rm -rf ~/.hgame/missions
	rm -f $(BIN)

install: all
	cp -f $(BIN) /usr/games/
	mkdir -p ~/.hgame
	mkdir -p ~/.hgame/bin
	mkdir -p ~/.hgame/files
	mkdir -p ~/.hgame/devices
	mkdir -p ~/.hgame/missions
	cp -r example/bin/*.so ~/.hgame/bin
	cp example/devices/mail ~/.hgame/devices
	cp example/devices/hosts ~/.hgame/devices
	cp -r example/missions ~/.hgame/
	chown -R 1000:1000 ~/.hgame

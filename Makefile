FILES=\
src/Snake.cpp \
src/Food.cpp \
src/snake_game.cpp

PROGRAM=snake_game

CXXFLAGS= -std=c++14 -Wall -Wextra -Wpedantic

LFLAGS=-lSDL2 -lSDL2_image

all: ${PROGRAM}

${PROGRAM}: ${FILES}
	g++ ${CXXFLAGS} ${FILES} ${LFLAGS} -Iinc -o ${PROGRAM}
	@echo "\n\nRun with ./snake_game <SPEED> <NUMBER OF PLAYERS>"

clean:
	rm -f ${PROGRAM}

CC = gcc
CFLAGS = $(shell sdl2-config --cflags) -Wall -Wextra -O2
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf -lpthread -lm

SRC = main.c virus_sim.c
OBJ = $(SRC:.c=.o)
EXEC = virus_sim

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
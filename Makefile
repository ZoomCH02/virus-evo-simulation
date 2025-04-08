CC = gcc
CFLAGS = `sdl2-config --cflags` -Wall -Wextra -O2
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf -lm

SRC = main.c virus_sim.c
OBJ = $(SRC:.c=.o)
TARGET = virus_sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
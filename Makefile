CC = gcc
CFLAGS = -Wall -g
INCLUDES = -Iinclude
SRC = src/main.c src/vfs.c
OUT = vfs

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

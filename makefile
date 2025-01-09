CC = gcc
CFLAGS = -Wall -g
SERVER_SRC = server.c client.c
SERVER_OUT = server

MAIN_SRC = main.c client.c
MAIN_OUT = main

all: $(SERVER_OUT) $(MAIN_OUT)

$(SERVER_OUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_OUT) $(SERVER_SRC)

$(MAIN_OUT): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $(MAIN_OUT) $(MAIN_SRC)

clean:
	rm -f $(SERVER_OUT) $(MAIN_OUT)
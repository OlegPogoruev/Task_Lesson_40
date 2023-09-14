#цель: зависимость
#[tab] команда


BIN = TCPClient
SOURCES = main.c
HEADERS = main.h
CC := gcc

all:
	$(CC) $(SOURCES) -o $(BIN).out
$(BIN): $(HEADERS)

run: all
	ulimit -n
	./$(BIN).out

limset:
	ulimit -n 65536

limview:
	ulimit -n

#clean:
#	rm -rf $(BIN) *.o $(BIN).out




















#цель: зависимость
#[tab] команда


BIN = TCP_Server
SOURCES = main.c
HEADERS = main.h
CC := gcc

all:
	$(CC) $(SOURCES) -lpthread -o $(BIN).out
$(BIN): $(HEADERS)

run: all
	./$(BIN).out

limset:
	ulimit -n 262144

limview:
	ulimit -n

#clean:
#	rm -rf $(BIN) *.o $(BIN).out




















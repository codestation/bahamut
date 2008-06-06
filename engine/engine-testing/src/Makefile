CC=g++
TARGET=bahamut-engine
CFLAGS=-c -Wall
LDFLAGS= -lpthread -lpcap
OBJECTS=$(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): main.o Interface.o Socket.o
	$(CC) $(LDFLAGS) main.o Interface.o Socket.o -o $(TARGET)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Interface.o: Interface.cpp
	$(CC) $(CFLAGS) Interface.cpp

Socket.o: Socket.cpp
	$(CC) $(CFLAGS) Socket.cpp

clean:
	rm -rf *o $(TARGET)
CC=g++
TARGET=bahamut-engine
CFLAGS=-c -Wall
LDFLAGS= -lpthread -lpcap
OBJECTS=$(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): main.o Interface.o Socket.o PspPacket.o DeviceContainer.o DeviceInfo.o
	$(CC) $(LDFLAGS) main.o Interface.o Socket.o PspPacket.o DeviceContainer.o DeviceInfo.o -o $(TARGET)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Interface.o: Interface.cpp
	$(CC) $(CFLAGS) Interface.cpp

Socket.o: Socket.cpp
	$(CC) $(CFLAGS) Socket.cpp
	
spPacket.o: PspPacket.cpp
	$(CC) $(CFLAGS) PspPacket.cpp

DeviceContainer.o: DeviceContainer.cpp
	$(CC) $(CFLAGS) DeviceContainer.cpp

DeviceInfo.o: DeviceInfo.cpp
	$(CC) $(CFLAGS) DeviceInfo.cpp

clean:
	rm -rf *o $(TARGET)
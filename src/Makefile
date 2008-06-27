CC = g++
TARGET = bahamut-engine
CFLAGS = -Wall
LIBS = -lpcap -lpthread
OBJS = main.o Interface.o Socket.o PspPacket.o DeviceInfo.o ServerSocket.o UDPServer.o ClientInfo.o DeviceBridge.o InterfaceInfo.o List.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

clean:
	rm -f ${OBJS} $(TARGET)

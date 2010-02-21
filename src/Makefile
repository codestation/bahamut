CC = g++
TARGET = bahamut-engine
CFLAGS = -O3 -pipe -fomit-frame-pointer -Wall
LIBS = -lpcap -lpthread
OBJS = main.o			\
		Interface.o		\
		Socket.o		\
		Packet.o		\
		EthPacket.o		\
		DeviceInfo.o 	\
		ServerSocket.o	\
		UDPServer.o		\
		ClientInfo.o	\
		DeviceBridge.o	\
		InterfaceInfo.o \
		List.o			\
		Logger.o		\
		Thread.o		\
		TCPServer.o		\
		TCPThread.o		\
		SpeedThread.o	\
		HTTPParser.o	\
		HTTPResponse.o	\
		AbstractSocket.o \
		ArgParser.o

SVNREV:= -D'SVN_REV="$(shell LC_ALL=C svnversion -cn ../ | sed -e "s/.*://" -e "s/\([0-9]*\).*/\1/" | grep "[0-9]" )"'
CFLAGS += $(SVNREV)


all: $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(TARGET)

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

clean:
	rm -f ${OBJS} $(TARGET)

#ifndef SOCKET_H_
#define SOCKET_H_

#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define PACKET_HEADER 10

class Socket
{
private:
	int port;
	int sock;
	int socket_flag;
	char *host;
	int proto;
	u_int packet_count;
	sockaddr_in client;

#pragma pack(push,1)
	struct packet_container {
		u_char header[2];
		u_int counter;
		u_int size;
		u_char data[1490];
	};
#pragma pack(pop)
	
	in_addr_t resolve(const char *url);
public:
	Socket(const char *addr, int port, const char *proto);
	Socket(int s);
	virtual ~Socket();
	bool connectSocket();
	ssize_t readSocket(void *, size_t size);
	ssize_t writeSocket(const void *, size_t size);
	void closeSocket();
};

#endif /*SOCKET_H_*/

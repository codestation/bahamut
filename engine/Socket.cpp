#include "Socket.h"

Socket::Socket(const char *addr, int port, const char *proto) {
	sock = 0;
	socket_flag = 1;
	packet_count = 0;
	host = strdup(addr);
	if(strcmp(proto,"tcp") == 0) {
		this->proto = IPPROTO_TCP;
	} else if (strcmp(proto,"udp") == 0) {
		this->proto = IPPROTO_UDP;
	} else {
		this->proto = -1;
	}
	this->port = port;
}

Socket::Socket(int sock) {
	this->sock = sock;
}

bool Socket::connectSocket() {
	if((sock = socket(PF_INET, proto == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM, proto)) >= 0) {
		memset(&client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(port);
		client.sin_len = sizeof(client);
		client.sin_addr.s_addr = ((in_addr*)((hostent *)gethostbyname(host)->h_addr))->s_addr;
		if(proto == IPPROTO_TCP) {
			//Disable the Nagle algorithm
			if(setsockopt(sock, IPPROTO_TCP, 1, &socket_flag, sizeof(int)) != 0) {
				return false;
			}
		}
		return connect(sock, (struct sockaddr *)&client, sizeof(client)) >= 0;
	} else {
		return false;
	}
}

ssize_t Socket::readSocket(void *buffer, size_t size) {
	return recv(sock, buffer, size, 0);
}

ssize_t Socket::writeSocket(const void *data, size_t length) {
	struct packet_container pkt;
	pkt.size = length;
	pkt.header[0] = 'M';
	pkt.header[1] = 'H';
	pkt.counter = packet_count;
	memcpy(pkt.data, data, length);
	int res = send(sock, &pkt, length + PACKET_HEADER , 0);
	if(res > 0) {
		packet_count++;
	}
	return res;
}

void Socket::closeSocket() {
	if(sock) {
		close(sock);
		sock = 0;
	}
}

Socket::~Socket() {
	closeSocket();
}

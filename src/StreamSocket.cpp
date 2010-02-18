/*
 * StreamSocket.cpp
 *
 *  Created on: 18/02/2010
 *      Author: code
 */

#include "StreamSocket.h"

StreamSocket::StreamSocket(Socket *s) {
	sock = s;
	stream = NULL;

}

bool StreamSocket::openStream() {
	return (stream = fdopen(sock->getDescriptor(), "r+"));
}

char *StreamSocket::readStream(char *data, int size) {
	return fgets(data, size, stream);
}

bool StreamSocket::writeStream(const char *data) {
	return fputs(data, stream) >= 0;
}

void StreamSocket::closeStream() {
	if(stream) {
		fclose(stream);
		stream = NULL;
	}
}

StreamSocket::~StreamSocket() {
	closeStream();
}

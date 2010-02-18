/*
 * StreamSocket.h
 *
 *  Created on: 18/02/2010
 *      Author: code
 */

#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include <stdio.h>
#include "Socket.h"

class StreamSocket {
	Socket *sock;
	FILE *stream;
public:
	StreamSocket(Socket *s);
	bool openStream();
	char *readStream(char *data, int size);
	bool writeStream(const char *data);
	void closeStream();
	virtual ~StreamSocket();
};

#endif /* STREAMSOCKET_H_ */

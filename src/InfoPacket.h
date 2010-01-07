/*
 * InfoPacket.h
 *
 *  Created on: 11-ago-2008
 *      Author: code
 */

#ifndef INFOPACKET_H_
#define INFOPACKET_H_

#include <stdlib.h>

class InfoPacket {
private:
	struct info {
		u_char id;
		u_char type;
		u_char mac[6];
	};
	struct info packet;
public:
	InfoPacket();
	void setMAC(const u_char *mac);
	const u_char *getMAC();
	void setID(u_char id);
	u_char getID();
	virtual ~InfoPacket();
};

#endif /* INFOPACKET_H_ */

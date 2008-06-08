#ifndef PSPINFO_H_
#define PSPINFO_H_

#include <stdlib.h>
#include <string.h>

class PspInfo {
private:
	u_char mac[6];
	int count;
public:
	PspInfo(u_char *psp_mac);
	const u_char *getMAC();
	void setMAC(const u_char *mac);
	int getPacketCounter();
	void setPacketCounter(int newcount);
	virtual ~PspInfo();
};

#endif /*PSPINFO_H_*/

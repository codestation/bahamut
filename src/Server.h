/*
*Project Name: Bahamut
* 
*Programmers: Codestation, ZackVixACD
* 
*Project Descrption: The project bahamut is a full ad-hoc tunneling
*	software to be used by the Playstation Portable (PSP) to emulate 
*	online features.
*
*File Description:
*	Server TCP/UDP skel
*Special Notes:
*	TODO: incomplete
*
*Copyright Stuff:
* 	TODO: put GPLv3 header
*/

#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>

class Server
{
private:
	pthread_t th;
protected:
	static void *run(void *);
public:
	Server();
	virtual ~Server();
	void start();
};

#endif /*SERVER_H_*/

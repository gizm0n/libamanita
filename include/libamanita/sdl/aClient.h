#ifndef _LIBAMANITA_SDL_ACLIENT_H
#define _LIBAMANITA_SDL_ACLIENT_H

#include <libamanita/sdl/aSocket.h>



class aClient : public aSocket {
private:
	uint32_t id;
	char *nick;
#ifndef TCPSOCK_NOCIPHER
	uint32_t *key;
	int keylen;
#endif /*TCPSOCK_NOCIPHER*/

	static int _run(void *p) { ((aClient *)p)->run();return 0; }
	void run();

public:
	aClient(SocketListener l,uint32_t id=0,const char *nick=0);
	~aClient();

	bool start(const char *con);
	bool start(const char *host,uint16_t port);
	void stop(bool kill=true);

	int send(uint8_t *d,size_t l);

	uint32_t getID() { return id; }
	void setNick(const char *n);
	const char *getNick() { return nick; }
#ifndef TCPSOCK_NOCIPHER
	void setKey(const uint32_t *k,size_t l);
#endif /*TCPSOCK_NOCIPHER*/
};


#endif /* _LIBAMANITA_SDL_ACLIENT_H */


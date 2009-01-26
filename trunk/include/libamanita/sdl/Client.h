#ifndef _LIBAMANITA_SDL_CLIENT_H
#define _LIBAMANITA_SDL_CLIENT_H

#include <libamanita/sdl/Socket.h>



class Client : public Socket {
private:
	uint32_t id;
	char *nick;
#ifndef TCPSOCK_NOCIPHER
	uint32_t *key;
	int keylen;
#endif /*TCPSOCK_NOCIPHER*/

	static int _run(void *p) { ((Client *)p)->run();return 0; }
	void run();

public:
	Client(SocketListener l,uint32_t id=0,const char *nick=0);
	~Client();

	bool start(const char *con);
	bool start(const char *host,Uint16 port);
	void stop(bool kill=true);

	int send(void *p,uint32_t l);

	uint32_t getID() { return id; }
	void setNick(const char *n);
	const char *getNick() { return nick; }
#ifndef TCPSOCK_NOCIPHER
	void setKey(const uint32_t *k,int l);
#endif /*TCPSOCK_NOCIPHER*/
};


#endif /* _LIBAMANITA_SDL_CLIENT_H */


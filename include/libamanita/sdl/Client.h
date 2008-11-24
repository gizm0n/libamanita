#ifndef _LIBAMANITA_SDL_CLIENT_H
#define _LIBAMANITA_SDL_CLIENT_H

#include <libamanita/sdl/Socket.h>



class Client : public Socket {
private:
	Uint32 id;
	char *nick;
#ifndef TCPSOCK_NOCIPHER
	Uint32 *key;
	int keylen;
#endif /*TCPSOCK_NOCIPHER*/

	static int _run(void *p) { ((Client *)p)->run();return 0; }
	void run();

public:
	Client(Uint32 (*l)(Socket *,Uint32,Uint32,Uint32,Uint32),Uint32 id=0,const char *nick=0);
	~Client();

	bool start(const char *con);
	bool start(const char *host,Uint16 port);
	void stop(bool kill=true);

	int send(void *p,Uint32 l);

	Uint32 getID() { return id; }
	void setNick(const char *n);
	const char *getNick() { return nick; }
#ifndef TCPSOCK_NOCIPHER
	void setKey(const Uint32 *k,int l);
#endif /*TCPSOCK_NOCIPHER*/
};


#endif /* _LIBAMANITA_SDL_CLIENT_H */


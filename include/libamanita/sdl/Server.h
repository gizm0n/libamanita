#ifndef _LIBAMANITA_SDL_SERVER_H
#define _LIBAMANITA_SDL_SERVER_H

#include <string.h>
#include <libamanita/sdl/Socket.h>
#include <libamanita/Hashtable.h>
#include <libamanita/Vector.h>


enum {
	CON_ACTIVE		= 1,
	CON_OPERATOR	= 2,
};


class ServerChannel : public Vector {
private:
	char *name;

public:
	ServerChannel(const char *nm) : Vector() { name = strdup(nm); }
	~ServerChannel() { free(name); }

	const char *getName() { return name; }
};

typedef ServerChannel *Channel;


class ServerConnection {
friend class Server;
private:
	TCPsocket sock;
	uint32_t id;
	char *nick;
	unsigned char status;
	Vector channels;
#ifndef TCPSOCK_NOCIPHER
	uint32_t *key;
	int keylen;
#endif /*TCPSOCK_NOCIPHER*/

	ServerConnection(TCPsocket sock,uint32_t id,const char *n)
			: sock(sock),id(id),status(0),channels()
#ifndef TCPSOCK_NOCIPHER
					,key(0),keylen(0)
#endif /*TCPSOCK_NOCIPHER*/
						{ nick = strdup(n); }
	~ServerConnection() { free(nick);
#ifndef TCPSOCK_NOCIPHER
		if(key) free(key);
#endif /*TCPSOCK_NOCIPHER*/
			}
public:
	const TCPsocket getSocket() { return sock; }
	uint32_t getID() { return id; }
	void setNick(const char *n) { free(nick);nick = strdup(n); }
	const char *getNick() { return nick; }
	void setActive(bool a) { status |= CON_ACTIVE;if(!a) status ^= CON_ACTIVE; }
	bool isPending() { return !(status&CON_ACTIVE); }
	bool isActive() { return status&CON_ACTIVE; }
	void setOperator(bool o) { status |= CON_OPERATOR;if(!o) status ^= CON_OPERATOR; }
	bool isOperator() { return status&CON_OPERATOR; }
#ifndef TCPSOCK_NOCIPHER
	void setKey(const uint32_t *k,int l);
#endif /*TCPSOCK_NOCIPHER*/

	void joinChannel(Channel c) { if(c) channels += c; }
	void leaveChannel(Channel c) { if(c) channels -= c; }
	Vector &getChannels() { return channels; }
};

typedef ServerConnection *Connection;

class Server : public Socket {
private:
	size_t setsz;				/** < SDL_SocketSet set, allocated size. */
	SDL_mutex *mut;			/** < Server mutex. */
	Hashtable clients;		/** < All clients connected to this server. */
	ServerChannel main;		/** < Main channel, contains all clients connected to this server. */
	Hashtable channels;		/** < All channels in this server. */

	Connection addClient(TCPsocket s,void *p,uint32_t l);
	void createSocketSet();
	static int _run(void *p) { ((Server *)p)->run();return 0; }
	void run();
	bool uniqueID(uint32_t id) { return clients.get((unsigned long)id)==0; }
	void killClient(Connection c) { killClient(c->id); }
	void killClient(uint32_t id);
	void killAllClients();

public:
	Server(SocketListener l);
	~Server();

	int lock() { return SDL_mutexP(mut); }
	int unlock() { return SDL_mutexV(mut); }

	bool start(const char *con);
	bool start(uint32_t port);
	void stop(bool kill=true);

	Connection getClient(uint32_t id) { return (Connection)clients.get((unsigned long)id); }
	void changeNick(uint32_t id,const char *nick);

	Channel createChannel(const char *ch);
	void deleteChannel(const char *ch);
	Channel getChannel(const char *ch) { return ch && *ch? (Channel)channels.get(ch) : 0; }
	void joinChannel(const char *ch,Connection c) { if(c) joinChannel(createChannel(ch),c); }
	void joinChannel(Channel ch,Connection c) { if(ch && c && !ch->contains(c)) { *ch += c;c->joinChannel(ch); } }
	void leaveChannel(const char *ch,Connection c) { if(c) leaveChannel(getChannel(ch),c); }
	void leaveChannel(Channel ch,Connection c);
	Hashtable::iterator getChannels() { return channels.iterate(); }

	int send(Connection c,void *p,uint32_t l);
	void send(void *p,uint32_t l) { send((Channel)0,p,l); }
	void send(Channel channel,void *p,uint32_t l);
};


#endif /* _LIBAMANITA_SDL_SERVER_H */


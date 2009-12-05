#ifndef _LIBAMANITA_NET_SERVER_H
#define _LIBAMANITA_NET_SERVER_H

/**
 * @file libamanita/net/aServer.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <string.h>
#include <libamanita/aHashtable.h>
#include <libamanita/aVector.h>
#include <libamanita/net/aSocket.h>


enum {
	CON_ACTIVE		= 1,
	CON_OPERATOR	= 2,
};


class ServerChannel : public aVector {
private:
	char *name;

public:
	ServerChannel(const char *nm) : aVector() { name = strdup(nm); }
	~ServerChannel() { free(name); }

	const char *getName() { return name; }
};

typedef ServerChannel *Channel;


class ServerConnection {
friend class aServer;
private:
	TCPsocket sock;
	uint32_t id;
	char *nick;
	uint8_t status;
	aVector channels;
#ifndef TCPSOCK_NOCIPHER
	uint32_t *key;
	size_t keylen;
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
	void setKey(const uint32_t *k,size_t l);
#endif /*TCPSOCK_NOCIPHER*/

	void joinChannel(Channel c) { if(c) channels += c; }
	void leaveChannel(Channel c) { if(c) channels -= c; }
	aVector &getChannels() { return channels; }
};

typedef ServerConnection *Connection;

class aServer : public aSocket {
private:
	size_t setsz;				/** < SDL_SocketSet set, allocated size. */
	SDL_mutex *mut;			/** < aServer mutex. */
	aHashtable clients;		/** < All clients connected to this server. */
	ServerChannel main;		/** < Main channel, contains all clients connected to this server. */
	aHashtable channels;		/** < All channels in this server. */

	Connection addClient(TCPsocket s,uint8_t *d,size_t l);
	void createSocketSet();
	static int _run(void *p) { ((aServer *)p)->run();return 0; }
	void run();
	bool uniqueID(uint32_t id) { return clients.get(id)==0; }
	void killClient(Connection c) { killClient(c->id); }
	void killClient(uint32_t id);
	void killAllClients();

public:
	aServer(SocketListener l);
	~aServer();

	int lock() { return SDL_mutexP(mut); }
	int unlock() { return SDL_mutexV(mut); }

	bool start(const char *con);
	bool start(uint32_t port);
	void stop(bool kill=true);

	Connection getClient(uint32_t id) { return (Connection)clients.get(id); }
	void changeNick(uint32_t id,const char *nick);

	Channel createChannel(const char *ch);
	void deleteChannel(const char *ch);
	Channel getChannel(const char *ch) { return ch && *ch? (Channel)channels.get(ch) : 0; }
	void joinChannel(const char *ch,Connection c) { if(c) joinChannel(createChannel(ch),c); }
	void joinChannel(Channel ch,Connection c) { if(ch && c && !ch->contains(c)) { *ch += c;c->joinChannel(ch); } }
	void leaveChannel(const char *ch,Connection c) { if(c) leaveChannel(getChannel(ch),c); }
	void leaveChannel(Channel ch,Connection c);
	aHashtable::iterator getChannels() { return channels.iterate(); }

	int send(Connection c,uint8_t *d,size_t l);
	void send(uint8_t *d,size_t l) { send((Channel)0,d,l); }
	void send(Channel channel,uint8_t *d,size_t l);
};


#endif /* _LIBAMANITA_NET_SERVER_H */


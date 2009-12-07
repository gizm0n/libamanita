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



/** Server channel.
 *
 * This class is used by aServer to handle channels.
 * @see aServer
 */
class aServerChannel : public aVector {
private:
	char *name;		//!< Name of channel.

public:
	/** Constructor. */
	aServerChannel(const char *nm) : aVector() { name = strdup(nm); }
	/** Destructor. */
	~aServerChannel() { free(name); }

	/** Get name of channel. */
	const char *getName() { return name; }
};



/** The aChannel typedef is prefered as a type for channels.
 * @see aServerChannel
 */
typedef aServerChannel *aChannel;



/** Status values for aServerConnection. */
enum {
	CON_ACTIVE				= 1,		//!< Is set if connection is active.
	CON_OPERATOR			= 2,		//!< Is set if client has operator access in channel.
};


/** Server connection.
 *
 * This class is used by aServer to handle connections to clients. It contains
 * all client data and which channels the client is in.
 * @see aServer
 */
class aServerConnection {
friend class aServer;

private:
	tcp_socket_t sock;			//!< Socket to client.
	uint32_t id;					//!< Client's unique id.
	char *nick;						//!< Client's nick.
	uint8_t status;				//!< Status of connection.
	aVector channels;				//!< Channels client is in.
#ifndef SOCKET_NOCIPHER
	uint32_t *key;					//!< For encrypted connections, the encryption key.
	size_t keylen;					//!< Length of key.
#endif /*SOCKET_NOCIPHER*/

	/** Constructor.
	 * The constructor can only be called by aServer. */
	aServerConnection(tcp_socket_t sock,uint32_t id,const char *n)
			: sock(sock),id(id),status(0),channels()
#ifndef SOCKET_NOCIPHER
					,key(0),keylen(0)
#endif /*SOCKET_NOCIPHER*/
						{ nick = strdup(n); }

	/** Destructor.
	 * The destructor can only be called by aServer. */
	~aServerConnection() { free(nick);
#ifndef SOCKET_NOCIPHER
		if(key) free(key);
#endif /*SOCKET_NOCIPHER*/
			}

public:
	const tcp_socket_t getSocket() { return sock; }				//!< Get TCP socket.
	uint32_t getID() { return id; }									//!< Get ID.
	/** Change nick for client. */
	void setNick(const char *n) { free(nick);nick = strdup(n); }
	const char *getNick() { return nick; }							//!< Get nick.
	/** Set whether connection is active. */
	void setActive(bool a) { status |= CON_ACTIVE;if(!a) status ^= CON_ACTIVE; }
	bool isPending() { return !(status&CON_ACTIVE); }			//!< True if connection is waiting to be inserted in queue by server.
	bool isActive() { return status&CON_ACTIVE; }				//!< True if connection is active.
	/** Set connection to be operator. */
	void setOperator(bool o) { status |= CON_OPERATOR;if(!o) status ^= CON_OPERATOR; }
	bool isOperator() { return status&CON_OPERATOR; }			//!< true if connection is operator.
#ifndef SOCKET_NOCIPHER
	void setKey(const uint32_t *k,size_t l);						//!< Set encryption key.
#endif /*SOCKET_NOCIPHER*/

	void joinChannel(aChannel c) { if(c) channels += c; }		//!< Join a channel.
	void leaveChannel(aChannel c) { if(c) channels -= c; }	//!< Leave a channel.
	aVector &getChannels() { return channels; }					//!< Get channels client is in.
};



/** The aConnection typedef is prefered as a type for connections.
 * @see aServerConnection
 */
typedef aServerConnection *aConnection;




/** A server class for TCP networking.
 *
 * This server can handle a large number of clients and use it's own built in protocol.
 * @see aSocket
 * @see aClient
 */
class aServer : public aSocket {
private:
#ifdef LIBAMANITA_SDL
	size_t setsz;				//!< SDL_SocketSet set, allocated size.
	SDL_mutex *mut;			//!< aServer mutex.
#elif defined __linux__
	pthread_mutex_t mut;
#endif /* LIBAMANITA_SDL */
	aHashtable sockets;		//!< All sockets connected to this server. Stored as socket=>aConnection.
	aHashtable clients;		//!< All clients connected to this server. Stored as ID=>aConnection.
	aServerChannel main;		//!< Main channel, contains all clients connected to this server.
	aHashtable channels;		//!< All channels in this server.

	/** Static method called by the thread and in turn calls run().
	 * @see run */
	static thread_func_t _run(void *p) { ((aServer *)p)->run();return 0; }
	/** Called by the thread, and handles all incomming messages. */
	void run();

	aConnection addClient(tcp_socket_t s,uint8_t *d,size_t l);
#ifdef LIBAMANITA_SDL
	void createSocketSet(int n=0);
#elif defined __linux__
#endif /* LIBAMANITA_SDL */
	bool uniqueID(uint32_t id) { return clients.get(id)==0; }
	void killClient(aConnection c) { killClient(c->id); }
	void killClient(uint32_t id);
	void killAllClients();

public:
	/** @name Constructors and Destructors
	 * @{ */
	/** Constructor. */
	aServer(socket_event_handler seh);
	/** Destructor. */
	~aServer();
	/** @} */

	/** @name Thread
	 * @{ */
#ifdef LIBAMANITA_SDL
	/** Lock the mutext. */
	int lock() { return SDL_mutexP(mut); }
	/** Unlock the mutext. */
	int unlock() { return SDL_mutexV(mut); }
#elif defined __linux__
	/** @cond */
	int lock() { return pthread_mutex_lock(&mut); }
	int unlock() { return pthread_mutex_unlock(&mut); }
	/** @endcond */
#endif /* LIBAMANITA_SDL */

	bool start(const char *con);
	bool start(uint16_t p);
	void stop(bool kill=true);
	/** @} */

	/** @name Client
	 * @{ */
	aConnection getClient(uint32_t id) { return (aConnection)clients.get(id); }
	void changeNick(uint32_t id,const char *nick);
	/** @} */

	/** @name Channel
	 * @{ */
	aChannel createChannel(const char *ch);
	void deleteChannel(const char *ch);
	aChannel getChannel(const char *ch) { return ch && *ch? (aChannel)channels.get(ch) : 0; }
	void joinChannel(const char *ch,aConnection c) { if(c) joinChannel(createChannel(ch),c); }
	void joinChannel(aChannel ch,aConnection c) { if(ch && c && !ch->contains(c)) { *ch += c;c->joinChannel(ch); } }
	void leaveChannel(const char *ch,aConnection c) { if(c) leaveChannel(getChannel(ch),c); }
	void leaveChannel(aChannel ch,aConnection c);
	aHashtable::iterator getChannels() { return channels.iterate(); }
	/** @} */

	/** @name Send
	 * @{ */
	int send(aConnection c,uint8_t *d,size_t l);
	void send(uint8_t *d,size_t l) { send((aChannel)0,d,l); }
	void send(aChannel channel,uint8_t *d,size_t l);
	/** @} */
};


#endif /* _LIBAMANITA_NET_SERVER_H */


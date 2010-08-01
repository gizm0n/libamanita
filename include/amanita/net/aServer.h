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
	aVector _channels;			//!< Channels client is in.
	void *data;						//!< User defined data attached to this connection.
#ifndef SOCKET_NOCIPHER
	uint32_t *key;					//!< For encrypted connections, the encryption key.
	size_t keylen;					//!< Length of key.
#endif /*SOCKET_NOCIPHER*/

	/** Constructor.
	 * The constructor can only be called by aServer. */
	aServerConnection(tcp_socket_t sock,uint32_t id,const char *n)
			: sock(sock),id(id),status(0),_channels(),data(0)
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
	/** @name Socket
	 * @{ */
	const tcp_socket_t getSocket() { return sock; }				//!< Get TCP socket.
	/** @] */

	/** @name Client
	 * @{ */
	uint32_t getID() { return id; }									//!< Get ID.
	/** Change nick for client. */
	void setNick(const char *n) { free(nick);nick = strdup(n); }
	const char *getNick() { return nick; }							//!< Get nick.
	/** @} */

	/** @name Data
	 * User defined data. Can be any data, a struct, class, integer,
	 * anything that can be stored as a pointer to void.
	 * @{ */
	void setData(void *d) { data = d; }								//!< Set user defined data.
	void *getData() { return data; }									//!< Get user defined data.
	/** @} */

	/** @name Status
	 * @{ */
	/** Set whether connection is active. */
	void setActive(bool a) { status |= CON_ACTIVE;if(!a) status ^= CON_ACTIVE; }
	bool isPending() { return !(status&CON_ACTIVE); }			//!< True if connection is waiting to be inserted in queue by server.
	bool isActive() { return status&CON_ACTIVE; }				//!< True if connection is active.
	/** Set connection to be operator. */
	void setOperator(bool o) { status |= CON_OPERATOR;if(!o) status ^= CON_OPERATOR; }
	bool isOperator() { return status&CON_OPERATOR; }			//!< true if connection is operator.
	/** @} */

#ifndef SOCKET_NOCIPHER
	void setKey(const uint32_t *k,size_t l);						//!< Set encryption key.
#endif /*SOCKET_NOCIPHER*/

	void joinChannel(aChannel c) { if(c) _channels += c; }	//!< Join a channel.
	void leaveChannel(aChannel c) { if(c) _channels -= c; }	//!< Leave a channel.
	aVector &getChannels() { return _channels; }					//!< Get channels client is in.
};



/** The aConnection typedef is prefered as a type for connections.
 * @see aServerConnection
 */
typedef aServerConnection *aConnection;



enum {
	/** Handle client's ID internally, this means server ignores ID that clients send
	 * and instead sends the client a generated ID. */
	SERVER_ST_INTERNAL_CLIENT_ID		= 0x0100,
	/** If set encrypts and decrypts all messages. */
	SERVER_ST_ENCRYPT_MESSAGES			= 0x0200,
};





/** A server class for TCP networking.
 *
 * This server can handle a large number of clients and use it's own built in protocol.
 * @see aSocket
 * @see aClient
 */
class aServer : public aSocket {
private:
	static uint32_t id_index;

#ifdef LIBAMANITA_SDL
	size_t setsz;					//!< SDL_SocketSet set, allocated size.
#elif defined __linux__
#endif /* LIBAMANITA_SDL */
	aHashtable _sockets;			//!< All sockets connected to this server. Stored as socket=>aConnection.
	aHashtable _clients;			//!< All clients connected to this server. Stored as ID=>aConnection.
	aServerChannel _main;		//!< Main channel, contains all clients connected to this server.
	aHashtable _channels;		//!< All channels in this server.

	/** Static method called by the thread and in turn calls run().
	 * @see run */
	static void _run(void *p) { ((aServer *)p)->run(); }
	/** Called by the thread, and handles all incomming messages. */
	void run();

	aConnection addClient(tcp_socket_t s,uint8_t *d,size_t l);
#ifdef LIBAMANITA_SDL
	void createSocketSet(int n=0);
#elif defined __linux__
#endif /* LIBAMANITA_SDL */
	bool uniqueID(uint32_t id) { return _clients.get(id)==0; }
	void killClient(uint32_t id) { killClient((aConnection)_clients.get(id)); }
	void killClient(const char *nick) { killClient((aConnection)_clients.get(nick)); }
	void killClient(aConnection c);
	void killAllClients();

public:
	/** @name Constructors and Destructors
	 * @{ */
	/** Constructor. */
	aServer(socket_event_handler seh);
	/** Destructor. */
	~aServer();
	/** @} */

	/** @name Status
	 * @{ */
	/** Set status for server. This method only accepts status flags for aServer, not for aSocket.
	 * @param s Status flags.
	 * @param b Set if true, else remove status flags. */
	void setStatus(int s,bool b) { s &= 0xff00,status = (b? (status|s) : (status&~s)); }
	/** @} */

	/** @name Thread
	 * @{ */
	bool start(const char *con);
	bool start(uint16_t p);
	void stop(bool kill=true);
	/** @} */

	/** @name Client
	 * @{ */
	aConnection getClient(uint32_t id) { return (aConnection)_clients.get(id); }
	aConnection getClient(const char *nick) { return (aConnection)_clients.get(nick); }
	size_t clients() { return _sockets.size(); }
	aHashtable::iterator getClients() { return _sockets.iterate(); }
	void changeNick(uint32_t id,const char *nick);
	/** @} */

	/** @name Channel
	 * @{ */
	aChannel createChannel(const char *ch);
	void deleteChannel(const char *ch);
	aChannel getChannel(const char *ch) { return ch && *ch? (aChannel)_channels.get(ch) : 0; }
	void joinChannel(const char *ch,aConnection c) { if(c) joinChannel(createChannel(ch),c); }
	void joinChannel(aChannel ch,aConnection c) { if(ch && c && !ch->contains(c)) { *ch += c;c->joinChannel(ch); } }
	void leaveChannel(const char *ch,aConnection c) { if(c) leaveChannel(getChannel(ch),c); }
	void leaveChannel(aChannel ch,aConnection c);
	size_t channels() { return _channels.size(); }
	aHashtable::iterator getChannels() { return _channels.iterate(); }
	/** @} */

	/** @name Send
	 * @{ */
	int send(aConnection c,uint8_t *d,size_t l);
	void send(uint8_t *d,size_t l) { send(&_main,d,l); }
	void send(aChannel channel,uint8_t *d,size_t l);
	/** @} */
};


#endif /* _LIBAMANITA_NET_SERVER_H */


#ifndef _AMANITA_NET_SERVER_H
#define _AMANITA_NET_SERVER_H

/**
 * @file amanita/net/Server.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <string.h>
#include <amanita/Hashtable.h>
#include <amanita/Vector.h>
#include <amanita/net/Socket.h>


/** Amanita Namespace */
namespace a {


/** Server channel.
 *
 * This class is used by Server to handle channels.
 * @see Server
 * 
 * @ingroup net */
class ServerChannel : public Vector {
private:
	char *name;		//!< Name of channel.

public:
	/** Constructor. */
	ServerChannel(const char *nm) : Vector() { name = strdup(nm); }
	/** Destructor. */
	~ServerChannel() { free(name); }

	/** Get name of channel. */
	const char *getName() { return name; }
};



/** The Channel typedef is prefered as a type for channels.
 * @see ServerChannel
 * 
 * @ingroup net */
typedef ServerChannel *Channel;



/** Status values for ServerConnection. */
enum {
	CON_ACTIVE				= 1,		//!< Is set if connection is active.
	CON_OPERATOR			= 2,		//!< Is set if client has operator access in channel.
};




/** Server connection.
 *
 * This class is used by Server to handle connections to clients. It contains
 * all client data and which channels the client is in.
 * @see Server
 * 
 * @ingroup net */
class ServerConnection {
friend class Server;

private:
	tcp_socket_t sock;			//!< Socket to client.
	uint32_t id;					//!< Client's unique id.
	char *nick;						//!< Client's nick.
	uint8_t status;				//!< Status of connection.
	Vector _channels;			//!< Channels client is in.
	void *data;						//!< User defined data attached to this connection.
#ifndef SOCKET_NOCIPHER
	uint32_t *key;					//!< For encrypted connections, the encryption key.
	size_t keylen;					//!< Length of key.
#endif /*SOCKET_NOCIPHER*/

	/** Constructor.
	 * The constructor can only be called by Server. */
	ServerConnection(tcp_socket_t sock,uint32_t id,const char *n)
			: sock(sock),id(id),status(0),_channels(),data(0)
#ifndef SOCKET_NOCIPHER
					,key(0),keylen(0)
#endif /*SOCKET_NOCIPHER*/
		{ nick = strdup(n); }

	/** Destructor.
	 * The destructor can only be called by Server. */
	~ServerConnection() { free(nick);
#ifndef SOCKET_NOCIPHER
		if(key) free(key);
#endif /*SOCKET_NOCIPHER*/
	}

public:
	/** @name Socket
	 * @{ */
	const tcp_socket_t getSocket() { return sock; }				//!< Get TCP socket.
	/** @} */

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

	void joinChannel(Channel c) { if(c) _channels << c; }	//!< Join a channel.
	void leaveChannel(Channel c) { if(c) _channels >> c; }	//!< Leave a channel.
	Vector &getChannels() { return _channels; }					//!< Get channels client is in.
};



/** The Connection typedef is prefered as a type for connections.
 * @see ServerConnection
 * 
 * @ingroup net */
typedef ServerConnection *Connection;



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
 * @see Socket
 * @see Client
 * 
 * @ingroup net */
class Server : public Socket {
private:
	static uint32_t id_index;

/*#ifdef USE_SDL
	size_t setsz;					//!< SDL_SocketSet set, allocated size.
#elif defined(USE_GTK) || defined(USE_WIN32)*/
	size_t sockets_n,sockets_cap;
	tcp_socket_t *sockets;
//#endif
	Hashtable _sockets;			//!< All sockets connected to this server. Stored as socket=>Connection.
	Hashtable _clients;			//!< All clients connected to this server. Stored as ID=>Connection.
	ServerChannel _main;		//!< Main channel, contains all clients connected to this server.
	Hashtable _channels;		//!< All channels in this server.

	/** Static method called by the thread and in turn calls run().
	 * @see run */
	static void _run(void *p) { ((Server *)p)->run(); }
	/** Called by the thread, and handles all incomming messages. */
	void run();

	Connection addClient(tcp_socket_t s,uint8_t *d,size_t l);
/*#ifdef USE_SDL
	void createSocketSet(int n=0);
#elif defined(USE_GTK) || defined(USE_WIN32)*/
	void addSocket(tcp_socket_t s);
	void removeSocket(tcp_socket_t s);
//#endif
	bool uniqueID(uint32_t id) { return _clients.get(id)==0; }
	void killClient(uint32_t id) { killClient((Connection)_clients.get(id)); }
	void killClient(const char *nick) { killClient((Connection)_clients.get(nick)); }
	void killClient(Connection c);
	void killAllClients();

public:
	/** @name Constructors and Destructors
	 * @{ */
	/** Constructor. */
	Server(socket_event_handler seh);
	/** Destructor. */
	~Server();
	/** @} */

	/** @name Status
	 * @{ */
	/** Set status for server. This method only accepts status flags for Server, not for Socket.
	 * @param s Status flags.
	 * @param b Set if true, else remove status flags. */
	void setStatus(int s,bool b) { s &= 0xff00,status = (b? (status|s) : (status&~s)); }
	/** @} */

	/** @name Thread
	 * @{ */
	bool start(const char *con);
	bool start(uint16_t p);
	void stop(bool kill=false);
	/** @} */

	/** @name Client
	 * @{ */
	Connection getClient(uint32_t id) { return (Connection)_clients.get(id); }
	Connection getClient(const char *nick) { return (Connection)_clients.get(nick); }
	size_t clients() { return _sockets.size(); }
	Hashtable::iterator getClients() { return _sockets.iterate(); }
	void changeNick(uint32_t id,const char *nick);
	/** @} */

	/** @name Channel
	 * @{ */
	Channel createChannel(const char *ch);
	void deleteChannel(const char *ch);
	Channel getChannel(const char *ch) { return ch && *ch? (Channel)_channels.get(ch) : 0; }
	void joinChannel(const char *ch,Connection c) { if(c) joinChannel(createChannel(ch),c); }
	void joinChannel(Channel ch,Connection c) { if(ch && c && !ch->contains(c)) { *ch << c;c->joinChannel(ch); } }
	void leaveChannel(const char *ch,Connection c) { if(c) leaveChannel(getChannel(ch),c); }
	void leaveChannel(Channel ch,Connection c);
	size_t channels() { return _channels.size(); }
	Hashtable::iterator getChannels() { return _channels.iterate(); }
	/** @} */

	/** @name Send
	 * @{ */
	int send(Connection c,uint8_t *d,size_t l);
	void send(uint8_t *d,size_t l) { send(&_main,d,l); }
	void send(Channel channel,uint8_t *d,size_t l);
	/** @} */
};

}; /* namespace a */



#endif /* _AMANITA_NET_SERVER_H */


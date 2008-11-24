#ifndef _LIBAMANITA_SDL_SOCKET_H
#define _LIBAMANITA_SDL_SOCKET_H

#include <stdarg.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_thread.h>


/**
 *
 * Note: By default the socket will send the length of a package previous to the package itself. By setting some
 * defined parameters during compiletime using the -D compile parameter it is possible to include the
 * packagelength in the sent data so to just send one package instead to cut down some bandwidth.
 *
 * TCPSOCK_LENINCL - If this is defined length of the sent data must be included in the packages sent and that
 *		it should be within the first four bytes of the package. This data should be ignored when sending, it will be
 *		handled internally.
 * TCPSOCK_LEN32 - This means the length of the packages sent is 32 bits.
 * TCPSOCK_LEN16 - Default, no need to define.
 * TCPSOCK_OFFSET16 - Must not be set with TCPSOCK_LEN32.
 * TCPSOCK_OFFSET8 - Must not be set with TCPSOCK_LEN32.
 * TCPSOCK_OFFSET0 - Default, no need to define.
 *
 * Example using -DTCPSOCK_LENINCL:
 * void sendHello() {
 * 	struct package { Uint16 len,char data[7] };
 * 	package p = { 0,"Hello!" }; // Note len should not be set, it is done internally.
 * 	socket->send(s,p,sizeof(p)); // 'socket' is an instance of the Socket class, and 's' an instance of TCPsocket
 * }
 *
 */

#ifdef TCPSOCK_LEN32
#	define TCPSOCK_LEN 4
#	define TCPSOCK_TYPE(v) ((Uint32 *)(v))
#	define TCPSOCK_SWAP(v) SDL_SwapBE32(v)
typedef Uint32 TCPsockHeader;
#else /*TCPSOCK_LEN32*/
#	define TCPSOCK_LEN 2
#	define TCPSOCK_TYPE(v) ((Uint16 *)(v))
#	define TCPSOCK_SWAP(v) SDL_SwapBE16(v)
typedef Uint16 TCPsockHeader;
#endif /*TCPSOCK_LEN32*/

#ifdef TCPSOCK_OFFSET16
#	define TCPSOCK_OFFSET 2
#elif defined TCPSOCK_OFFSET8
#	define TCPSOCK_OFFSET 1
#else
#	define TCPSOCK_OFFSET 0
#endif /*TCPSOCK_OFFSET16*/

#ifdef TCPSOCK_LEN32
#	ifdef TCPSOCK_OFFSET16
#		define TCPSOCK_HD 6
#	elif defined TCPSOCK_OFFSET8
#		define TCPSOCK_HD 5
#	else
#		define TCPSOCK_HD 4
#	endif
#else /*TCPSOCK_LEN32*/
#	ifdef TCPSOCK_OFFSET16
#		define TCPSOCK_HD 4
#	elif defined TCPSOCK_OFFSET8
#		define TCPSOCK_HD 3
#	else
#		define TCPSOCK_HD 2
#	endif
#endif /*TCPSOCK_LEN32*/

enum {
	SM_ERR_RESOLVE_HOST			= 0x0011,
	SM_RESOLVE_HOST				= 0x0021,
	SM_ERR_OPEN_SOCKET			= 0x0031,
	SM_ERR_ADD_SOCKET				= 0x0032,
	SM_STARTING_SERVER			= 0x0041,
	SM_STOPPING_SERVER			= 0x0042,
	SM_STARTING_CLIENT			= 0x0043,
	SM_STOPPING_CLIENT			= 0x0044,
	SM_ERR_ALLOC_SOCKETSET		= 0x0051,
	SM_ERR_CHECK_SOCKETS			= 0x0052,
	SM_CHECK_NICK					= 0x0061,
	SM_DUPLICATE_ID				= 0x0063,
	SM_ADD_CLIENT					= 0x0071,
	SM_KILL_CLIENT					= 0x0072,
	SM_GET_MESSAGE					= 0x0081,
	SM_ERR_GET_MESSAGE			= 0x0091,
	SM_ERR_PUT_MESSAGE			= 0x0092,
};

enum {
	SOCK_ST_RUNNING				= 0x0001,
	SOCK_ST_STARTING,
};

class Socket;

typedef Uint32 (*SocketListener)(Socket *,Uint32,Uint32,Uint32,Uint32);

class Socket {
private:
	SocketListener listener;
	void *buf;
	Uint32 len;

protected:
	SDL_Thread *handle;
	int status;
	IPaddress ip;
	const char *host;
	TCPsocket sock;
	SDLNet_SocketSet set;

	void setRunning(bool b) { status |= SOCK_ST_RUNNING;if(!b) status ^= SOCK_ST_RUNNING; }
	void setStarting(bool b) { status |= SOCK_ST_STARTING;if(!b) status ^= SOCK_ST_STARTING; }

	Uint32 stateChanged(Uint32 state,Uint32 p1,Uint32 p2,Uint32 p3) { return (*listener)(this,state,p1,p2,p3); }
	void *receive(TCPsocket s,Sint32 &n);
	void releaseMessageBuffer(void *b) { if(b!=buf) free(b); }

public:
	Socket(SocketListener l);
	~Socket();

	static void resolveConnection(const char *con,Uint32 &ip,Uint16 &port,Uint32 &id,char *nick,int nlen);
#ifndef TCPSOCK_NOCIPHER
	static void XORcipher(char *d,const char *s,Uint32 l,const Uint32 *k,int kl);
#endif /*TCPSOCK_NOCIPHER*/

	bool isRunning() { return (status&SOCK_ST_RUNNING); }
	bool isStarting() { return (status&SOCK_ST_STARTING); }

	Uint32 getIP() { return ip.host; }
	Uint32 getPort() { return ip.port; }
	const char *getHost() { return host; }

	void setMessageBuffer(Uint32 n);

	int send(TCPsocket s,void *d,Uint32 l);
};


#endif /* _LIBAMANITA_SDL_SOCKET_H */


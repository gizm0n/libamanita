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
 */

#ifdef TCPSOCK_LEN32
#	define TCPSOCK_LEN 4
#	define TCPSOCK_TYPE(v) ((uint32_t *)(v))
#	define TCPSOCK_SWAP(v) SDL_SwapBE32(((uint32_t)(v)))
typedef uint32_t TCPsockHeader;
#else /*TCPSOCK_LEN32*/
#	define TCPSOCK_LEN 2
#	define TCPSOCK_TYPE(v) ((uint16_t *)(v))
#	define TCPSOCK_SWAP(v) SDL_SwapBE16(((uint16_t)(v)))
typedef uint16_t TCPsockHeader;
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

#define pack_int8(data,i) {*(int8_t *)(data) = (int8_t)(i);(data)++;}
#define unpack_int8(data,i) {(i) = *(int8_t *)(data);(data)++;}
#define get_unpack_int8(data) (*(int8_t *)(data))

#define pack_uint8(data,i) {*(uint8_t *)(data) = (uint8_t)(i);(data)++;}
#define unpack_uint8(data,i) {(i) = *(uint8_t *)(data);(data)++;}
#define get_unpack_uint8(data) (*(uint8_t *)(data))

#define pack_int16(data,i) {*(int16_t *)(data) = SDL_SwapBE16((int16_t)(i));(data) += 2;}
#define unpack_int16(data,i) {(i) = SDL_SwapBE16(*(int16_t *)(data));(data) += 2;}
#define get_unpack_int16(data) (int16_t)SDL_SwapBE16(*(int16_t *)(data))

#define pack_uint16(data,i) {*(uint16_t *)(data) = SDL_SwapBE16((uint16_t)(i));(data) += 2;}
#define unpack_uint16(data,i) {(i) = SDL_SwapBE16(*(uint16_t *)(data));(data) += 2;}
#define get_unpack_uint16(data) (uint16_t)SDL_SwapBE16(*(uint16_t *)(data))

#define pack_int32(data,i) {*(int32_t *)(data) = SDL_SwapBE32((int32_t)(i));(data) += 4;}
#define unpack_int32(data,i) {(i) = SDL_SwapBE32(*(int32_t *)(data));(data) += 4;}
#define get_unpack_int32(data) (int32_t)SDL_SwapBE32(*(int32_t *)(data))

#define pack_uint32(data,i) {*(uint32_t *)(data) = SDL_SwapBE32((uint32_t)(i));(data) += 4;}
#define unpack_uint32(data,i) {(i) = SDL_SwapBE32(*(uint32_t *)(data));(data) += 4;}
#define get_unpack_uint32(data) (uint32_t)SDL_SwapBE32(*(uint32_t *)(data))

#define pack_int64(data,i) {*(int64_t *)(data) = SDL_SwapBE64((int64_t)(i));(data) += 8;}
#define unpack_int64(data,i) {(i) = SDL_SwapBE64(*(int64_t *)(data));(data) += 8;}
#define get_unpack_int64(data) (int64_t)SDL_SwapBE64(*(int64_t *)(data))

#define pack_uint64(data,i) {*(uint64_t *)(data) = SDL_SwapBE64((uint64_t)(i));(data) += 8;}
#define unpack_uint64(data,i) {(i) = SDL_SwapBE64(*(uint64_t *)(data));(data) += 8;}
#define get_unpack_uint64(data) (uint64_t)SDL_SwapBE64(*(uint64_t *)(data))

class Socket;

typedef uint32_t (*SocketListener)(Socket *,uint32_t,intptr_t,intptr_t,intptr_t);

class Socket {
private:
	SocketListener listener;
	uint8_t *buf;
	size_t len;

protected:
	SDL_Thread *handle;
	int status;
	IPaddress ip;
	const char *host;
	TCPsocket sock;
	SDLNet_SocketSet set;

	void setRunning(bool b) { status |= SOCK_ST_RUNNING;if(!b) status ^= SOCK_ST_RUNNING; }
	void setStarting(bool b) { status |= SOCK_ST_STARTING;if(!b) status ^= SOCK_ST_STARTING; }

	uint32_t stateChanged(uint32_t state,intptr_t p1,intptr_t p2,intptr_t p3) { return (*listener)(this,state,p1,p2,p3); }
	uint8_t *receive(TCPsocket s,size_t &l);
	void releaseMessageBuffer(uint8_t *b) { if(b!=buf) free(b); }

public:
	Socket(SocketListener l);
	~Socket();

	static void resolveConnection(const char *con,uint32_t &ip,uint16_t &port,uint32_t &id,char *nick,int nlen);
#ifndef TCPSOCK_NOCIPHER
	static void XORcipher(uint8_t *d,const uint8_t *s,size_t l,const uint32_t *k,size_t kl);
#endif /*TCPSOCK_NOCIPHER*/

	bool isRunning() { return (status&SOCK_ST_RUNNING); }
	bool isStarting() { return (status&SOCK_ST_STARTING); }

	uint32_t getIP() { return ip.host; }
	uint32_t getPort() { return ip.port; }
	const char *getHost() { return host; }

	void setMessageBuffer(size_t l);

	size_t send(TCPsocket s,uint8_t *d,size_t l);
};


#endif /* _LIBAMANITA_SDL_SOCKET_H */


#ifndef _LIBAMANITA_NET_SOCKET_H
#define _LIBAMANITA_NET_SOCKET_H

/**
 * @file libamanita/net/aSocket.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <stdarg.h>
#include <stdint.h>

#ifdef LIBAMANITA_SDL
	#include <SDL/SDL.h>
	#include <SDL/SDL_net.h>
	#include <SDL/SDL_thread.h>

	#define tcp_send(s,d,l) SDLNet_TCP_Send((s),(d),(l))
	#define tcp_recv(s,d,l) SDLNet_TCP_Recv((s),(d),(l))

	typedef TCPsocket tcp_socket_t;
	typedef int thread_func_t;

#elif defined __linux__
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <pthread.h>
	#include <signal.h>
	#include <endian.h>

	#define tcp_send(s,d,l) ::send((s),(d),(l),0)
	#define tcp_recv(s,d,l) ::recv((s),(d),(l),0)

	typedef int tcp_socket_t;
	typedef void *thread_func_t;

#endif /* LIBAMANITA_SDL */

#undef swap_be_16
#undef swap_be_32
#undef swap_be_64

#ifdef LIBAMANITA_SDL
	#define swap_be_16 SDL_SwapBE16
	#define swap_be_32 SDL_SwapBE32
	#define swap_be_64 SDL_SwapBE64

#elif __BYTE_ORDER == __BIG_ENDIAN
	#define swap_be_16(n) n
	#define swap_be_32(n) n
	#define swap_be_64(n) n

#else
	#include <byteswap.h>
	#define swap_be_16(n) bswap_16(n)
	#define swap_be_32(n) bswap_32(n)
	#define swap_be_64(n) bswap_64(n)
#endif /* LIBAMANITA_SDL */



/**
 *
 * Note: By default the socket will send the length of a package previous to the package itself. By setting some
 * defined parameters during compiletime using the -D compile parameter it is possible to include the
 * packagelength in the sent data so to just send one package instead to cut down some bandwidth.
 *
 * SOCKET_LENINCL - If this is defined length of the sent data must be included in the packages sent and that
 *		it should be within the first four bytes of the package. This data should be ignored when sending, it will be
 *		handled internally.
 * SOCKET_LEN==32 - This means the length of the packages sent is 32 bits.
 * SOCKET_LEN==16 - Default, no need to define.
 * SOCKET_OFFSET==16 - Must not be set with SOCKET_LEN==32.
 * SOCKET_OFFSET==8 - Must not be set with SOCKET_LEN==32.
 * SOCKET_OFFSET==0 - Default, no need to define.
 *
 */


#undef SOCKET_LENINCL
#undef SOCKET_LEN
#undef SOCKET_OFFSET
#undef SOCKET_NOCIPHER
#undef SOCKET_HD

#ifdef LIBAMANITA_SOCKET_LENINCL
	#define SOCKET_LENINCL
#endif

#if LIBAMANITA_SOCKET_LEN==32
	#define SOCKET_LEN 4
	#define SOCKET_TYPE(v) ((uint32_t *)(v))
	#define SOCKET_SWAP(v) swap_be_32(((uint32_t)(v)))
	typedef uint32_t socket_header_t;
#else /* LIBAMANITA_SOCKET_LEN */
	#define SOCKET_LEN 2
	#define SOCKET_TYPE(v) ((uint16_t *)(v))
	#define SOCKET_SWAP(v) swap_be_16(((uint16_t)(v)))
	typedef uint16_t socket_header_t;
#endif /* LIBAMANITA_SOCKET_LEN */

#if LIBAMANITA_SOCKET_OFFSET==16
	#define SOCKET_OFFSET 2
#elif LIBAMANITA_SOCKET_OFFSET==8
	#define SOCKET_OFFSET 1
#else
	#define SOCKET_OFFSET 0
#endif /* SOCKET_OFFSET16 */

#if LIBAMANITA_SOCKET_LEN==32
	#if LIBAMANITA_SOCKET_OFFSET==16
		#define SOCKET_HD 6
	#elif LIBAMANITA_SOCKET_OFFSET==8
		#define SOCKET_HD 5
	#else
		#define SOCKET_HD 4
	#endif
#else /* LIBAMANITA_SOCKET_LEN */
	#if LIBAMANITA_SOCKET_OFFSET==16
		#define SOCKET_HD 4
	#elif LIBAMANITA_SOCKET_OFFSET==8
		#define SOCKET_HD 3
	#else
		#define SOCKET_HD 2
	#endif
#endif /* LIBAMANITA_SOCKET_LEN */


#ifdef LIBAMANITA_SOCKET_NOCIPHER
	#define SOCKET_NOCIPHER
#endif


enum {
	SM_ERR_RESOLVE_HOST			= 0x0011,
	SM_RESOLVE_HOST				= 0x0021,
	SM_ERR_OPEN_SOCKET			= 0x0031,
	SM_ERR_CONNECT					= 0x0032,
	SM_ERR_BIND						= 0x0033,
	SM_ERR_LISTEN					= 0x0034,
	SM_ERR_ADD_SOCKET				= 0x0035,
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


inline void pack_int8(uint8_t **data,int8_t i) { *(int8_t *)((void *)*data) = i;*data++; }
inline void unpack_int8(uint8_t **data,int8_t &i) { i = *(int8_t *)((void *)data);*data++; }
inline int8_t get_unpack_int8(uint8_t *data) { return *(int8_t *)((void *)data); }

inline void pack_uint8(uint8_t **data,uint8_t i) { *(uint8_t *)((void *)*data) = i;*data++; }
inline void unpack_uint8(uint8_t **data,uint8_t &i) { i = *(uint8_t *)((void *)data);*data++; }
inline uint8_t get_unpack_uint8(uint8_t *data) { return *(uint8_t *)((void *)data); }

inline void pack_int16(uint8_t **data,int16_t i) { *(int16_t *)((void *)*data) = i;*data += 2; }
inline void unpack_int16(uint8_t **data,int16_t &i) { i = *(int16_t *)((void *)data);*data += 2; }
inline int16_t get_unpack_int16(uint8_t *data) { return *(int16_t *)((void *)data); }

inline void pack_uint16(uint8_t **data,uint16_t i) { *(uint16_t *)((void *)*data) = i;*data += 2; }
inline void unpack_uint16(uint8_t **data,uint16_t &i) { i = *(uint16_t *)((void *)data);*data += 2; }
inline uint16_t get_unpack_uint16(uint8_t *data) { return *(uint16_t *)((void *)data); }

inline void pack_int32(uint8_t **data,int32_t i) { *(int32_t *)((void *)*data) = i;*data += 4; }
inline void unpack_int32(uint8_t **data,int32_t &i) { i = *(int32_t *)((void *)data);*data += 4; }
inline int32_t get_unpack_int32(uint8_t *data) { return *(int32_t *)((void *)data); }

inline void pack_uint32(uint8_t **data,uint32_t i) { *(uint32_t *)((void *)*data) = i;*data += 4; }
inline void unpack_uint32(uint8_t **data,uint32_t &i) { i = *(uint32_t *)((void *)data);*data += 4; }
inline uint32_t get_unpack_uint32(uint8_t *data) { return *(uint32_t *)((void *)data); }

inline void pack_int64(uint8_t **data,int64_t i) { *(int64_t *)((void *)*data) = i;*data += 8; }
inline void unpack_int64(uint8_t **data,int64_t &i) { i = *(int64_t *)((void *)data);*data += 8; }
inline int64_t get_unpack_int64(uint8_t *data) { return *(int64_t *)((void *)data); }

inline void pack_uint64(uint8_t **data,uint64_t i) { *(uint64_t *)((void *)*data) = i;*data += 8; }
inline void unpack_uint64(uint8_t **data,uint64_t &i) { i = *(uint64_t *)((void *)data);*data += 8; }
inline uint64_t get_unpack_uint64(uint8_t *data) { return *(uint64_t *)((void *)data); }


class aSocket;

/** Callback function used by aSocket to handle events.
 *
 * This function is one of the most important in this interface, because it makes it possible
 * for the program to intercept all communication over the network. It also sends error messages
 * and warnings.
 */
typedef uint32_t (*socket_event_handler)(aSocket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3);



/** TCP sockets class.
 *
 * This class can handle TCP sockets but should not be instantiated in itself, and should be
 * considered abstract. Instead, use it's subclasses; aClient and aServer.
 * @see aClient
 * @see aServer
 */
class aSocket {
private:
	socket_event_handler event_handler;
	uint8_t *buf;
	size_t len;

protected:
	char *host;
	int status;
	tcp_socket_t sock;
	uint32_t ip;
	uint16_t port;
#ifdef LIBAMANITA_SDL
	SDL_Thread *thread;
	IPaddress address;
	SDLNet_SocketSet set;
#elif defined __linux__
	pthread_t thread;
	fd_set set;
	hostent *hostinfo;
	sockaddr_in address;
#endif /* LIBAMANITA_SDL */

	void setRunning(bool b) { status |= SOCK_ST_RUNNING;if(!b) status ^= SOCK_ST_RUNNING; }
	void setStarting(bool b) { status |= SOCK_ST_STARTING;if(!b) status ^= SOCK_ST_STARTING; }

	uint32_t stateChanged(uint32_t state,intptr_t p1,intptr_t p2,intptr_t p3) { return (*event_handler)(this,state,p1,p2,p3); }
	uint8_t *receive(tcp_socket_t s,size_t &l);
	void releaseMessageBuffer(uint8_t *b) { if(b!=buf) free(b); }

public:
	aSocket(socket_event_handler seh);
	~aSocket();

	static void resolveConnection(const char *con,uint32_t &ip,uint16_t &port,uint32_t &id,char *nick,int nlen);
#ifndef SOCKET_NOCIPHER
	static void XORcipher(uint8_t *d,const uint8_t *s,size_t l,const uint32_t *k,size_t kl);
#endif /*SOCKET_NOCIPHER*/

	bool isRunning() { return (status&SOCK_ST_RUNNING); }
	bool isStarting() { return (status&SOCK_ST_STARTING); }

	uint32_t getIP() { return ip; }
	uint16_t getPort() { return port; }
	const char *getHost() { return host; }

	void setMessageBuffer(size_t l);

	size_t send(tcp_socket_t s,uint8_t *d,size_t l);
};


#endif /* _LIBAMANITA_NET_SOCKET_H */


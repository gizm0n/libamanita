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
#include <stdlib.h>

/** Set LIBAMANITA_SOCKET_HEADER to the sizeof the header in number of bytes.
 * Is can be 2 to 8 bytes and defines the size of the length parameter in the
 * header and the offset the lenth parameter is placed. I.e. if it's set to
 * 3, the length parameter is placed 1 byte in and has the size of 16 bits.
 * For the value of 6 the length parameter will be 32 bits and positioned 2
 * bytes in. This is the configuration for the different sizes of the header
 * where '.'=offset and '*'=length parameter.
 * 2: [**]
 * 3: [.**] <- 1 byte for command, and 2 bytes tell the size of the packet.
 * 4: [..**]
 * 5: [.****]
 * 6: [..****] <- 2 byte for command, and 4 bytes tell the size of the packet.
 * 7: [...****]
 * 8: [....****]
 */
#define LIBAMANITA_SOCKET_HEADER 3

/** Uncomment this if you don't want to include the minor overhead
 * and extra memory of encryption in messages, or if encryption is
 * handled elsewhere.
 */
#if 0
#define LIBAMANITA_NOCIPHER
#endif


#ifdef LIBAMANITA_SDL
	#include <SDL/SDL.h>
	#include <SDL/SDL_net.h>
	#include <SDL/SDL_thread.h>

	#define tcp_close(s) SDLNet_TCP_Close(s)
	#define tcp_send(s,d,l) SDLNet_TCP_Send((s),(d),(l))
	#define tcp_recv(s,d,l) SDLNet_TCP_Recv((s),(d),(l))

	typedef TCPsocket tcp_socket_t;
	typedef int thread_func_t;
	typedef SDL_mutex *socket_mutex_t;

#elif defined __linux__
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <pthread.h>
	#include <signal.h>
	#include <endian.h>

	#define tcp_close(s) ::close(s)
	#define tcp_send(s,d,l) ::send((s),(d),(l),0)
	#define tcp_recv(s,d,l) ::recv((s),(d),(l),0)

	typedef int tcp_socket_t;
	typedef void *thread_func_t;
	typedef pthread_mutex_t *socket_mutex_t;

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
 * SOCKET_HEADER_INCLUDED - If this is defined length of the sent data must be included in the packages sent and that
 *		it should be within the first four bytes of the package. This data should be ignored when sending, it will be
 *		handled internally.
 * SOCKET_LEN - Number of bytes the length parameter in the header is. Can be 2 or 4. Default is 2.
 * SOCKET_OFFSET - Number of bytes before the length parameter in the header. Default is 0.
 * SOCKET_HEADER - Number of bytes the header is. Accept 2 to 8 bytes, other values are ignored.
 */


#undef SOCKET_HEADER_INCLUDED
#undef SOCKET_HEADER
#undef SOCKET_LEN
#undef SOCKET_OFFSET
#undef SOCKET_NOCIPHER

#if LIBAMANITA_SOCKET_HEADER>=3 && LIBAMANITA_SOCKET_HEADER<8
	#define SOCKET_HEADER_INCLUDED
	#define SOCKET_HEADER LIBAMANITA_SOCKET_HEADER
	#if SOCKET_HEADER<=4
		#define SOCKET_LEN 2
		#if SOCKET_HEADER==3
			#define SOCKET_OFFSET 1
		#else
			#define SOCKET_OFFSET 2
		#endif
	#else
		#define SOCKET_LEN 4
		#if SOCKET_HEADER==5
			#define SOCKET_OFFSET 1
		#elif SOCKET_HEADER==6
			#define SOCKET_OFFSET 2
		#elif SOCKET_HEADER==7
			#define SOCKET_OFFSET 3
		#else
			#define SOCKET_OFFSET 4
		#endif
	#endif
#else
	#define SOCKET_LEN 2
	#define SOCKET_OFFSET 0
	#define SOCKET_HEADER 2
#endif

#if SOCKET_OFFSET>=4
	#define SOCKET_HEADER_CMD(v) swap_be_32(*(uint32_t *)(v))
#elif SOCKET_OFFSET>=2
	#define SOCKET_HEADER_CMD(v) swap_be_16(*(uint16_t *)(v))
#elif SOCKET_OFFSET==1
	#define SOCKET_HEADER_CMD(v) (*(uint8_t *)(v))
#else
	#define SOCKET_HEADER_CMD(v) 0
#endif

#if SOCKET_LEN==4
	#define SOCKET_HEADER_LEN_TYPE(v) ((uint32_t *)(v))
	#define SOCKET_HEADER_LEN_SWAP(v) swap_be_32(((uint32_t)(v)))
	typedef uint32_t socket_header_len_t;
#else
	#define SOCKET_HEADER_LEN_TYPE(v) ((uint16_t *)(v))
	#define SOCKET_HEADER_LEN_SWAP(v) swap_be_16(((uint16_t)(v)))
	typedef uint16_t socket_header_len_t;
#endif

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
	SOCK_ST_ALIEN_MUTEX			= 0x0001,
	SOCK_ST_RUNNING,
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
	socket_mutex_t mut;				//!< aSocket mutex.
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

	/** @name Thread
	 * @{ */
	void setMutex(socket_mutex_t m);
	void createMutex();
	void deleteMutex();
	int lock();				//!< Lock the mutext.
	int unlock();			//!< Unlock the mutext.
	bool isRunning() { return (status&SOCK_ST_RUNNING); }
	bool isStarting() { return (status&SOCK_ST_STARTING); }
	/** @} */

	uint32_t getIP() { return ip; }
	uint16_t getPort() { return port; }
	const char *getHost() { return host; }
	bool isLocalSocket(tcp_socket_t s) { return sock==s; }

	void setMessageBuffer(size_t l);
	static int getPacketHeaderSize() { return SOCKET_HEADER; }
	static int getPacketHeader(uint8_t *data) { return SOCKET_HEADER_CMD(data); }
	static int getPacketSize(uint8_t *data) { return SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(data+SOCKET_OFFSET)); }

	size_t send(tcp_socket_t s,uint8_t *d,size_t l);
};


#endif /* _LIBAMANITA_NET_SOCKET_H */


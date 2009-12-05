#ifndef _LIBAMANITA_NET_SOCKET_H
#define _LIBAMANITA_NET_SOCKET_H

/**
 * @file libamanita/net/aSocket.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <stdarg.h>

#ifdef LIBAMANITA_SDL
	#include <SDL/SDL.h>
	#include <SDL/SDL_net.h>
	#include <SDL/SDL_thread.h>

	#define bswap_16 SDL_SwapBE16
	#define bswap_32 SDL_SwapBE32
	#define bswap_64 SDL_SwapBE64

#else /* LIBAMANITA_SDL */
	#include <byteswap.h>
#endif

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
	#define TCPSOCK_LEN 4
	#define TCPSOCK_TYPE(v) ((uint32_t *)(v))
	#define TCPSOCK_SWAP(v) bswap_32(((uint32_t)(v)))
	typedef uint32_t TCPsockHeader;
#else /* TCPSOCK_LEN32 */
	#define TCPSOCK_LEN 2
	#define TCPSOCK_TYPE(v) ((uint16_t *)(v))
	#define TCPSOCK_SWAP(v) bswap_16(((uint16_t)(v)))
	typedef uint16_t TCPsockHeader;
#endif /* TCPSOCK_LEN32 */

#ifdef TCPSOCK_OFFSET16
	#define TCPSOCK_OFFSET 2
#elif defined TCPSOCK_OFFSET8
	#define TCPSOCK_OFFSET 1
#else
	#define TCPSOCK_OFFSET 0
#endif /* TCPSOCK_OFFSET16 */

#ifdef TCPSOCK_LEN32
	#ifdef TCPSOCK_OFFSET16
		#define TCPSOCK_HD 6
	#elif defined TCPSOCK_OFFSET8
		#define TCPSOCK_HD 5
	#else
		#define TCPSOCK_HD 4
	#endif
#else /* TCPSOCK_LEN32 */
	#ifdef TCPSOCK_OFFSET16
		#define TCPSOCK_HD 4
	#elif defined TCPSOCK_OFFSET8
		#define TCPSOCK_HD 3
	#else
		#define TCPSOCK_HD 2
	#endif
#endif /* TCPSOCK_LEN32 */

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

typedef uint32_t (*SocketListener)(aSocket *,uint32_t,intptr_t,intptr_t,intptr_t);

class aSocket {
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
	aSocket(SocketListener l);
	~aSocket();

	static void resolveConnection(const char *con,uint32_t &ip,uint16_t &port,uint32_t &id,char *nick,int nlen);
#ifndef TCPSOCK_NOCIPHER
	static void XORcipher(uint8_t *d,const uint8_t *s,size_t l,const uint32_t *k,size_t kl);
#endif /* TCPSOCK_NOCIPHER */

	bool isRunning() { return (status&SOCK_ST_RUNNING); }
	bool isStarting() { return (status&SOCK_ST_STARTING); }

	uint32_t getIP() { return ip.host; }
	uint32_t getPort() { return ip.port; }
	const char *getHost() { return host; }

	void setMessageBuffer(size_t l);

	size_t send(TCPsocket s,uint8_t *d,size_t l);
};


#endif /* _LIBAMANITA_NET_SOCKET_H */


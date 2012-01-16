
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <amanita/net/aSocket.h>



const char *aSocket::message_names[] = {0,
	"SM_ERR_RESOLVE_HOST",
	"SM_ERR_OPEN_SOCKET",
	"SM_ERR_CONNECT",
	"SM_ERR_BIND",
	"SM_ERR_LISTEN",
	"SM_ERR_ADD_SOCKET",
	"SM_ERR_ALLOC_SOCKETSET",
	"SM_ERR_CHECK_SOCKETS",
	"SM_ERR_GET_MESSAGE",
	"SM_ERR_PUT_MESSAGE",

	"SM_RESOLVE_HOST",
	"SM_STARTING_SERVER",
	"SM_STOPPING_SERVER",
	"SM_STARTING_CLIENT",
	"SM_STOPPING_CLIENT",
	"SM_CHECK_NICK",
	"SM_DUPLICATE_ID",
	"SM_ADD_CLIENT",
	"SM_KILL_CLIENT",
	"SM_GET_MESSAGE",
};



void aSocket::init() {
#ifdef LIBAMANITA_SDL
#elif defined(WIN32)
	WSADATA wsadata;
	if(WSAStartup(MAKEWORD(2,0),&wsadata)!=0) {
debug_output("Socket Initialization Error.\n");
		WSACleanup();
	}
#endif
}

void aSocket::close() {
#ifdef LIBAMANITA_SDL
#elif defined(WIN32)
	WSACleanup();
#endif
}



void aSocket::print_packet(const uint8_t *data,size_t l) {
	fputc('[',stderr);
	for(int n=0,n1,n2; n<SOCKET_HEADER; n++) {
		n2 = data[n],n1 = n2>>4,n2 &= 0xf;
		fputc(n1<10? '0'+n1 : 'A'+n1-10,stderr);
		fputc(n2<10? '0'+n2 : 'A'+n2-10,stderr);
	}
	fputc(']',stderr);
	fputc('[',stderr);
	for(int n=SOCKET_HEADER,n1,n2; n<(int)l && n<32; n++) {
		n2 = data[n];
		if(n2>=32 && n2<=127) fputc(n2,stderr);
		else {
			n1 = n2>>4,n2 &= 0xf;
			fputc(n1<10? '0'+n1 : 'A'+n1-10,stderr);
			fputc(n2<10? '0'+n2 : 'A'+n2-10,stderr);
		}
	}
	fputc(']',stderr);
}


aSocket::aSocket(socket_event_handler seh) : event_handler(seh) {
	host = 0;
	status = 0;
	sock = 0;
	ip = 0;
	port = 0;
#ifdef LIBAMANITA_SDL
	address = (IPaddress){0,0};
	set = 0;
#elif defined(__linux__) || defined(WIN32)
	hostinfo = 0;
#endif
	buf = 0;
	len = 0;
	setMessageBuffer(1024);
}

aSocket::~aSocket() {
	if(host) { free(host);host = 0; }
#ifdef LIBAMANITA_SDL
	if(set) { SDLNet_FreeSocketSet(set);set = 0; }
#endif
	if(buf) { free(buf);buf = 0,len = 0; }
}

void aSocket::resolveConnection(const char *con,uint32_t &ip,uint16_t &port,uint32_t &id,char *nick,int nlen) {
	ip = 0,port = 0,id = 0,*nick = '\0';
	int n;
	uint32_t i = 0,d = 0;
	uint16_t p = 0;
	if(!con || !*con) return;
	char buf[strlen(con)+1];
	strcpy(buf,con);
	char *p1 = buf,*p2;
	for(n=0; p1; n++,p1=p2+1) {
		p2 = strpbrk(p1,".: |");
		if(p2) *p2 = '\0';
		if(!p1) continue;
		if(n<4) i = (i<<8)|atoi(p1);
		else if(n==4) p = atoi(p1);
		else if(n==5) d = atoi(p1);
		else if(n==6) {
			strncpy(nick,p1,nlen-1);
			break;
		}
	}
	ip = i;
	port = p;
	id = d;
debug_output("aSocket::resolveConnection(ip=%x,port=%d,id=%d)\n",ip,port,id);
}

void aSocket::setMessageBuffer(size_t l) {
	if(isRunning() || isStarting()) return;
	if(buf) free(buf);
	buf = (uint8_t *)malloc(l),len = l;
}

uint8_t *aSocket::receive(tcp_socket_t s,size_t &l) {
	uint8_t *b = buf;
	int r;
	size_t i = 0;
	do {
		r = tcp_recv(s,b+i,SOCKET_HEADER-i);
		if(r>0) i += r;
	} while(r>0 && i<SOCKET_HEADER);

debug_output("aSocket::receive(0,r=%d,cmd=%d,len=%d)\n",
		r,(int)*b,SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(b+SOCKET_OFFSET)));

	if(i==SOCKET_HEADER &&
			(i=SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(b+SOCKET_OFFSET)))>0) {
debug_output("aSocket::receive(1,r=%d,i=%lu)\n",r,(unsigned long)i);
#ifdef SOCKET_HEADER_INCLUDED
		l = SOCKET_HEADER;
		if(i>len) {
			b = (uint8_t *)malloc(i);
			memcpy(b,buf,SOCKET_HEADER);
		}
#else /*SOCKET_HEADER_INCLUDED*/
		l = 0;
		if((uint32_t)i>len) b = (char *)malloc(i);
#endif /*SOCKET_HEADER_INCLUDED*/
		while(r>0 && l<i) {
			r = tcp_recv(s,b+l,i-l);
			l += r;
debug_output("aSocket::receive(3,r=%d,l=%lu)\n",r,(unsigned long)l);
		}
		if(l==i) {
debug_output("aSocket::receive(");
#ifdef LIBAMANITA_DEBUG
print_packet(b,l);
#endif
debug_output(")\n");
			return b;
		}
		if(r==-1) {
debug_output("aSocket::receive(error=%s)\n",getError());
		}
		if(b!=buf) free(b);
	}
//	stateChanged(SM_ERR_GET_MESSAGE,0,0,0);
	l = 0;
	return 0;
}

#ifdef LIBAMANITA_SDL
#elif defined(WIN32)
const char *getError() {
	static struct wsa_error_entry {
		int id;
		const char *msg;
	} wsa_error_list[] = {
		{ 0,									"No error" },										// 0
		{ WSAEINTR,							"Interrupted system call" },					// 4
		{ WSAEBADF,							"Bad file number" },								// 9
		{ WSAEACCES,						"Permission denied" },							// 13
		{ WSAEFAULT,						"Bad address" },									// 14
		{ WSAEINVAL,						"Invalid argument" },							// 22
		{ WSAEMFILE,						"Too many open sockets" },						// 24
		{ WSAEWOULDBLOCK,					"Operation would block" },						// 35
		{ WSAEINPROGRESS,					"Operation now in progress" },				// 36
		{ WSAEALREADY,						"Operation already in progress" },			// 37
		{ WSAENOTSOCK,						"Socket operation on non-socket" },			// 38
		{ WSAEDESTADDRREQ,				"Destination address required" },			// 39
		{ WSAEMSGSIZE,						"Message too long" },							// 40
		{ WSAEPROTOTYPE,					"Protocol wrong type for socket" },			// 41
		{ WSAENOPROTOOPT,					"Bad protocol option" },						// 42
		{ WSAEPROTONOSUPPORT,			"Protocol not supported" },					// 43
		{ WSAESOCKTNOSUPPORT,			"Socket type not supported" },				// 44
		{ WSAEOPNOTSUPP,					"Operation not supported on socket" },		// 45
		{ WSAEPFNOSUPPORT,				"Protocol family not supported" },			// 46
		{ WSAEAFNOSUPPORT,				"Address family not supported" },			// 47
		{ WSAEADDRINUSE,					"Address already in use" },					// 48
		{ WSAEADDRNOTAVAIL,				"Can't assign requested address" },			// 49
		{ WSAENETDOWN,						"Network is down" },								// 50
		{ WSAENETUNREACH,					"Network is unreachable" },					// 51
		{ WSAENETRESET,					"Net connection reset" },						// 52
		{ WSAECONNABORTED,				"Software caused connection abort" },		// 53
		{ WSAECONNRESET,					"Connection reset by peer" },					// 54
		{ WSAENOBUFS,						"No buffer space available" },				// 55
		{ WSAEISCONN,						"Socket is already connected" },				// 56
		{ WSAENOTCONN,						"Socket is not connected" },					// 57
		{ WSAESHUTDOWN,					"Can't send after socket shutdown" },		// 58
		{ WSAETOOMANYREFS,				"Too many references, can't splice" },		// 59
		{ WSAETIMEDOUT,					"Connection timed out" },						// 60
		{ WSAECONNREFUSED,				"Connection refused" },							// 61
		{ WSAELOOP,							"Too many levels of symbolic links" },		// 62
		{ WSAENAMETOOLONG,				"File name too long" },							// 63
		{ WSAEHOSTDOWN,					"Host is down" },									// 64
		{ WSAEHOSTUNREACH,				"No route to host" },							// 65
		{ WSAENOTEMPTY,					"Directory not empty" },						// 66
		{ WSAEPROCLIM,						"Too many processes" },							// 67
		{ WSAEUSERS,						"Too many users" },								// 68
		{ WSAEDQUOT,						"Disc quota exceeded" },						// 69
		{ WSAESTALE,						"Stale NFS file handle" },						// 70
		{ WSAEREMOTE,						"Too many levels of remote in path" },		// 71
		{ WSASYSNOTREADY,					"Network system is unavailable" },			// 91
		{ WSAVERNOTSUPPORTED,			"Winsock version out of range" },			// 92
		{ WSANOTINITIALISED,				"WSAStartup not yet called" },				// 93
		{ WSAEDISCON,						"Graceful shutdown in progress" },			// 101
		{ WSAENOMORE,						"WSAENOMORE" },									// 102
		{ WSAECANCELLED,					"WSAECANCELLED" },								// 103
		{ WSAEINVALIDPROCTABLE,			"WSAEINVALIDPROCTABLE" },						// 104
		{ WSAEINVALIDPROVIDER,			"WSAEINVALIDPROVIDER" },						// 105
		{ WSAEPROVIDERFAILEDINIT,		"WSAEPROVIDERFAILEDINIT" },					// 106
		{ WSASYSCALLFAILURE,				"WSASYSCALLFAILURE" },							// 107
		{ WSASERVICE_NOT_FOUND,			"WSASERVICE_NOT_FOUND" },						// 108
		{ WSATYPE_NOT_FOUND,				"WSATYPE_NOT_FOUND" },							// 109
		{ WSA_E_NO_MORE,					"WSA_E_NO_MORE" },								// 110
		{ WSA_E_CANCELLED,				"WSA_E_CANCELLED" },								// 111
		{ WSAEREFUSED,						"WSAEREFUSED" },									// 112
		{ WSAHOST_NOT_FOUND,				"Host not found" },								// 1001
		{ WSATRY_AGAIN,					"Try again" },										// 1002
		{ WSANO_RECOVERY,					"Host not found" },								// 1003
		{ WSANO_DATA,						"No host data of that type was found" },	// 1004
		{ WSA_QOS_RECEIVERS,				"WSA_QOS_RECEIVERS" },							// 1005
		{ WSA_QOS_SENDERS,				"WSA_QOS_SENDERS" },								// 1006
		{ WSA_QOS_NO_SENDERS,			"WSA_QOS_NO_SENDERS" },							// 1007
		{ WSA_QOS_NO_RECEIVERS,			"WSA_QOS_NO_RECEIVERS" },						// 1008
		{ WSA_QOS_REQUEST_CONFIRMED,	"WSA_QOS_REQUEST_CONFIRMED" },				// 1009
		{ WSA_QOS_ADMISSION_FAILURE,	"WSA_QOS_ADMISSION_FAILURE" },				// 1010
		{ WSA_QOS_POLICY_FAILURE,		"WSA_QOS_POLICY_FAILURE" },					// 1011
		{ WSA_QOS_BAD_STYLE,				"WSA_QOS_BAD_STYLE" },							// 1012
		{ WSA_QOS_BAD_OBJECT,			"WSA_QOS_BAD_OBJECT" },							// 1013
		{ WSA_QOS_TRAFFIC_CTRL_ERROR,	"WSA_QOS_TRAFFIC_CTRL_ERROR" },				// 1014
		{ WSA_QOS_GENERIC_ERROR,		"WSA_QOS_GENERIC_ERROR" },						// 1015
		{ WSA_QOS_ESERVICETYPE,			"WSA_QOS_ESERVICETYPE" },						// 1016
		{ WSA_QOS_EFLOWSPEC,				"WSA_QOS_EFLOWSPEC" },							// 1017
		{ WSA_QOS_EPROVSPECBUF,			"WSA_QOS_EPROVSPECBUF" },						// 1018
		{ WSA_QOS_EFILTERSTYLE,			"WSA_QOS_EFILTERSTYLE" },						// 1019
		{ WSA_QOS_EFILTERTYPE,			"WSA_QOS_EFILTERTYPE" },						// 1020
		{ WSA_QOS_EFILTERCOUNT,			"WSA_QOS_EFILTERCOUNT" },						// 1021
		{ WSA_QOS_EOBJLENGTH,			"WSA_QOS_EOBJLENGTH" },							// 1022
		{ WSA_QOS_EFLOWCOUNT,			"WSA_QOS_EFLOWCOUNT" },							// 1023
		{ WSA_QOS_EUNKOWNPSOBJ,			"WSA_QOS_EUNKOWNPSOBJ" },						// 1024
		{ WSA_QOS_EPOLICYOBJ,			"WSA_QOS_EPOLICYOBJ" },							// 1025
		{ WSA_QOS_EFLOWDESC,				"WSA_QOS_EFLOWDESC" },							// 1026
		{ WSA_QOS_EPSFLOWSPEC,			"WSA_QOS_EPSFLOWSPEC" },						// 1027
		{ WSA_QOS_EPSFILTERSPEC,		"WSA_QOS_EPSFILTERSPEC" },						// 1028
		{ WSA_QOS_ESDMODEOBJ,			"WSA_QOS_ESDMODEOBJ" },							// 1029
		{ WSA_QOS_ESHAPERATEOBJ,		"WSA_QOS_ESHAPERATEOBJ" },						// 1030
		{ WSA_QOS_RESERVED_PETYPE,		"WSA_QOS_RESERVED_PETYPE" }					// 1031
	};
	int id = WSAGetLastError(),i,n = sizeof(wsa_error_list)/sizeof(wsa_error_entry);
	for(i=0; i<n; i++) if(wsa_error_list[i].id==id) break;
	if(i==n) return "Unknown error";
	return wsa_error_list[i].msg;
}
#endif


#ifndef SOCKET_NOCIPHER
void aSocket::XORcipher(uint8_t *d,const uint8_t *s,size_t l,const uint32_t *k,size_t kl) {
	size_t n,i;
	uint32_t *d1 = (uint32_t *)d;
	const uint32_t *s1 = (const uint32_t *)s;
	for(n=0,i=0; l>=4; n++,l-=4,i++) {
		if(i==kl) i = 0;
		d1[n] = s1[n]^k[i];
	}
	if(l>0) {
		if(i==kl) i = 0;
		const uint8_t *k1 = (const uint8_t *)&k[i];
		for(i=0,n<<=2; i<l; i++,n++) d[n] = s[n]^k1[i];
	}
}
#endif /*SOCKET_NOCIPHER*/


size_t aSocket::send(tcp_socket_t s,uint8_t *d,size_t l) {
	if(d && l>0) {
debug_output("aSocket::send(0,s=%p,l=%lu)\n",(void *)s,(unsigned long)l);
#ifdef SOCKET_HEADER_INCLUDED
		int n;
		*SOCKET_HEADER_LEN_TYPE(d+SOCKET_OFFSET) = SOCKET_HEADER_LEN_SWAP(l);
		if((n=tcp_send(s,d,l))==(int)l) return n;
#else /*SOCKET_HEADER_INCLUDED*/
		tcp_socket_header_len_t n = SOCKET_HEADER_LEN_SWAP(l);
		if(tcp_send(s,&n,sizeof(n))==sizeof(n) && (n=tcp_send(s,d,l))==(int)l) return n;
#endif /*SOCKET_HEADER_INCLUDED*/
debug_output("aSocket::send(1,l=%lu)\n",(unsigned long)l);
	}
//	stateChanged(SM_ERR_PUT_MESSAGE,(uint32_t)buf,(uint32_t)l,0);
	return 0;
}




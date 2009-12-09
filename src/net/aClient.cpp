
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libamanita/net/aClient.h>





aClient::aClient(socket_event_handler seh) : aSocket(seh) {
	host = 0;
	port = 0;
	id = 0;
	nick = 0;
#ifndef SOCKET_NOCIPHER
	key = 0;
	keylen = 0;
#endif /*SOCKET_NOCIPHER*/
}

aClient::~aClient() {
	stop();
	if(nick) { free(nick);nick = 0; }
#ifndef SOCKET_NOCIPHER
	if(key) { free(key);key = 0; }
#endif /*SOCKET_NOCIPHER*/
}

bool aClient::start(const char *con) {
	char h[16],n[33];
	uint32_t i,d;
	uint16_t p;
fprintf(stderr,"start()\n");
fflush(stderr);
	resolveConnection(con,i,p,d,n,32);
	sprintf(h,"%d.%d.%d.%d",i>>24,(i>>16)&0xff,(i>>8)&0xff,i&0xff);
fprintf(stderr,"start(\"%s\",%x,%d,%d,%s)\n",h,i,p,d,n);
fflush(stderr);
	return start(h,p,d,n);
}


bool aClient::start(const char *h,uint16_t p,uint32_t i,const char *n) {
fprintf(stderr,"start(\"%s\",%d)\n",h,p);
fflush(stderr);

	if(host) free(host);
	host = strdup(h);
	port = swap_be_16(p);
	id = i;
	setNick(n);

#ifdef LIBAMANITA_SDL
	if(!(set=SDLNet_AllocSocketSet(1))) stateChanged(SM_ERR_ALLOC_SOCKETSET,0,0,0);
	else if(SDLNet_ResolveHost(&address,host,port)==-1) stateChanged(SM_ERR_RESOLVE_HOST,0,0,0);
	else {
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&address))) stateChanged(SM_ERR_OPEN_SOCKET,0,0,0);
		else if(SDLNet_TCP_AddSocket(set,sock)==-1) stateChanged(SM_ERR_ADD_SOCKET,0,0,0);
		else {
			ip = address.host;

#elif defined __linux__
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1) stateChanged(SM_ERR_OPEN_SOCKET,0,0,0);
	else if(!(hostinfo=gethostbyname(host))) stateChanged(SM_ERR_RESOLVE_HOST,0,0,0);
	else {
		address.sin_addr = *(in_addr *)*hostinfo->h_addr_list;
		address.sin_family = AF_INET;
		address.sin_port = port;
		if(connect(sock,(sockaddr *)&address,sizeof(address))<0) stateChanged(SM_ERR_CONNECT,0,0,0);
		else {
			ip = address.sin_addr.s_addr;
			FD_ZERO(&set);
			FD_SET(sock,&set);
#endif /* LIBAMANITA_SDL */

#ifdef SOCKET_LENINCL
fprintf(stderr,"start(SOCKET_LENINCL)\n");
fflush(stderr);
			uint8_t d[strlen(nick)+SOCKET_HD+5];
			*SOCKET_TYPE(d+SOCKET_OFFSET) = SOCKET_SWAP(sizeof(d));
			*(uint32_t *)((void *)&d[SOCKET_HD]) = swap_be_32(id);
			strcpy((char *)&d[SOCKET_HD+4],nick);
#else /*SOCKET_LENINCL*/
			uint8_t d[strlen(nick)+5];
			*(uint32_t *)d = swap_be_32(id);
			strcpy(&d[4],nick);
#endif /*SOCKET_LENINCL*/
			if(aSocket::send(sock,d,sizeof(d))) {
				stateChanged(SM_STARTING_CLIENT,0,0,0);
#ifdef LIBAMANITA_SDL
				thread = SDL_CreateThread(_run,this);
				if(thread) return true;
#elif defined __linux__
				if(pthread_create(&thread,0,_run,this)==0) return true;
#endif /* LIBAMANITA_SDL */
			}
		}
	}
	return false;
}

void aClient::stop(bool kill) {
	if(isRunning()) {
		stateChanged(SM_STOPPING_CLIENT,0,0,0);
#ifdef LIBAMANITA_SDL
		if(thread) {
			if(kill) SDL_KillThread(thread);
			else SDL_WaitThread(thread,0);
			thread = 0;
		}

#elif defined __linux__
		if(kill) pthread_kill(thread,0);
		else pthread_join(thread,0);
#endif /* LIBAMANITA_SDL */

		setRunning(false);
		tcp_close(sock);
		sock = 0;
	}
}

void aClient::run() {
	int n;
#ifdef LIBAMANITA_SDL
#elif defined __linux__
	int s;
	fd_set test;
#endif /* LIBAMANITA_SDL */
	uint8_t *d;
	size_t l;
	setRunning(true);
	while(isRunning()) {
#ifdef LIBAMANITA_SDL
		n = SDLNet_CheckSockets(set,(uint32_t)-1);
fprintf(stderr,"aClient::run(n=%d)\n",n);
fflush(stderr);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,0,0);
			break;
		}
		if(n && SDLNet_SocketReady(sock)) {

#elif defined __linux__
		test = set;
		n = select(FD_SETSIZE,&test,0,0,0);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,0,0);
			break;
		}
		if(!n) continue;
		for(s=0; s<FD_SETSIZE; s++) {
			if(!FD_ISSET(s,&test)) continue;
#endif /* LIBAMANITA_SDL */

			d = receive(sock,l);
fprintf(stderr,"aClient::run(l=%zu)\n",l);
fflush(stderr);
			if(l>0) {
#ifndef SOCKET_NOCIPHER
	#ifdef SOCKET_LENINCL
				if(key) XORcipher(&d[SOCKET_HD],&d[SOCKET_HD],l-SOCKET_HD,key,keylen);
#else /*SOCKET_LENINCL*/
				if(key) XORcipher(d,d,l,key,keylen);
#endif /*SOCKET_LENINCL*/
#endif /*SOCKET_NOCIPHER*/
				stateChanged(SM_GET_MESSAGE,0,(intptr_t)d,(intptr_t)l);
			} else setRunning(false);
			releaseMessageBuffer(d);
		}
	}
}

int aClient::send(uint8_t *d,size_t l) {
#ifndef SOCKET_NOCIPHER
	if(key) {
		uint8_t dc[l];
#ifdef SOCKET_LENINCL
		memcpy(dc,d,SOCKET_HD);
		XORcipher(&dc[SOCKET_HD],&d[SOCKET_HD],l-SOCKET_HD,key,keylen);
#else /*SOCKET_LENINCL*/
		XORcipher(dc,d,l,key,keylen);
#endif /*SOCKET_LENINCL*/
		return aSocket::send(sock,dc,l);
	} else
#endif /*SOCKET_NOCIPHER*/
		return aSocket::send(sock,d,l);
}

void aClient::setNick(const char *n) {
	if(nick) free(nick);
	nick = strdup(n);
}

#ifndef SOCKET_NOCIPHER
void aClient::setKey(const uint32_t *k,size_t l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*SOCKET_NOCIPHER*/



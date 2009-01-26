
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libamanita/sdl/Client.h>



Client::Client(SocketListener l,uint32_t id,const char *nick)
		: Socket(l),id(id),nick(0) {
	if(nick && *nick) setNick(nick);
#ifndef TCPSOCK_NOCIPHER
	key = 0,keylen = 0;
#endif /*TCPSOCK_NOCIPHER*/
}

Client::~Client() {
	stop();
	if(nick) { free(nick);nick = 0; }
#ifndef TCPSOCK_NOCIPHER
	if(key) { free(key);key = 0; }
#endif /*TCPSOCK_NOCIPHER*/
}

bool Client::start(const char *con) {
	uint32_t i;
	uint16_t port;
	char host[16],nick[32];
fprintf(stderr,"start()\n");
fflush(stderr);
	resolveConnection(con,i,port,id,nick,32);
	i = SDL_SwapBE32(i);
	sprintf(host,"%d.%d.%d.%d",i>>24,(i>>16)&0xff,(i>>8)&0xff,i&0xff);
	if(*nick) setNick(nick);
fprintf(stderr,"start(\"%s\",%x,%d,%d,%s)\n",host,i,port,id,nick);
fflush(stderr);
	return start(host,port);
}


bool Client::start(const char *host,uint16_t port) {
fprintf(stderr,"start(\"%s\",%d)\n",host,port);
fflush(stderr);
	if(!(set=SDLNet_AllocSocketSet(1))) stateChanged(SM_ERR_ALLOC_SOCKETSET,0,0,0);
	else if(SDLNet_ResolveHost(&ip,host,port)==-1) stateChanged(SM_ERR_RESOLVE_HOST,0,0,0);
	else {
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&ip))) stateChanged(SM_ERR_OPEN_SOCKET,0,0,0);
		else if(SDLNet_TCP_AddSocket(set,sock)==-1) stateChanged(SM_ERR_ADD_SOCKET,0,0,0);
		else {
#ifdef TCPSOCK_LENINCL
fprintf(stderr,"start(TCPSOCK_LENINCL)\n");
fflush(stderr);
			char b[strlen(nick)+TCPSOCK_HD+5];
			*TCPSOCK_TYPE(b+TCPSOCK_OFFSET) = TCPSOCK_SWAP(sizeof(b));
			*(uint32_t *)&b[TCPSOCK_HD] = SDL_SwapBE32(id);
			strcpy(&b[TCPSOCK_HD+4],nick);
#else /*TCPSOCK_LENINCL*/
			char b[strlen(nick)+5];
			*(uint32_t *)b = SDL_SwapBE32(id);
			strcpy(&b[4],nick);
#endif /*TCPSOCK_LENINCL*/
			if(Socket::send(sock,b,sizeof(b))) {
				stateChanged(SM_STARTING_CLIENT,0,0,0);
				handle = SDL_CreateThread(_run,this);
				if(handle) return true;
			}
		}
	}
	return false;
}

void Client::stop(bool kill) {
	if(isRunning()) {
		stateChanged(SM_STOPPING_CLIENT,0,0,0);
		if(handle) {
			if(kill) SDL_KillThread(handle);
			else SDL_WaitThread(handle,0);
			handle = 0;
		}
		setRunning(false);
		SDLNet_TCP_Close(sock);
	}
}

void Client::run() {
	int n;
	char *b;
	int32_t l;
	setRunning(true);
	while(isRunning()) {
		n = SDLNet_CheckSockets(set,(uint32_t)-1);
fprintf(stderr,"Client::run(n=%d)\n",n);
fflush(stderr);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,0,0);
			break;
		}
		if(n && SDLNet_SocketReady(sock)) {
			b = (char *)receive(sock,l);
fprintf(stderr,"Client::run(l=%d)\n",l);
fflush(stderr);
			if(l<=0) setRunning(false);
			else {
#ifndef TCPSOCK_NOCIPHER
#	ifdef TCPSOCK_LENINCL
				if(key) XORcipher(&b[TCPSOCK_HD],&b[TCPSOCK_HD],l-TCPSOCK_HD,key,keylen);
#	else /*TCPSOCK_LENINCL*/
				if(key) XORcipher(b,b,l,key,keylen);
#	endif /*TCPSOCK_LENINCL*/
#endif /*TCPSOCK_NOCIPHER*/
				stateChanged(SM_GET_MESSAGE,0,(intptr_t)b,(intptr_t)l);
			}
			releaseMessageBuffer(b);
		}
	}
}

int Client::send(void *p,uint32_t l) {
#ifndef TCPSOCK_NOCIPHER
	if(key) {
		char pc[l];
#	ifdef TCPSOCK_LENINCL
		memcpy(pc,p,TCPSOCK_HD);
		XORcipher(&pc[TCPSOCK_HD],&((char *)p)[TCPSOCK_HD],l-TCPSOCK_HD,key,keylen);
#	else /*TCPSOCK_LENINCL*/
		XORcipher(pc,p,l,key,keylen);
#	endif /*TCPSOCK_LENINCL*/
		return Socket::send(sock,pc,l);
	} else
#endif /*TCPSOCK_NOCIPHER*/
		return Socket::send(sock,p,l);
}

void Client::setNick(const char *n) {
	if(nick) free(nick);
	nick = strdup(n);
}

#ifndef TCPSOCK_NOCIPHER
void Client::setKey(const uint32_t *k,int l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*TCPSOCK_NOCIPHER*/



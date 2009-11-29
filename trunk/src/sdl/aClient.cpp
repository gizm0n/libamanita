
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libamanita/sdl/aClient.h>



aClient::aClient(SocketListener l,uint32_t id,const char *nick) : aSocket(l),id(id),nick(0) {
	if(nick && *nick) setNick(nick);
#ifndef TCPSOCK_NOCIPHER
	key = 0,keylen = 0;
#endif /*TCPSOCK_NOCIPHER*/
}

aClient::~aClient() {
	stop();
	if(nick) { free(nick);nick = 0; }
#ifndef TCPSOCK_NOCIPHER
	if(key) { free(key);key = 0; }
#endif /*TCPSOCK_NOCIPHER*/
}

bool aClient::start(const char *con) {
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


bool aClient::start(const char *host,uint16_t port) {
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
			uint8_t d[strlen(nick)+TCPSOCK_HD+5];
			*TCPSOCK_TYPE(d+TCPSOCK_OFFSET) = TCPSOCK_SWAP(sizeof(d));
			*(uint32_t *)((void *)&d[TCPSOCK_HD]) = SDL_SwapBE32(id);
			strcpy((char *)&d[TCPSOCK_HD+4],nick);
#else /*TCPSOCK_LENINCL*/
			uint8_t d[strlen(nick)+5];
			*(uint32_t *)d = SDL_SwapBE32(id);
			strcpy(&d[4],nick);
#endif /*TCPSOCK_LENINCL*/
			if(aSocket::send(sock,d,sizeof(d))) {
				stateChanged(SM_STARTING_CLIENT,0,0,0);
				handle = SDL_CreateThread(_run,this);
				if(handle) return true;
			}
		}
	}
	return false;
}

void aClient::stop(bool kill) {
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

void aClient::run() {
	int n;
	uint8_t *d;
	size_t l;
	setRunning(true);
	while(isRunning()) {
		n = SDLNet_CheckSockets(set,(uint32_t)-1);
fprintf(stderr,"aClient::run(n=%d)\n",n);
fflush(stderr);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,0,0);
			break;
		}
		if(n && SDLNet_SocketReady(sock)) {
			d = receive(sock,l);
fprintf(stderr,"aClient::run(l=%zu)\n",l);
fflush(stderr);
			if(l>0) {
#ifndef TCPSOCK_NOCIPHER
#	ifdef TCPSOCK_LENINCL
				if(key) XORcipher(&d[TCPSOCK_HD],&d[TCPSOCK_HD],l-TCPSOCK_HD,key,keylen);
#	else /*TCPSOCK_LENINCL*/
				if(key) XORcipher(d,d,l,key,keylen);
#	endif /*TCPSOCK_LENINCL*/
#endif /*TCPSOCK_NOCIPHER*/
				stateChanged(SM_GET_MESSAGE,0,(intptr_t)d,(intptr_t)l);
			} else setRunning(false);
			releaseMessageBuffer(d);
		}
	}
}

int aClient::send(uint8_t *d,size_t l) {
#ifndef TCPSOCK_NOCIPHER
	if(key) {
		uint8_t dc[l];
#	ifdef TCPSOCK_LENINCL
		memcpy(dc,d,TCPSOCK_HD);
		XORcipher(&dc[TCPSOCK_HD],&d[TCPSOCK_HD],l-TCPSOCK_HD,key,keylen);
#	else /*TCPSOCK_LENINCL*/
		XORcipher(dc,d,l,key,keylen);
#	endif /*TCPSOCK_LENINCL*/
		return aSocket::send(sock,dc,l);
	} else
#endif /*TCPSOCK_NOCIPHER*/
		return aSocket::send(sock,d,l);
}

void aClient::setNick(const char *n) {
	if(nick) free(nick);
	nick = strdup(n);
}

#ifndef TCPSOCK_NOCIPHER
void aClient::setKey(const uint32_t *k,size_t l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*TCPSOCK_NOCIPHER*/



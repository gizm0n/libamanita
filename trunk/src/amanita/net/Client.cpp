
#include "../_config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <amanita/net/Client.h>



namespace a {



Client::Client(socket_event_handler seh) : Socket(seh) {
	nick = 0;
#ifndef SOCKET_NOCIPHER
	key = 0;
	keylen = 0;
#endif /*SOCKET_NOCIPHER*/
}

Client::~Client() {
	stop();
	if(nick) { free(nick);nick = 0; }
#ifndef SOCKET_NOCIPHER
	if(key) { free(key);key = 0; }
#endif /*SOCKET_NOCIPHER*/
}

bool Client::start(const char *con) {
	char h[16],n[33];
	uint32_t i,d;
	uint16_t p;
debug_output("start()\n");
	resolveConnection(con,i,p,d,n,32);
	sprintf(h,"%d.%d.%d.%d",i>>24,(i>>16)&0xff,(i>>8)&0xff,i&0xff);
debug_output("start(\"%s\",%x,%d,%d,%s)\n",h,i,p,d,n);
	return start(h,p,d,n);
}


bool Client::start(const char *h,uint16_t p,uint32_t i,const char *n) {
debug_output("start(\"%s\",%d)\n",h,p);

	setStarting(true);
	if(host) free(host);
	host = strdup(h);
	port = p;

/*#ifdef USE_SDL
	if(!(set=SDLNet_AllocSocketSet(1)))
		stateChanged(SM_ERR_ALLOC_SOCKETSET,0,(intptr_t)getError(),0);
	else if(SDLNet_ResolveHost(&address,host,p)==-1)
		stateChanged(SM_ERR_RESOLVE_HOST,0,(intptr_t)getError(),0);
	else {
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&address)))
			stateChanged(SM_ERR_OPEN_SOCKET,0,(intptr_t)getError(),0);
		else if(SDLNet_TCP_AddSocket(set,sock)==-1)
			stateChanged(SM_ERR_ADD_SOCKET,0,(intptr_t)getError(),0);
		else {
			ip = swap_be_32(address.host);

#elif defined(USE_GTK) || defined(USE_WIN32)*/
#ifdef USE_GTK
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
#endif
#ifdef USE_WIN32
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
#endif
		stateChanged(SM_ERR_OPEN_SOCKET,0,(intptr_t)getError(),0);
	else if(!(hostinfo=gethostbyname(host)))
		stateChanged(SM_ERR_RESOLVE_HOST,0,(intptr_t)getError(),0);
	else {
		address.sin_addr = *(in_addr *)*hostinfo->h_addr_list;
		address.sin_family = AF_INET;
		address.sin_port = swap_be_16(p);
#ifdef USE_GTK
		if(connect(sock,(sockaddr *)&address,sizeof(address))<0)
#endif
#ifdef USE_WIN32
		if(connect(sock,(SOCKADDR *)&address,sizeof(SOCKADDR_IN))!=0)
#endif
			stateChanged(SM_ERR_CONNECT,0,(intptr_t)getError(),0);
		else {
			ip = swap_be_32(address.sin_addr.s_addr);
			FD_ZERO(&set);
			FD_SET(sock,&set);
//#endif

#ifdef SOCKET_HEADER_INCLUDED
debug_output("start(SOCKET_HEADER_INCLUDED)\n");
			uint8_t d[strlen(n)+SOCKET_HEADER+5];
			*SOCKET_HEADER_LEN_TYPE(d+SOCKET_OFFSET) = SOCKET_HEADER_LEN_SWAP(sizeof(d));
			*(uint32_t *)((void *)&d[SOCKET_HEADER]) = swap_be_32(i);
			strcpy((char *)&d[SOCKET_HEADER+4],n);
#else /*SOCKET_HEADER_INCLUDED*/
			uint8_t d[strlen(n)+5];
			*(uint32_t *)d = swap_be_32(i);
			strcpy(&d[4],n);
#endif /*SOCKET_HEADER_INCLUDED*/
			if(Socket::send(sock,d,sizeof(d))) {
				stateChanged(SM_STARTING_CLIENT,0,0,0);
				thread.start(_run,this);
				return true;
			}
		}
	}
	setStarting(false);
	return false;
}

void Client::stop(bool kill) {
	if(isRunning() && !isStopping()) {
		setStopping(true);
		stateChanged(SM_STOPPING_CLIENT,0,0,0);
		setRunning(false);
		if(kill) thread.kill();
		else thread.stop();
		tcp_close(sock);
		sock = 0;
		setStopping(false);
	}
}

void Client::run() {
	int n;
//#ifdef USE_SDL
//#elif defined(USE_GTK) || defined (USE_WIN32)
	fd_set test;
	timeval timeout = { 0,LIBAMANITA_SELECT_TIMEOUT };
//#endif
	uint8_t *d;
	size_t l;
	setRunning(true);
	while(isRunning()) {
/*#ifdef USE_SDL
		n = SDLNet_CheckSockets(set,(uint32_t)-1);
debug_output("Client::run(n=%d)\n",n);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,(intptr_t)getError(),0);
			break;
		}
		if(n && SDLNet_SocketReady(sock)) {

#elif defined(USE_GTK) || defined(USE_WIN32)*/
		test = set;
		n = select(FD_SETSIZE,&test,0,0,&timeout);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,(intptr_t)getError(),0);
			break;
		}
		if(!n) continue;
		if(FD_ISSET(sock,&test)) {
//#endif

debug_output("Client::run(receive)\n");
			d = receive(sock,l);
debug_output("Client::run(l=%lu)\n",(unsigned long)l);
			if(l>0) {
				if(isStarting()) { // Server sends directly a message when accepting client, containing ID and nick. This guarantees client and server use the same ID and nick.
					uint8_t *p = d+SOCKET_HEADER;
					unpack_uint32(&p,id);
					setNick((char *)p);
debug_output("start(received id=%d,nick=%s)\n",getID(),getNick());
					setStarting(false);
				} else {
#ifndef SOCKET_NOCIPHER
#ifdef SOCKET_HEADER_INCLUDED
					if(key) XORcipher(&d[SOCKET_HEADER],&d[SOCKET_HEADER],l-SOCKET_HEADER,key,keylen);
#else /*SOCKET_HEADER_INCLUDED*/
					if(key) XORcipher(d,d,l,key,keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
#endif /*SOCKET_NOCIPHER*/
					stateChanged(SM_GET_MESSAGE,0,(intptr_t)d,(intptr_t)l);
				}
			} else setRunning(false);
			releaseMessageBuffer(d);
		}
	}
}

int Client::send(uint8_t *d,size_t l) {
#ifndef SOCKET_NOCIPHER
	if(key) {
		uint8_t dc[l];
#ifdef SOCKET_HEADER_INCLUDED
		memcpy(dc,d,SOCKET_HEADER);
		XORcipher(&dc[SOCKET_HEADER],&d[SOCKET_HEADER],l-SOCKET_HEADER,key,keylen);
#else /*SOCKET_HEADER_INCLUDED*/
		XORcipher(dc,d,l,key,keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
		return Socket::send(sock,dc,l);
	} else
#endif /*SOCKET_NOCIPHER*/
		return Socket::send(sock,d,l);
}

void Client::setNick(const char *n) {
	if(nick) free(nick);
	nick = strdup(n);
}

#ifndef SOCKET_NOCIPHER
void Client::setKey(const uint32_t *k,size_t l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*SOCKET_NOCIPHER*/


}; /* namespace a */


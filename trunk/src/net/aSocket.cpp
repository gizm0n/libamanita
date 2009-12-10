
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <libamanita/net/aSocket.h>


aSocket::aSocket(socket_event_handler seh) : event_handler(seh) {
	host = 0;
	status = 0;
	sock = 0;
	mut = 0;
#ifdef LIBAMANITA_SDL
	thread = 0;
	address = (IPaddress){0,0};
	set = 0;
#elif defined __linux__
	hostinfo = 0;
#endif /* LIBAMANITA_SDL */
	buf = 0;
	len = 0;
	setMessageBuffer(1024);
}

aSocket::~aSocket() {
	if(mut) deleteMutex();
	if(host) { free(host);host = 0; }
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
}

void aSocket::setMutex(socket_mutex_t m) {
	if(mut) deleteMutex();
	status |= SOCK_ST_ALIEN_MUTEX;
	mut = m;
}

void aSocket::createMutex() {
	if(mut) deleteMutex();
	status = (status&~SOCK_ST_ALIEN_MUTEX);
#ifdef LIBAMANITA_SDL
	mut = SDL_CreateMutex();
#elif defined __linux__
	mut = (socket_mutex_t)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mut,0);
#endif /* LIBAMANITA_SDL */
}

void aSocket::deleteMutex() {
	if(!mut) return;
	if(status&SOCK_ST_ALIEN_MUTEX) mut = 0;
	else {
#ifdef LIBAMANITA_SDL
		SDL_DestroyMutex(mut);
		mut = 0;
#elif defined __linux__
		pthread_mutex_destroy(mut);
		free(mut);
		mut = 0;
#endif /* LIBAMANITA_SDL */
	}
}


int aSocket::lock() {
	if(!mut) return 0;
#ifdef LIBAMANITA_SDL
	return SDL_mutexP(mut);
#elif defined __linux__
	return pthread_mutex_lock(mut);
#endif /* LIBAMANITA_SDL */
}

int aSocket::unlock() {
	if(!mut) return 0;
#ifdef LIBAMANITA_SDL
	return SDL_mutexV(mut);
#elif defined __linux__
	return pthread_mutex_unlock(mut);
#endif /* LIBAMANITA_SDL */
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
		r =tcp_recv(s,b+i,SOCKET_HEADER-i);
		if(r>0) i += r;
	} while(r>0 && i<SOCKET_HEADER);

fprintf(stderr,"aSocket::receive(0,r=%d,cmd=%d,len=%d)\n",
		r,(int)*b,SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(b+SOCKET_OFFSET)));
fflush(stderr);

	if(i==SOCKET_HEADER &&
			(i=SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(b+SOCKET_OFFSET)))>0) {
fprintf(stderr,"aSocket::receive(1,r=%d,i=%zu)\n",r,i);
fflush(stderr);
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
fprintf(stderr,"aSocket::receive(3,r=%d,l=%zu)\n",r,l);
fflush(stderr);
		}
		if(l==i) {
fprintf(stderr,"aSocket::receive(");
for(i=3; i<l && i<20; i++) {
	r = b[i];
	fputc(((r>>4)&0xf)<=9? '0'+((r>>4)&0xf) : 'a'+((r>>4)&0xf)-9,stderr);
	fputc((r&0xf)<=9? '0'+(r&0xf) : 'a'+(r&0xf)-9,stderr);
}
fprintf(stderr,")\n");
fflush(stderr);
			return b;
		}
		if(r==-1) {
#ifdef LIBAMANITA_SDL
fprintf(stderr,"aSocket::receive(SDL_Error=%s)\n",SDL_GetError());
#elif defined __linux__
perror("aSocket::receive");
#endif /* LIBAMANITA_SDL */
fflush(stderr);
		}
		if(b!=buf) free(b);
	}
//	stateChanged(SM_ERR_GET_MESSAGE,0,0,0);
	l = 0;
	return 0;
}


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
fprintf(stderr,"aSocket::send(0,s=%p,l=%zu)\n",(void *)s,l);
fflush(stderr);
#ifdef SOCKET_HEADER_INCLUDED
		int n;
		*SOCKET_HEADER_LEN_TYPE(d+SOCKET_OFFSET) = SOCKET_HEADER_LEN_SWAP(l);
		if((n=tcp_send(s,d,l))==(int)l) return n;
#else /*SOCKET_HEADER_INCLUDED*/
		TCPsockHeader n = SOCKET_HEADER_LEN_SWAP(l);
		if(tcp_send(s,&n,SOCKET_HEADER)==SOCKET_HEADER && (n=tcp_send(s,d,l))==(int)l) return n;
#endif /*SOCKET_HEADER_INCLUDED*/
fprintf(stderr,"aSocket::send(1,l=%zu)\n",l);
fflush(stderr);
	}
//	stateChanged(SM_ERR_PUT_MESSAGE,(uint32_t)buf,(uint32_t)l,0);
	return 0;
}





#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <libamanita/sdl/Socket.h>


Socket::Socket(SocketListener l) : listener(l) {
	handle = 0;
	ip = (IPaddress){0,0};
	host = 0;
	status = 0;
	sock = 0;
	set = 0;
	buf = 0;
	len = 0;
	setMessageBuffer(1024);
}

Socket::~Socket() {
	if(buf) { free(buf);buf = 0,len = 0; }
}

void Socket::resolveConnection(const char *con,uint32_t &ip,uint16_t &port,uint32_t &id,char *nick,int nlen) {
	ip = 0,port = 0,id = 0,*nick = '\0';
	if(!con || !*con) return;
	char buf[strlen(con)+1];
	strcpy(buf,con);
	char *p1 = buf,*p2;
	for(int i=0; p1; i++,p1=p2+1) {
		p2 = strpbrk(p1,".: |");
		if(p2) *p2 = '\0';
		if(p1) {
			if(i<4) ip |= atoi(p1)<<((3-i)*8);
			else if(i==4) port = atoi(p1);
			else if(i==5) id = atoi(p1);
			else if(i==6) {
				strncpy(nick,p1,nlen-1);
				break;
			}
		}
	}
	if(ip) ip = SDL_SwapBE32(ip);
}

void Socket::setMessageBuffer(size_t l) {
	if(isRunning() || isStarting()) return;
	if(buf) free(buf);
	buf = (uint8_t *)malloc(l),len = l;
}

uint8_t *Socket::receive(TCPsocket s,size_t &l) {
	uint8_t *b = buf;
	int r;
	size_t i = 0;
	do { r = SDLNet_TCP_Recv(s,b+i,TCPSOCK_HD-i),i += r; } while(r>0 && i<TCPSOCK_HD);
fprintf(stderr,"Socket::receive(0,r=%d,cmd=%d,len=%d)\n",r,(int)*b,TCPSOCK_SWAP(*TCPSOCK_TYPE(b+TCPSOCK_OFFSET)));
fflush(stderr);
	if(i==TCPSOCK_HD && (i=TCPSOCK_SWAP(*TCPSOCK_TYPE(b+TCPSOCK_OFFSET)))>0) {
fprintf(stderr,"Socket::receive(1,r=%d,i=%zu)\n",r,i);
fflush(stderr);
#ifdef TCPSOCK_LENINCL
		l = TCPSOCK_HD;
		if(i>len) {
			b = (uint8_t *)malloc(i);
			memcpy(b,buf,TCPSOCK_HD);
		}
#else /*TCPSOCK_LENINCL*/
		l = 0;
		if((uint32_t)i>len) b = (char *)malloc(i);
#endif /*TCPSOCK_LENINCL*/
		while(r>0 && l<i) {
			r = SDLNet_TCP_Recv(s,b+l,i-l);
			l += r;
fprintf(stderr,"Socket::receive(3,r=%d,l=%zu)\n",r,l);
fflush(stderr);
		}
		if(l==i) {
fprintf(stderr,"Socket::receive(");
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
fprintf(stderr,"Socket::receive(SDL_Error=%s)\n",SDL_GetError());
fflush(stderr);
		}
		if(b!=buf) free(b);
	}
//	stateChanged(SM_ERR_GET_MESSAGE,0,0,0);
	l = 0;
	return 0;
}


#ifndef TCPSOCK_NOCIPHER
void Socket::XORcipher(uint8_t *d,const uint8_t *s,size_t l,const uint32_t *k,size_t kl) {
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
#endif /*TCPSOCK_NOCIPHER*/


size_t Socket::send(TCPsocket s,uint8_t *d,size_t l) {
	if(d && l>0) {
fprintf(stderr,"Socket::send(0,s=%p,l=%zu)\n",s,l);
fflush(stderr);
#ifdef TCPSOCK_LENINCL
		int n;
		*TCPSOCK_TYPE(d+TCPSOCK_OFFSET) = TCPSOCK_SWAP(l);
		if((n=SDLNet_TCP_Send(s,d,l))==(int)l) return n;
#else /*TCPSOCK_LENINCL*/
		TCPsockHeader n = TCPSOCK_SWAP(l);
		if(SDLNet_TCP_Send(s,&n,TCPSOCK_HD)==TCPSOCK_HD && (n=SDLNet_TCP_Send(s,d,l))==(int)l) return n;
#endif /*TCPSOCK_LENINCL*/
fprintf(stderr,"Socket::send(1,l=%zu)\n",l);
fflush(stderr);
	}
//	stateChanged(SM_ERR_PUT_MESSAGE,(uint32_t)buf,(uint32_t)l,0);
	return 0;
}





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

void Socket::resolveConnection(const char *con,Uint32 &ip,Uint16 &port,Uint32 &id,char *nick,int nlen) {
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

void Socket::setMessageBuffer(Uint32 n) {
	if(isRunning() || isStarting()) return;
	if(buf) free(buf);
	buf = malloc(n),len = n;
}

void *Socket::receive(TCPsocket s,Sint32 &n) {
	char *b = (char *)buf;
	int l = 0,r;
	do r = SDLNet_TCP_Recv(s,b+l,TCPSOCK_HD-l),l += r;
	while(r>0 && l<TCPSOCK_HD);
fprintf(stderr,"Socket::receive(0,r=%d,l=%d)\n",r,TCPSOCK_SWAP(*TCPSOCK_TYPE(b+TCPSOCK_OFFSET)));
fflush(stderr);
	if(l==TCPSOCK_HD && (l=TCPSOCK_SWAP(*TCPSOCK_TYPE(b+TCPSOCK_OFFSET)))>0) {
fprintf(stderr,"Socket::receive(1,r=%d,l=%d)\n",r,l);
fflush(stderr);
#ifdef TCPSOCK_LENINCL
		n = TCPSOCK_HD;
		if((Uint32)l>len) {
			b = (char *)malloc(l);
			memcpy(b,buf,TCPSOCK_HD);
		}
#else /*TCPSOCK_LENINCL*/
		n = 0;
		if((Uint32)l>len) b = (char *)malloc(l);
#endif /*TCPSOCK_LENINCL*/
		while(r>0 && n<l) {
			r = SDLNet_TCP_Recv(s,b+n,l-n);
			n += r;
fprintf(stderr,"Socket::receive(3,r=%d,n=%d)\n",r,n);
fflush(stderr);
		}
		if(n==l) return b;
		if(r==-1) {
fprintf(stderr,"Socket::receive(4,SDL_Error=%s)\n",SDL_GetError());
fflush(stderr);
		}
		if(b!=buf) free(b);
	}
//	stateChanged(SM_ERR_GET_MESSAGE,0,0,0);
	n = 0;
	return 0;
}


#ifndef TCPSOCK_NOCIPHER
void Socket::XORcipher(char *d,const char *s,Uint32 l,const Uint32 *k,int kl) {
	Uint32 n,i,*d1 = (Uint32 *)d;
	const Uint32 *s1 = (const Uint32 *)s;
	for(n=0,i=0; l>=4; n++,l-=4,i++) {
		if((int)i==kl) i = 0;
		d1[n] = s1[n]^k[i];
	}
	if(l>0) {
		if((int)i==kl) i = 0;
		const char *k1 = (const char *)&k[i];
		for(i=0,n<<=2; i<l; i++,n++) d[n] = s[n]^k1[i];
	}
}
#endif /*TCPSOCK_NOCIPHER*/


int Socket::send(TCPsocket s,void *p,Uint32 l) {
	if(p && l>0) {
fprintf(stderr,"Socket::send(0,s=%p,l=%d)\n",s,l);
fflush(stderr);
#ifdef TCPSOCK_LENINCL
		int n;
		*TCPSOCK_TYPE(((char *)p)+TCPSOCK_OFFSET) = TCPSOCK_SWAP(l);
		if((n=SDLNet_TCP_Send(s,p,l))==(int)l) return n;
#else /*TCPSOCK_LENINCL*/
		TCPsockHeader n = TCPSOCK_SWAP(l);
		if(SDLNet_TCP_Send(s,&n,TCPSOCK_HD)==TCPSOCK_HD && (n=SDLNet_TCP_Send(s,p,l))==l) return (int)n;
#endif /*TCPSOCK_LENINCL*/
fprintf(stderr,"Socket::send(1,l=%d)\n",l);
fflush(stderr);
	}
//	stateChanged(SM_ERR_PUT_MESSAGE,(Uint32)buf,(Uint32)l,0);
	return 0;
}




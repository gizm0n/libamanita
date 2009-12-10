
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <libamanita/net/aServer.h>



#ifndef SOCKET_NOCIPHER
void aServerConnection::setKey(const uint32_t *k,size_t l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*SOCKET_NOCIPHER*/



uint32_t aServer::id_index = 0;

aServer::aServer(socket_event_handler seh) : aSocket(seh),clients(),main("main"),channels() {
#ifdef LIBAMANITA_SDL
	setsz = 0;
#elif defined __linux__
#endif /* LIBAMANITA_SDL */
	channels.put(main.getName(),&main);
}

aServer::~aServer() {
	stop();
}

bool aServer::start(const char *con) {
	if(isRunning() || isStarting()) return false;
	uint32_t id;
	char nick[32];
fprintf(stderr,"start()\n");
fflush(stderr);
	resolveConnection(con,ip,port,id,nick,32);
fprintf(stderr,"start(ip=%x,port=%d,id=%d,nick=%s)\n",ip,port,id,nick);
fflush(stderr);
	return start(port);
}

bool aServer::start(uint16_t p) {
	if(isRunning() || isStarting()) return false;
	bool ret = false;
	if(!mut) createMutex();
	lock();
	setStarting(true);
	port = swap_be_16(p);

#ifdef LIBAMANITA_SDL
	if(SDLNet_ResolveHost(&address,0,p)==-1) stateChanged(SM_ERR_RESOLVE_HOST,(intptr_t)&address,0,0);
	else {
		host = strdup(SDLNet_ResolveIP(&address));
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&address))) stateChanged(SM_ERR_OPEN_SOCKET,(intptr_t)&address,0,0);
		else {
			stateChanged(SM_STARTING_SERVER,0,0,0);
			thread = SDL_CreateThread(_run,this);
			ret = true;
		}
	}

#elif defined __linux__
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1) stateChanged(SM_ERR_OPEN_SOCKET,0,0,0);
	else {
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(p);
		if(bind(sock,(sockaddr *)&address,sizeof(address))==-1) stateChanged(SM_ERR_BIND,(intptr_t)&address,0,0);
		else if(listen(sock,1)==-1) stateChanged(SM_ERR_LISTEN,(intptr_t)&address,0,0);
		else {
			FD_ZERO(&set);
			FD_SET(sock,&set);
			if(pthread_create(&thread,0,_run,this)==0) ret = true;
		}
	}
#endif /* LIBAMANITA_SDL */

	setStarting(false);
	unlock();
	return ret;
}

void aServer::stop(bool kill) {
	if(isRunning()) {
		lock();
		stateChanged(SM_STOPPING_SERVER,0,0,0);
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
		killAllClients();
		tcp_close(sock);
		sock = 0;
		unlock();
	}
}

void aServer::run() {
	tcp_socket_t s;
	aConnection c;
	uint8_t *b;
	size_t l;
#ifdef LIBAMANITA_SDL
	int n,i;
	createSocketSet(16);
#elif defined __linux__
	fd_set test;
#endif /* LIBAMANITA_SDL */
	setRunning(true);
	while(isRunning()) {

#ifdef LIBAMANITA_SDL
		if(!set) { stop();break; }
		n = SDLNet_CheckSockets(set,(uint32_t)-1);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,0,0);
			break;
		} else if(n==0) continue;
		lock();
		if(SDLNet_SocketReady(sock)) {
			n--;
			if((s=SDLNet_TCP_Accept(sock))) {
				b = receive(s,l);
				if(l>0) c = addClient(s,b,l);
				else tcp_close(s);
				releaseMessageBuffer(b);
			}
		}
fprintf(stderr,"aServer::run(clients=%zu,n=%d)\n",clients.size(),n);
fflush(stderr);
		for(i=main.size()-1; n>0 && i>=0; i--) {
			c = (aConnection)main[i];
fprintf(stderr,"aServer::run(id=%" PRIu32 ",sock=%p)\n",c->getID(),c->sock);
fflush(stderr);
			if(SDLNet_SocketReady(c->sock)) {
				n--;
				b = receive(c->sock,l);
				if(l>0) {
#ifndef SOCKET_NOCIPHER
#ifdef SOCKET_HEADER_INCLUDED
					if(c->key) XORcipher(&b[SOCKET_HEADER],&b[SOCKET_HEADER],l-SOCKET_HEADER,c->key,c->keylen);
#else /*SOCKET_HEADER_INCLUDED*/
					if(c->key) XORcipher(b,b,l,c->key,c->keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
#endif /*SOCKET_NOCIPHER*/
					stateChanged(SM_GET_MESSAGE,(intptr_t)c,(intptr_t)b,(intptr_t)l);
				} else killClient(c->id);
				releaseMessageBuffer(b);
			}
		}
		unlock();

#elif defined __linux__
		test = set;
		select(FD_SETSIZE,&test,0,0,0);

		lock();
		for(s=0; s<FD_SETSIZE; s++) {
			if(FD_ISSET(s,&test)) {
				if(s==sock) {
					s = accept(sock,0,0);
					b = receive(s,l);
					if(l>0) c = addClient(s,b,l);
					else tcp_close(s);
					releaseMessageBuffer(b);
					continue;
				}
				c = (aConnection)sockets.get(s);
				if(!c) {
					FD_CLR(s,&set);
					tcp_close(s);
					continue;
				}
fprintf(stderr,"aServer::run(id=%" PRIu32 ",sock=%p)\n",c->getID(),(void *)c->sock);
fflush(stderr);
				b = receive(c->sock,l);
				if(l>0) {
#ifndef SOCKET_NOCIPHER
#ifdef SOCKET_HEADER_INCLUDED
					if(c->key) XORcipher(&b[SOCKET_HEADER],&b[SOCKET_HEADER],l-SOCKET_HEADER,c->key,c->keylen);
#else /*SOCKET_HEADER_INCLUDED*/
					if(c->key) XORcipher(b,b,l,c->key,c->keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
#endif /*SOCKET_NOCIPHER*/
					stateChanged(SM_GET_MESSAGE,(intptr_t)c,(intptr_t)b,(intptr_t)l);
				} else killClient(c->id);
				releaseMessageBuffer(b);
			}
		}
		unlock();

#endif /* LIBAMANITA_SDL */
	}
}

void aServer::changeNick(uint32_t id,const char *nick) {
	aConnection client = (aConnection)clients.get(id);
	if(!client) return;
	free(client->nick);
	client->nick = strdup(nick);
}

aChannel aServer::createChannel(const char *ch) {
	if(!ch || !*ch) return 0;
	aChannel channel = (aChannel)channels.get(ch);
	if(!channel) {
		channel = (aChannel)new aServerChannel(ch);
		channels.put(ch,channel);
	}
	return channel;
}

void aServer::deleteChannel(const char *ch) {
fprintf(stderr,"aServer::deleteChannel(name=%s)\n",ch);
fflush(stderr);
	aChannel channel = (aChannel)channels.remove(ch);
	if(channel && channel!=&main) {
		if(channel->size()>0) {
			aConnection c;
			for(int i=channel->size()-1; i>=0; i--) {
				c = (aConnection)(*channel)[i];
				c->leaveChannel(channel);
			}
		}
		delete channel;
	}
}

void aServer::leaveChannel(aChannel ch,aConnection c) {
	if(ch && c) {
fprintf(stderr,"aServer::leaveChannel(1,name=%s,size(%zu))\n",ch->getName(),ch->size());
fflush(stderr);
		*ch -= c;
		c->leaveChannel(ch);
fprintf(stderr,"aServer::leaveChannel(2,size(%zu))\n",ch->size());
fflush(stderr);
		if(ch->size()==0) deleteChannel(ch->getName());
	}
}


aConnection aServer::addClient(tcp_socket_t s,uint8_t *d,size_t l) {
	d += SOCKET_HEADER;
	uint32_t id;
	if(status&SERVER_ST_INTERNAL_CLIENT_ID) id = ++id_index;
	else id = swap_be_32(*(uint32_t *)d);
	char *nick = (char *)(d+4);
fprintf(stderr,"aServer::addClient(id=%" PRIu32 ",nick=%s)\n",id,nick);
fflush(stderr);
	stateChanged(SM_CHECK_NICK,(intptr_t)&nick,0,0);
	if(!uniqueID(id)) stateChanged(SM_DUPLICATE_ID,id,(intptr_t)nick,0);
	else {
		aServerConnection *c = new aServerConnection(s,id,nick);
		sockets.put(s,(void *)c);
		clients.put(id,(void *)c);
		main += c;
		stateChanged(SM_ADD_CLIENT,(intptr_t)c,0,0);
#ifdef LIBAMANITA_SDL
		if(main.size()+1>setsz) createSocketSet();
		SDLNet_TCP_AddSocket(set,s);
#elif defined __linux__
		FD_SET(s,&set);
#endif /* LIBAMANITA_SDL */
		return (aConnection)c;
	}
	tcp_close(s);
	return 0;
}

void aServer::killClient(uint32_t id) {
fprintf(stderr,"aServer::killClient(id=%" PRIu32 ")\n",id);
fflush(stderr);
	aConnection client = (aConnection)clients.remove(id);
	if(client) {
		sockets.remove(client->sock);
		main -= client;
		if(client->channels.size()>0)
			for(int i=client->channels.size()-1; i>=0; i--)
				leaveChannel((aChannel)client->channels[i],client);
#ifdef LIBAMANITA_SDL
		SDLNet_TCP_DelSocket(set,client->sock);
#elif defined __linux__
		FD_CLR(client->sock,&set);
#endif /* LIBAMANITA_SDL */
		tcp_close(client->sock);
		stateChanged(SM_KILL_CLIENT,(intptr_t)client,0,0);
		delete client;
	}
fprintf(stderr,"aServer::killClient(done)\n");
fflush(stderr);
}

void aServer::killAllClients() {
	if(!clients.size()) return;
	lock();
fprintf(stderr,"aServer::killAllClients()\n");
fflush(stderr);
	aConnection client;
	for(int i=main.size()-1; i>=0; i--) {
		client = (aConnection)main[i];
		tcp_close(client->sock);
		stateChanged(SM_KILL_CLIENT,(intptr_t)client,0,0);
		delete client;
	}
	sockets.removeAll();
	clients.removeAll();
	main.clear();
	aHashtable::iterator iter = channels.iterate();
	aChannel ch;
	while((ch=(aChannel)iter.next()))
		if(ch!=&main) delete ch;
	channels.removeAll();
	channels.put(main.getName(),&main);
#ifdef LIBAMANITA_SDL
	createSocketSet(16);
#elif defined __linux__
	FD_ZERO(&set);
	FD_SET(sock,&set);
#endif /* LIBAMANITA_SDL */
	unlock();
}


#ifdef LIBAMANITA_SDL
void aServer::createSocketSet(int n) {
	setsz = n;
	if(set) SDLNet_FreeSocketSet(set);
	if(main.size()+1>setsz) setsz = (main.size()+1)*2;
fprintf(stderr,"aServer::createSocketSet(setsz=%zu)\n",setsz);
fflush(stderr);
	set = SDLNet_AllocSocketSet(setsz);
	if(!set) stateChanged(SM_ERR_ALLOC_SOCKETSET,0,0,0);
	else {
		SDLNet_TCP_AddSocket(set,sock);
		aConnection client;
		int n;
		for(size_t i=0; i<main.size(); i++) {
			client = (aConnection)main[i];
fprintf(stderr,"aServer::createSocketSet(id=%" PRIu32 ",nick=%s)\n",client->getID(),client->getNick());
fflush(stderr);
			n = SDLNet_TCP_AddSocket(set,client->sock);
if(n==-1) {
fprintf(stderr,"aServer::createSocketSet(error=%s)\n",SDLNet_GetError());
fflush(stderr);
}
		}
	}
}
#elif defined __linux__
#endif /* LIBAMANITA_SDL */


int aServer::send(aConnection c,uint8_t *d,size_t l) {
#ifndef SOCKET_NOCIPHER
	if(c->key) {
		uint8_t dc[l];
#ifdef SOCKET_HEADER_INCLUDED
		memcpy(dc,d,SOCKET_HEADER);
		XORcipher(&dc[SOCKET_HEADER],&d[SOCKET_HEADER],l-SOCKET_HEADER,c->key,c->keylen);
#else /*SOCKET_HEADER_INCLUDED*/
		XORcipher(dc,d,l,c->key,c->keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
		return aSocket::send(c->sock,dc,l);
	} else
#endif /*SOCKET_NOCIPHER*/
		return aSocket::send(c->sock,d,l);
}

void aServer::send(aChannel channel,uint8_t *d,size_t l) {
	if(!channel) channel = (aChannel)&main;
	if(l==0 || !d || channel->size()==0) return;
#ifndef SOCKET_NOCIPHER
	uint8_t p[l],*dc;
#define SOCKET_P dc
#else /*SOCKET_NOCIPHER*/
#define SOCKET_P d
#endif /*SOCKET_NOCIPHER*/
#ifdef SOCKET_HEADER_INCLUDED
	*SOCKET_HEADER_LEN_TYPE(d+SOCKET_OFFSET) = SOCKET_HEADER_LEN_SWAP(l);
#ifndef SOCKET_NOCIPHER
	memcpy(p,d,SOCKET_HEADER);
#endif /*SOCKET_NOCIPHER*/
#else /*SOCKET_HEADER_INCLUDED*/
	TCPsockType n = SOCKET_HEADER_LEN_SWAP(l);
#endif /*SOCKET_HEADER_INCLUDED*/
	aConnection c;
fprintf(stderr,"aServer::send(l=%zu,cmd=%d,len=%d)\n",
		l,(int)*d,SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(d+SOCKET_OFFSET)));
fflush(stderr);
	lock();
	for(size_t i=0; i<channel->size(); i++)
		if((c=(aConnection)(*channel)[i])->isActive()) {
#ifndef SOCKET_NOCIPHER
			if(c->key) {
				dc = p;
#ifdef SOCKET_HEADER_INCLUDED
				XORcipher(&dc[SOCKET_HEADER],&d[SOCKET_HEADER],l-SOCKET_HEADER,c->key,c->keylen);
#else /*SOCKET_HEADER_INCLUDED*/
				XORcipher(dc,d,l,c->key,c->keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
			} else dc = d;
#endif /*SOCKET_NOCIPHER*/

#ifdef SOCKET_HEADER_INCLUDED
			if(tcp_send(c->sock,SOCKET_P,l)<(int)l) {
#else /*SOCKET_HEADER_INCLUDED*/
			if(tcp_send(c->sock,&n,SOCKET_HEADER)!=SOCKET_HEADER
					|| tcp_send(c->sock,SOCKET_P,l)<(int)l) {
#endif /*SOCKET_HEADER_INCLUDED*/
				killClient(c);
				i--;
			}
		}
	unlock();
}




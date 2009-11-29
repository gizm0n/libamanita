
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <libamanita/sdl/aServer.h>


#ifndef TCPSOCK_NOCIPHER
void ServerConnection::setKey(const uint32_t *k,size_t l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*TCPSOCK_NOCIPHER*/

aServer::aServer(SocketListener l) : aSocket(l),clients(),main("main"),channels() {
	setsz = 0;
	mut = SDL_CreateMutex();
	channels.put(main.getName(),&main);
}

aServer::~aServer() {
	stop();
	if(mut) { SDL_DestroyMutex(mut);mut = 0; }
}

bool aServer::start(const char *con) {
	if(isRunning() || isStarting()) return false;
	uint32_t id;
	char nick[32];
fprintf(stderr,"start()\n");
fflush(stderr);
	resolveConnection(con,ip.host,ip.port,id,nick,32);
fprintf(stderr,"start(%x,%d,%d,%s)\n",ip.host,ip.port,id,nick);
fflush(stderr);
	return start(ip.port);
}

bool aServer::start(uint32_t port) {
	if(isRunning() || isStarting()) return false;
	lock();
	setStarting(true);
	ip.port = port;
	IPaddress i;
	bool ret = false;
	if(SDLNet_ResolveHost(&i,0,port)==-1) stateChanged(SM_ERR_RESOLVE_HOST,(intptr_t)&i,0,0);
	else {
		host = SDLNet_ResolveIP(&i);
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&i))) stateChanged(SM_ERR_OPEN_SOCKET,(intptr_t)&i,0,0);
		else {
			stateChanged(SM_STARTING_SERVER,0,0,0);
			handle = SDL_CreateThread(_run,this);
			ret = true;
		}
	}
	setStarting(false);
	unlock();
	return ret;
}

void aServer::stop(bool kill) {
	if(isRunning()) {
		lock();
		stateChanged(SM_STOPPING_SERVER,0,0,0);
		if(handle) {
			if(kill) SDL_KillThread(handle);
			else SDL_WaitThread(handle,0);
			handle = 0;
		}
		setRunning(false);
		killAllClients();
		SDLNet_TCP_Close(sock);
		sock = 0;
		unlock();
	}
}

void aServer::run() {
	int n,i;
	TCPsocket s;
	Connection c;
	uint8_t *b;
	size_t l;
	setRunning(true);
	setsz = 16;
	createSocketSet();
	while(isRunning()) {
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
				else SDLNet_TCP_Close(s);
				releaseMessageBuffer(b);
			}
		}
fprintf(stderr,"aServer::run(clients=%zu,n=%d)\n",clients.size(),n);
fflush(stderr);
		for(i=main.size()-1; n>0 && i>=0; i--) {
			c = (Connection)main[i];
fprintf(stderr,"aServer::run(id=%" PRIu32 ",sock=%p)\n",c->getID(),c->sock);
fflush(stderr);
			if(SDLNet_SocketReady(c->sock)) {
				n--;
				b = receive(c->sock,l);
				if(l>0) {
#ifndef TCPSOCK_NOCIPHER
#	ifdef TCPSOCK_LENINCL
					if(c->key) XORcipher(&b[TCPSOCK_HD],&b[TCPSOCK_HD],l-TCPSOCK_HD,c->key,c->keylen);
#	else /*TCPSOCK_LENINCL*/
					if(c->key) XORcipher(b,b,l,c->key,c->keylen);
#	endif /*TCPSOCK_LENINCL*/
#endif /*TCPSOCK_NOCIPHER*/
					stateChanged(SM_GET_MESSAGE,(intptr_t)c,(intptr_t)b,(intptr_t)l);
				} else killClient(c->id);
				releaseMessageBuffer(b);
			}
		}
		unlock();
	}
}

void aServer::changeNick(uint32_t id,const char *nick) {
	Connection client = (Connection)clients.get(id);
	if(!client) return;
	free(client->nick);
	client->nick = strdup(nick);
}

Channel aServer::createChannel(const char *ch) {
	if(!ch || !*ch) return 0;
	Channel channel = (Channel)channels.get(ch);
	if(!channel) {
		channel = (Channel)new ServerChannel(ch);
		channels.put(ch,channel);
	}
	return channel;
}

void aServer::deleteChannel(const char *ch) {
fprintf(stderr,"aServer::deleteChannel(name=%s)\n",ch);
fflush(stderr);
	Channel channel = (Channel)channels.remove(ch);
	if(channel && channel!=&main) {
		if(channel->size()>0) {
			Connection c;
			for(int i=channel->size()-1; i>=0; i--) {
				c = (Connection)(*channel)[i];
				c->leaveChannel(channel);
			}
		}
		delete channel;
	}
}

void aServer::leaveChannel(Channel ch,Connection c) {
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


Connection aServer::addClient(TCPsocket s,uint8_t *d,size_t l) {
	d += TCPSOCK_HD;
	uint32_t id = SDL_SwapBE32(*(uint32_t *)d);
	char *nick = (char *)(d+4);
fprintf(stderr,"aServer::addClient(id=%" PRIu32 ",nick=%s)\n",id,nick);
fflush(stderr);
	stateChanged(SM_CHECK_NICK,(intptr_t)nick,0,0);
	if(!uniqueID(id)) stateChanged(SM_DUPLICATE_ID,id,(intptr_t)nick,0);
	else {
		ServerConnection *c = new ServerConnection(s,id,nick);
		clients.put(id,(void *)c);
		main += c;
		stateChanged(SM_ADD_CLIENT,(intptr_t)c,0,0);
		if(main.size()+1>setsz) createSocketSet();
		else SDLNet_TCP_AddSocket(set,c->sock);
		return (Connection)c;
	}
	SDLNet_TCP_Close(s);
	return 0;
}

void aServer::killClient(uint32_t id) {
fprintf(stderr,"aServer::killClient(id=%" PRIu32 ")\n",id);
fflush(stderr);
	Connection client = (Connection)clients.remove(id);
	if(client) {
		main -= client;
		if(client->channels.size()>0)
			for(int i=client->channels.size()-1; i>=0; i--)
				leaveChannel((Channel)client->channels[i],client);
		SDLNet_TCP_DelSocket(set,client->sock);
		SDLNet_TCP_Close(client->sock);
		stateChanged(SM_KILL_CLIENT,(intptr_t)client,0,0);
		delete client;
	}
fprintf(stderr,"aServer::killClient(done)\n");
fflush(stderr);
}

void aServer::killAllClients() {
	if(!clients.size()) return;
	lock();
	Connection client;
	for(int i=main.size()-1; i>=0; i--) {
		client = (Connection)main[i];
		SDLNet_TCP_Close(client->sock);
		stateChanged(SM_KILL_CLIENT,(intptr_t)client,0,0);
		delete client;
	}
	clients.removeAll();
	main.clear();
	aHashtable::iterator iter = channels.iterate();
	Channel ch;
	while((ch=(Channel)iter.next())) delete ch;
	channels.removeAll();
	createSocketSet();
	unlock();
}

void aServer::createSocketSet() {
	if(set) SDLNet_FreeSocketSet(set);
	if(main.size()+1>setsz) setsz = (main.size()+1)*2;
fprintf(stderr,"aServer::createSocketSet(setsz=%zu)\n",setsz);
fflush(stderr);
	set = SDLNet_AllocSocketSet(setsz);
	if(!set) stateChanged(SM_ERR_ALLOC_SOCKETSET,0,0,0);
	else {
		SDLNet_TCP_AddSocket(set,sock);
		Connection client;
		int n;
		for(size_t i=0; i<main.size(); i++) {
			client = (Connection)main[i];
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

int aServer::send(Connection c,uint8_t *d,size_t l) {
#ifndef TCPSOCK_NOCIPHER
	if(c->key) {
		uint8_t dc[l];
#	ifdef TCPSOCK_LENINCL
		memcpy(dc,d,TCPSOCK_HD);
		XORcipher(&dc[TCPSOCK_HD],&d[TCPSOCK_HD],l-TCPSOCK_HD,c->key,c->keylen);
#	else /*TCPSOCK_LENINCL*/
		XORcipher(dc,d,l,c->key,c->keylen);
#	endif /*TCPSOCK_LENINCL*/
		return aSocket::send(c->sock,dc,l);
	} else
#endif /*TCPSOCK_NOCIPHER*/
		return aSocket::send(c->sock,d,l);
}

void aServer::send(Channel channel,uint8_t *d,size_t l) {
	if(!channel) channel = (Channel)&main;
	if(l==0 || !d || channel->size()==0) return;
#ifndef TCPSOCK_NOCIPHER
	uint8_t p[l],*dc;
#	define TCPSOCK_P dc
#else /*TCPSOCK_NOCIPHER*/
#	define TCPSOCK_P d
#endif /*TCPSOCK_NOCIPHER*/
#ifdef TCPSOCK_LENINCL
	*TCPSOCK_TYPE(d+TCPSOCK_OFFSET) = TCPSOCK_SWAP(l);
#	ifndef TCPSOCK_NOCIPHER
	memcpy(p,d,TCPSOCK_HD);
#	endif /*TCPSOCK_NOCIPHER*/
#else /*TCPSOCK_LENINCL*/
	TCPsockType n = TCPSOCK_SWAP(l);
#endif /*TCPSOCK_LENINCL*/
	Connection c;
fprintf(stderr,"aServer::send(l=%zu,cmd=%d,len=%d)\n",l,(int)*d,TCPSOCK_SWAP(*TCPSOCK_TYPE(d+TCPSOCK_OFFSET)));
fflush(stderr);
	lock();
	for(size_t i=0; i<channel->size(); i++)
		if((c=(Connection)(*channel)[i])->isActive()) {
#ifndef TCPSOCK_NOCIPHER
			if(c->key) {
				dc = p;
#	ifdef TCPSOCK_LENINCL
				XORcipher(&dc[TCPSOCK_HD],&d[TCPSOCK_HD],l-TCPSOCK_HD,c->key,c->keylen);
#	else /*TCPSOCK_LENINCL*/
				XORcipher(dc,d,l,c->key,c->keylen);
#	endif /*TCPSOCK_LENINCL*/
			} else dc = d;
#endif /*TCPSOCK_NOCIPHER*/
#ifdef TCPSOCK_LENINCL
			if(SDLNet_TCP_Send(c->sock,TCPSOCK_P,l)<(int)l) {
#else /*TCPSOCK_LENINCL*/
			if(SDLNet_TCP_Send(c->sock,&n,TCPSOCK_HD)!=TCPSOCK_HD || SDLNet_TCP_Send(c->sock,TCPSOCK_P,l)<(int)l) {
#endif /*TCPSOCK_LENINCL*/
				killClient(c);
				i--;
			}
		}
	unlock();
}


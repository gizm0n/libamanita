
#include "../config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <libamanita/sdl/Server.h>


#ifndef TCPSOCK_NOCIPHER
void ServerConnection::setKey(const Uint32 *k,int l) {
	if(key) free(key);
	key = (Uint32 *)malloc(sizeof(Uint32)*l),keylen = l;
	memcpy(key,k,sizeof(Uint32)*keylen);
}
#endif /*TCPSOCK_NOCIPHER*/

Server::Server(SocketListener l) : Socket(l),clients(),main("main"),channels() {
	setsz = 0;
	mut = SDL_CreateMutex();
	channels.put(main.getName(),&main);
}

Server::~Server() {
	stop();
	if(mut) { SDL_DestroyMutex(mut);mut = 0; }
}

bool Server::start(const char *con) {
	if(isRunning() || isStarting()) return false;
	Uint32 id;
	char nick[32];
fprintf(stderr,"start()\n");
fflush(stderr);
	resolveConnection(con,ip.host,ip.port,id,nick,32);
fprintf(stderr,"start(%x,%d,%d,%s)\n",ip.host,ip.port,id,nick);
fflush(stderr);
	return start(ip.port);
}

bool Server::start(Uint32 port) {
	if(isRunning() || isStarting()) return false;
	lock();
	setStarting(true);
	ip.port = port;
	IPaddress i;
	bool ret = false;
	if(SDLNet_ResolveHost(&i,0,port)==-1) stateChanged(SM_ERR_RESOLVE_HOST,(Uint32)&i,0,0);
	else {
		host = SDLNet_ResolveIP(&i);
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&i))) stateChanged(SM_ERR_OPEN_SOCKET,(Uint32)&i,0,0);
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

void Server::stop(bool kill) {
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

void Server::run() {
	int n,i;
	TCPsocket s;
	Connection c;
	char *b;
	Sint32 l;
	setRunning(true);
	setsz = 16;
	createSocketSet();
	while(isRunning()) {
		if(!set) { stop();break; }
		n = SDLNet_CheckSockets(set,(Uint32)-1);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,0,0);
			break;
		} else if(n==0) continue;
		lock();
		if(SDLNet_SocketReady(sock)) {
			n--;
			if((s=SDLNet_TCP_Accept(sock))) {
				b = (char *)receive(s,l);
				if(l>0) c = addClient(s,b,l);
				else SDLNet_TCP_Close(s);
				releaseMessageBuffer(b);
			}
		}
fprintf(stderr,"Server::run(clients=%d,n=%d)\n",clients.size(),n);
fflush(stderr);
		for(i=main.size()-1; n>0 && i>=0; i--) {
			c = (Connection)main[i];
fprintf(stderr,"Server::run(id=%d,sock=%p)\n",c->getID(),c->sock);
fflush(stderr);
			if(SDLNet_SocketReady(c->sock)) {
				n--;
				b = (char *)receive(c->sock,l);
				if(l>0) {
#ifndef TCPSOCK_NOCIPHER
#	ifdef TCPSOCK_LENINCL
					if(c->key) XORcipher(&b[TCPSOCK_HD],&b[TCPSOCK_HD],l-TCPSOCK_HD,c->key,c->keylen);
#	else /*TCPSOCK_LENINCL*/
					if(c->key) XORcipher(b,b,l,c->key,c->keylen);
#	endif /*TCPSOCK_LENINCL*/
#endif /*TCPSOCK_NOCIPHER*/
					stateChanged(SM_GET_MESSAGE,(Uint32)c,(Uint32)b,(Uint32)l);
				} else killClient(c->id);
				releaseMessageBuffer(b);
			}
		}
		unlock();
	}
}

void Server::changeNick(Uint32 id,const char *nick) {
	Connection client = (Connection)clients.get((unsigned long)id);
	if(!client) return;
	free(client->nick);
	client->nick = strdup(nick);
}

Channel Server::createChannel(const char *ch) {
	if(!ch || !*ch) return 0;
	Channel channel = (Channel)channels.get(ch);
	if(!channel) {
		channel = (Channel)new ServerChannel(ch);
		channels.put(ch,channel);
	}
	return channel;
}

void Server::deleteChannel(const char *ch) {
fprintf(stderr,"Server::deleteChannel(name=%s)\n",ch);
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

void Server::leaveChannel(Channel ch,Connection c) {
	if(ch && c) {
fprintf(stderr,"Server::leaveChannel(1,name=%s,size(%d))\n",ch->getName(),ch->size());
fflush(stderr);
		*ch -= c;
		c->leaveChannel(ch);
fprintf(stderr,"Server::leaveChannel(2,size(%d))\n",ch->size());
fflush(stderr);
		if(ch->size()==0) deleteChannel(ch->getName());
	}
}


Connection Server::addClient(TCPsocket s,void *p,Uint32 l) {
	p = ((char *)p)+TCPSOCK_HD;
	Uint32 id = SDL_SwapBE32(*(Uint32 *)p);
	char *nick = ((char *)p)+4;
fprintf(stderr,"Server::addClient(id=%d,nick=%s)\n",id,nick);
fflush(stderr);
	stateChanged(SM_CHECK_NICK,(Uint32)nick,0,0);
	if(!uniqueID(id)) stateChanged(SM_DUPLICATE_ID,id,(Uint32)nick,0);
	else {
		ServerConnection *c = new ServerConnection(s,id,nick);
		clients.put((unsigned long)id,(void *)c);
		main += c;
		stateChanged(SM_ADD_CLIENT,(Uint32)c,0,0);
		if(main.size()+1>setsz) createSocketSet();
		else SDLNet_TCP_AddSocket(set,c->sock);
		return (Connection)c;
	}
	SDLNet_TCP_Close(s);
	return 0;
}

void Server::killClient(Uint32 id) {
fprintf(stderr,"Server::killClient(id=%d)\n",id);
fflush(stderr);
	Connection client = (Connection)clients.remove((unsigned long)id);
	if(client) {
		main -= client;
		if(client->channels.size()>0)
			for(int i=client->channels.size()-1; i>=0; i--)
				leaveChannel((Channel)client->channels[i],client);
		SDLNet_TCP_DelSocket(set,client->sock);
		SDLNet_TCP_Close(client->sock);
		stateChanged(SM_KILL_CLIENT,(Uint32)client,0,0);
		delete client;
	}
fprintf(stderr,"Server::killClient(done)\n");
fflush(stderr);
}

void Server::killAllClients() {
	if(!clients.size()) return;
	lock();
	Connection client;
	for(int i=main.size()-1; i>=0; i--) {
		client = (Connection)main[i];
		SDLNet_TCP_Close(client->sock);
		stateChanged(SM_KILL_CLIENT,(Uint32)client,0,0);
		delete client;
	}
	clients.removeAll();
	main.clear();
	Hashtable::iterator iter = channels.iterate();
	Channel ch;
	while((ch=(Channel)iter.next())) delete ch;
	channels.removeAll();
	createSocketSet();
	unlock();
}

void Server::createSocketSet() {
	if(set) SDLNet_FreeSocketSet(set);
	if(main.size()+1>setsz) setsz = (main.size()+1)*2;
fprintf(stderr,"Server::createSocketSet(setsz=%d)\n",setsz);
fflush(stderr);
	set = SDLNet_AllocSocketSet(setsz);
	if(!set) stateChanged(SM_ERR_ALLOC_SOCKETSET,0,0,0);
	else {
		SDLNet_TCP_AddSocket(set,sock);
		Connection client;
		int n;
		for(size_t i=0ul; i<main.size(); i++) {
			client = (Connection)main[i];
fprintf(stderr,"Server::createSocketSet(id=%d,nick=%s)\n",client->getID(),client->getNick());
fflush(stderr);
			n = SDLNet_TCP_AddSocket(set,client->sock);
if(n==-1) {
fprintf(stderr,"Server::createSocketSet(error=%s)\n",SDLNet_GetError());
fflush(stderr);
}
		}
	}
}

int Server::send(Connection c,void *p,Uint32 l) {
#ifndef TCPSOCK_NOCIPHER
	if(c->key) {
		char pc[l];
#	ifdef TCPSOCK_LENINCL
		memcpy(pc,p,TCPSOCK_HD);
		XORcipher(&pc[TCPSOCK_HD],&((char *)p)[TCPSOCK_HD],l-TCPSOCK_HD,c->key,c->keylen);
#	else /*TCPSOCK_LENINCL*/
		XORcipher(pc,p,l,c->key,c->keylen);
#	endif /*TCPSOCK_LENINCL*/
		return Socket::send(c->sock,pc,l);
	} else
#endif /*TCPSOCK_NOCIPHER*/
		return Socket::send(c->sock,p,l);
}

void Server::send(Channel channel,void *p,Uint32 l) {
	if(!channel) channel = (Channel)&main;
	if(l==0 || !p || channel->size()==0) return;
fprintf(stderr,"Server::send(l=%d)\n",l);
fflush(stderr);
#ifndef TCPSOCK_NOCIPHER
	char d[l],*pc;
#	define TCPSOCK_P pc
#else /*TCPSOCK_NOCIPHER*/
#	define TCPSOCK_P p
#endif /*TCPSOCK_NOCIPHER*/
#ifdef TCPSOCK_LENINCL
	*TCPSOCK_TYPE(((char *)p)+TCPSOCK_OFFSET) = TCPSOCK_SWAP(l);
#	ifndef TCPSOCK_NOCIPHER
	memcpy(d,p,TCPSOCK_HD);
#	endif /*TCPSOCK_NOCIPHER*/
#else /*TCPSOCK_LENINCL*/
	TCPsockType n = TCPSOCK_SWAP(l);
#endif /*TCPSOCK_LENINCL*/
	Connection c;
	lock();
	for(size_t i=0ul; i<channel->size(); i++)
		if((c=(Connection)(*channel)[i])->isActive()) {
#ifndef TCPSOCK_NOCIPHER
			if(c->key) {
				pc = d;
#	ifdef TCPSOCK_LENINCL
				XORcipher(&pc[TCPSOCK_HD],&((char *)p)[TCPSOCK_HD],l-TCPSOCK_HD,c->key,c->keylen);
#	else /*TCPSOCK_LENINCL*/
				XORcipher(pc,(char *)p,l,c->key,c->keylen);
#	endif /*TCPSOCK_LENINCL*/
			} else pc = (char *)p;
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


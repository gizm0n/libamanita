
#include "../_config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <amanita/net/Server.h>



namespace a {

#ifndef SOCKET_NOCIPHER
void ServerConnection::setKey(const uint32_t *k,size_t l) {
	if(key) free(key);
	key = (uint32_t *)malloc(sizeof(uint32_t)*l),keylen = l;
	memcpy(key,k,sizeof(uint32_t)*keylen);
}
#endif /*SOCKET_NOCIPHER*/



uint32_t Server::id_index = 0xffff;

Server::Server(socket_event_handler seh) : Socket(seh),_sockets(),_clients(),_main("main"),_channels() {
/*#ifdef USE_SDL
	setsz = 0;
#elif defined(USE_LINUX) || defined(USE_WIN32)*/
	sockets_n = sockets_cap = 0,sockets = 0;
//#endif
	_channels.put(_main.getName(),&_main);
}

Server::~Server() {
/*#ifdef USE_SDL
#elif defined(USE_LINUX) || defined(USE_WIN32)*/
	if(sockets) free(sockets);
	sockets_n = sockets_cap = 0,sockets = 0;
//#endif
	stop();
}

bool Server::start(const char *con) {
	if(isRunning() || isStarting()) return false;
	uint32_t id;
	char nick[32];
debug_output("start()\n");
	resolveConnection(con,ip,port,id,nick,32);
debug_output("start(ip=%x,port=%d,id=%d,nick=%s)\n",ip,port,id,nick);
	return start(port);
}

bool Server::start(uint16_t p) {
	while(isStopping()) thread.pause(50);
	if(isRunning() || isStarting()) return false;
	bool ret = false;
	setStarting(true);
	port = p;

/*#ifdef USE_SDL
	if(SDLNet_ResolveHost(&address,0,swap_be_16(p))==-1)
		stateChanged(SM_ERR_RESOLVE_HOST,(intptr_t)&address,(intptr_t)getError(),0);
	else {
		host = strdup(SDLNet_ResolveIP(&address));
		stateChanged(SM_RESOLVE_HOST,0,0,0);
		if(!(sock=SDLNet_TCP_Open(&address)))
			stateChanged(SM_ERR_OPEN_SOCKET,(intptr_t)&address,(intptr_t)getError(),0);
		else {
			stateChanged(SM_STARTING_SERVER,0,0,0);
			thread.start(_run,this);
			ret = true;
		}
	}

#elif defined(USE_LINUX) || defined(USE_WIN32)*/
#ifdef USE_LINUX
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
#endif
#ifdef USE_WIN32
	if((sock=WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED))==INVALID_SOCKET)
#endif
		stateChanged(SM_ERR_OPEN_SOCKET,0,(intptr_t)getError(),0);
	else {
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = swap_be_32(INADDR_ANY);
		address.sin_port = swap_be_16(p);
#ifdef USE_LINUX
		int y = 1;
		if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char *)&y,sizeof(int))==-1 ||
				bind(sock,(sockaddr *)&address,sizeof(address))==-1)
#endif
#ifdef USE_WIN32
		int y = 1,n;
		if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char *)&y,sizeof(int))==SOCKET_ERROR ||
				bind(sock,(SOCKADDR *)&address,sizeof(address))==SOCKET_ERROR)
#endif
			stateChanged(SM_ERR_BIND,(intptr_t)&address,(intptr_t)getError(),0);
#ifdef USE_LINUX
		else if(listen(sock,SOMAXCONN)==-1)
#endif
#ifdef USE_WIN32
		else if(listen(sock,SOMAXCONN)==SOCKET_ERROR ||
				ioctlsocket(sock,FIONBIO,(u_long *)&n)==SOCKET_ERROR)
#endif
			stateChanged(SM_ERR_LISTEN,(intptr_t)&address,(intptr_t)getError(),0);
		else {
			stateChanged(SM_STARTING_SERVER,0,0,0);
			thread.start(_run,this);
			ret = true;
		}
	}
//#endif

	setStarting(false);
	return ret;
}

void Server::stop(bool kill) {
	while(isStarting()) thread.pause(50);
	if(isRunning()) {
		setStopping(true);
		stateChanged(SM_STOPPING_SERVER,0,0,0);
		setRunning(false);
debug_output("Server::stop(kill=%d)\n",kill);
		if(kill) thread.kill();
		else thread.stop();
debug_output("Server::stop(killAllClients)\n");
		killAllClients();
debug_output("Server::stop(tcp_close)\n");
		tcp_close(sock);
		sock = 0;
		setStopping(false);
	}
}

//#ifdef USE_SDL
//#elif defined(USE_LINUX) || defined(USE_WIN32)
void Server::addSocket(tcp_socket_t s) {
	if(!sockets) sockets_cap = 8,sockets = (tcp_socket_t *)malloc(sizeof(tcp_socket_t)*sockets_cap);
	else if(sockets_n+1==sockets_cap) sockets_cap <<= 1,sockets = (tcp_socket_t *)realloc(sockets,sockets_cap);
	sockets[sockets_n++] = s;
	FD_SET(s,&set);
}

void Server::removeSocket(tcp_socket_t s) {
	size_t i,j;
	for(i=0; i<sockets_n; i++) if(sockets[i]==s) {
		for(j=i+1; j<sockets_n; j++) sockets[j-1] = sockets[j];
		sockets_n--;
		break;
	}
	tcp_close(s);
	FD_CLR(s,&set);
}
//#endif


void Server::run() {
	tcp_socket_t s;
	Connection c;
	uint8_t *b;
	size_t l;
/*#ifdef USE_SDL
	int i,n;
	createSocketSet(16);
#elif defined(USE_LINUX) || defined(USE_WIN32)*/
	size_t i;
	int n;
	fd_set test;
	FD_ZERO(&set);
	FD_SET(sock,&set);
	timeval timeout = { 0,LIBAMANITA_SELECT_TIMEOUT };
//#endif
	setRunning(true);
	while(isRunning()) {

/*#ifdef USE_SDL
		if(!set) { stop();break; }
		n = SDLNet_CheckSockets(set,(uint32_t)-1);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,(intptr_t)getError(),0);
			break;
		} else if(n==0) continue;
		if(SDLNet_SocketReady(sock)) {
			n--;
			if((s=SDLNet_TCP_Accept(sock))) {
				b = receive(s,l);
				if(l>0) c = addClient(s,b,l);
				else tcp_close(s);
				releaseMessageBuffer(b);
			}
		}
debug_output("Server::run(clients=%lu,n=%d)\n",(unsigned long)_sockets.size(),n);
		for(i=_main.size()-1; n>0 && i>=0; i--) {
			c = (Connection)_main[i];
debug_output("Server::run(id=%" PRIu32 ",sock=%p)\n",c->getID(),c->sock);
			if(SDLNet_SocketReady(c->sock)) {
				n--;
				b = receive(c->sock,l);
				if(l>0) {
#ifndef SOCKET_NOCIPHER
#ifdef SOCKET_HEADER_INCLUDED
					if(c->key) XORcipher(&b[SOCKET_HEADER],&b[SOCKET_HEADER],l-SOCKET_HEADER,c->key,c->keylen);
#else //SOCKET_HEADER_INCLUDED
					if(c->key) XORcipher(b,b,l,c->key,c->keylen);
#endif //SOCKET_HEADER_INCLUDED
#endif //SOCKET_NOCIPHER
					stateChanged(SM_GET_MESSAGE,(intptr_t)c,(intptr_t)b,(intptr_t)l);
				} else killClient(c->id);
				releaseMessageBuffer(b);
			}
		}

#elif defined(USE_LINUX) || defined(USE_WIN32)*/
		test = set;
		n = select(FD_SETSIZE,&test,0,0,&timeout);
		if(n==-1) {
			stateChanged(SM_ERR_CHECK_SOCKETS,0,(intptr_t)getError(),0);
			break;
		}
		if(!n) continue;

debug_output("Server::run(1)\n");
		if(FD_ISSET(sock,&test)) {
debug_output("Server::run(2)\n");
			s = accept(sock,0,0);
			addSocket(s);
debug_output("Server::run(3,n=%lu,cap=%lu)\n",(unsigned long)sockets_n,(unsigned long)sockets_cap);
		} else for(i=0; i<sockets_n; i++) {
			s = sockets[i];
debug_output("Server::run(4,i=%lu,s=%d (sock=%d))\n",(unsigned long)i,s,sock);
			if(FD_ISSET(s,&test)) {
debug_output("Server::run(5)\n");
				b = receive(s,l);
				c = (Connection)_sockets.get(s);
				if(!c) {
debug_output("Server::run(6,s=%d (sock=%d))\n",s,sock);
					if(l>0) c = addClient(s,b,l);
					else removeSocket(s);
				} else if(l>0) {
debug_output("Server::run(id=%" PRIu32 ",sock=%p)\n",(uint32_t)c->getID(),(void *)((intptr_t)c->sock));
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

//#endif
	}
}

void Server::changeNick(uint32_t id,const char *nick) {
	Connection c = (Connection)_clients.get(id);
	if(!c) return;
	_clients.remove(c->nick);
	free(c->nick);
	c->nick = strdup(nick);
	_clients.put(c->nick,(void *)c);
}

Channel Server::createChannel(const char *ch) {
	if(!ch || !*ch) return 0;
	Channel channel = (Channel)_channels.get(ch);
	if(!channel) {
		channel = (Channel)new ServerChannel(ch);
		_channels.put(ch,channel);
	}
	return channel;
}

void Server::deleteChannel(const char *ch) {
debug_output("Server::deleteChannel(name=%s)\n",ch);
	Channel channel = (Channel)_channels.remove(ch);
	if(channel && channel!=&_main) {
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
debug_output("Server::leaveChannel(1,name=%s,size(%lu))\n",ch->getName(),(unsigned long)ch->size());
		*ch << c;
		c->leaveChannel(ch);
debug_output("Server::leaveChannel(2,size(%lu))\n",(unsigned long)ch->size());
		if(ch->size()==0) deleteChannel(ch->getName());
	}
}


Connection Server::addClient(tcp_socket_t s,uint8_t *d,size_t l) {
	d += SOCKET_HEADER;
	uint32_t id;
	Connection c = 0;
	if(status&SERVER_ST_INTERNAL_CLIENT_ID) id = ++id_index;
	else id = swap_be_32(*(uint32_t *)d);
	char *nick = (char *)(d+4);
debug_output("Server::addClient(id=%" PRIu32 ",nick=%s)\n",id,nick);
	stateChanged(SM_CHECK_NICK,(intptr_t)&nick,0,0);
	if(!uniqueID(id)) stateChanged(SM_DUPLICATE_ID,id,(intptr_t)nick,0);
	else {
		c = (Connection)new ServerConnection(s,id,nick);
		_sockets.put(c->sock,(void *)c);
		_clients.put(c->id,(void *)c);
		_clients.put(c->nick,(void *)c);
		_main << c;
/*#ifdef USE_SDL
		if(_main.size()+1>setsz) createSocketSet();
		SDLNet_TCP_AddSocket(set,s);
#endif*/
		{ // Respond directly with ID and nick.
debug_output("Server::addClient(respond)\n");
			uint8_t data[SOCKET_HEADER+5+strlen(nick)],*p = data;
			pack_header(&p,0);
			pack_uint32(&p,id);
			strcpy((char *)p,nick);
			Socket::send(s,data,sizeof(data));
		}
		stateChanged(SM_ADD_CLIENT,(intptr_t)c,0,0);
	}
	return c;
}

void Server::killClient(Connection c) {
	if(!c) return;
debug_output("Server::killClient(id=%" PRIu32 ")\n",c->id);
	_sockets.remove(c->sock);
	_clients.remove(c->id);
	_clients.remove(c->nick);
	_main >> c;
	if(c->_channels.size()>0)
		for(int i=c->_channels.size()-1; i>=0; i--)
			leaveChannel((Channel)c->_channels[i],c);
/*#ifdef USE_SDL
	SDLNet_TCP_DelSocket(set,c->sock);
	tcp_close(c->sock);
#elif defined(USE_LINUX) || defined(USE_WIN32)*/
	removeSocket(c->sock);
//#endif
	stateChanged(SM_KILL_CLIENT,(intptr_t)c,0,0);
	delete c;
debug_output("Server::killClient(done)\n");
}

void Server::killAllClients() {
	if(!_sockets.size()) return;
debug_output("Server::killAllClients()\n");
	Connection client;
	for(int i=_main.size()-1; i>=0; i--) {
		client = (Connection)_main[i];
		tcp_close(client->sock);
		delete client;
	}
	_sockets.removeAll();
	_clients.removeAll();
	_main.clear();
	Hashtable::iterator iter = _channels.iterate();
	Channel ch;
	while((ch=(Channel)iter.next()))
		if(ch!=&_main) delete ch;
	_channels.removeAll();
	_channels.put(_main.getName(),&_main);
/*#ifdef USE_SDL
	SDLNet_FreeSocketSet(set);
	set = 0;
#endif*/
}


/*#ifdef USE_SDL
void Server::createSocketSet(int n) {
	setsz = n;
	if(set) SDLNet_FreeSocketSet(set);
	if(_main.size()+1>setsz) setsz = (_main.size()+1)*2;
debug_output("Server::createSocketSet(setsz=%lu)\n",(unsigned long)setsz);
	set = SDLNet_AllocSocketSet(setsz);
	if(!set) stateChanged(SM_ERR_ALLOC_SOCKETSET,0,(intptr_t)getError(),0);
	else {
		SDLNet_TCP_AddSocket(set,sock);
		Connection client;
		int n;
		for(size_t i=0; i<_main.size(); i++) {
			client = (Connection)_main[i];
debug_output("Server::createSocketSet(id=%" PRIu32 ",nick=%s)\n",client->getID(),client->getNick());
			n = SDLNet_TCP_AddSocket(set,client->sock);
if(n==-1) {
debug_output("Server::createSocketSet(error=%s)\n",SDLNet_GetError());
}
		}
	}
}
#endif*/


int Server::send(Connection c,uint8_t *d,size_t l) {
//debug_output("Server::send(c=%p)\n",c);
	if(!c) {
		send(&_main,d,l);
		return l;
	}
#ifndef SOCKET_NOCIPHER
	if(c->key) {
		uint8_t dc[l];
#ifdef SOCKET_HEADER_INCLUDED
		memcpy(dc,d,SOCKET_HEADER);
		XORcipher(&dc[SOCKET_HEADER],&d[SOCKET_HEADER],l-SOCKET_HEADER,c->key,c->keylen);
#else /*SOCKET_HEADER_INCLUDED*/
		XORcipher(dc,d,l,c->key,c->keylen);
#endif /*SOCKET_HEADER_INCLUDED*/
		return Socket::send(c->sock,dc,l);
	}
#endif /*SOCKET_NOCIPHER*/
	return Socket::send(c->sock,d,l);
}

void Server::send(Channel channel,uint8_t *d,size_t l) {
	if(!channel) channel = (Channel)&_main;
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
	tcp_socket_header_len_t n = SOCKET_HEADER_LEN_SWAP(l);
#endif /*SOCKET_HEADER_INCLUDED*/
	Connection c;
debug_output("Server::send(l=%lu,cmd=%d,len=%d)\n",
		(unsigned long)l,(int)*d,SOCKET_HEADER_LEN_SWAP(*SOCKET_HEADER_LEN_TYPE(d+SOCKET_OFFSET)));
debug_output("Server::send(lock)\n");
	lock();
debug_output("Server::send(channel->size=%d)\n",(int)channel->size());
	for(size_t i=0; i<channel->size(); i++)
		if((c=(Connection)(*channel)[i])->isActive()) {
debug_output("Server::send(c=%p)\n",c);
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

debug_output("Server::send(tcp_send)\n");
#ifdef SOCKET_HEADER_INCLUDED
			if(tcp_send(c->sock,SOCKET_P,l)<(int)l) {
#else /*SOCKET_HEADER_INCLUDED*/
			if(tcp_send(c->sock,&n,sizeof(n))!=sizeof(n)
					|| tcp_send(c->sock,SOCKET_P,l)<(int)l) {
#endif /*SOCKET_HEADER_INCLUDED*/
debug_output("Server::send(killClient)\n");
				killClient(c);
				i--;
			}
		}
debug_output("Server::send(unlock)\n");
	unlock();
}

}; /* namespace a */




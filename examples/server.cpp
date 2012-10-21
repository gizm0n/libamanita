
#include <stdio.h>
#include <amanita/Application.h>
#include <amanita/net/Server.h>

Server *server;

void send_server_message(Connection c,const char *format, ...) {
	uint8_t data[SOCKET_HEADER+257],*p = data;
	pack_header(&p,0);
	va_list args;
   va_start(args,format);
	vsnprintf((char *)p,256,format,args);
   va_end(args);
	if(*p) server->send(c,data,SOCKET_HEADER+strlen((const char *)p)+1);
}

uint32_t server_listener(Socket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	switch(st) {
		case SM_ERR_RESOLVE_HOST:
		case SM_ERR_OPEN_SOCKET:
		case SM_ERR_CONNECT:
		case SM_ERR_BIND:
		case SM_ERR_LISTEN:
		case SM_ERR_ADD_SOCKET:
		case SM_ERR_ALLOC_SOCKETSET:
		case SM_ERR_CHECK_SOCKETS:
		case SM_ERR_GET_MESSAGE:
		case SM_ERR_PUT_MESSAGE:
			fprintf(stderr,"Server error: %u[%s] %s\n",st,Socket::message_names[st],(const char *)p2);
			break;
		case SM_CHECK_NICK:
			printf("Check nick: %s\n",*(char **)p1);
			return 1;
		case SM_DUPLICATE_ID:
			printf("server_listener(SM_DUPLICATE_ID)\n");
			return 1;
		case SM_STARTING_SERVER:
		{
			uint32_t ipaddr = server->getIP();
			printf("Starting server...\nIP address: %d.%d.%d.%d\nPort: %d\n",
					ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff,server->getPort());
			break;
		}
		case SM_STOPPING_SERVER:
			printf("Stopping server...\n");
			break;
		case SM_ADD_CLIENT:
		{
			printf("Add Client.\n");
			Connection c = (Connection)p1;
			/* Add handling of new client here; store client data, send list of clients etc. */
			c->setActive(true);
			send_server_message(0,"%s joined.",c->getNick());
			break;
		}
		case SM_KILL_CLIENT:
		{
			Connection c = (Connection)p1;
			c->setActive(false);
			printf("Kill Client.\n");
			fflush(stdout);
			send_server_message(0,"%s left.",c->getNick());
			break;
		}
		case SM_GET_MESSAGE:
		{
			uint8_t *data = (uint8_t *)p2,cmd;
			unpack_header(&data,cmd);
			printf("Receive message: %s\n",data);
			server->send((uint8_t *)p2,(size_t)p3);
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	char text[1024];
	Application app(INIT_SOCKETS);
	app.open(argc,argv);
	server = new Server(server_listener);
	server->start(2012);
	gets(text);
	server->stop();
	delete(server);
	app.close();
	return 0;
}


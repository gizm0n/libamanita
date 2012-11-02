
#include <stdio.h>
#include <amanita/Main.h>
#include <amanita/net/Client.h>

using namespace a;


Client *client;


uint32_t client_listener(Socket *s,uint32_t st,intptr_t p1,intptr_t p2,intptr_t p3) {
	switch(st) {
		case SM_ERR_RESOLVE_HOST:
		case SM_ERR_OPEN_SOCKET:
		case SM_ERR_CONNECT:
		case SM_ERR_ADD_SOCKET:
		case SM_ERR_ALLOC_SOCKETSET:
		case SM_ERR_CHECK_SOCKETS:
		case SM_ERR_GET_MESSAGE:
		case SM_ERR_PUT_MESSAGE:
			fprintf(stderr,"Client error: %u[%s] %s\n",st,Socket::message_names[st],(const char *)p2);
			break;
		case SM_STARTING_CLIENT:
		{
			uint32_t ipaddr = client->getIP();
			const char *host = client->getHost();
			printf("Connecting to server...\nIP Address: %d.%d.%d.%d\nHostname: %s\nPort: %d\n",
				ipaddr>>24,(ipaddr>>16)&0xff,(ipaddr>>8)&0xff,ipaddr&0xff,host? host : "N/A",client->getPort());
			break;
		}
		case SM_STOPPING_CLIENT:
			printf("Disconnecting from server...\n");
			break;
		case SM_GET_MESSAGE:
		{
			uint8_t *data = (uint8_t *)p2,cmd;
			unpack_header(&data,cmd);
			printf("%s\n",data);
			break;
		}
	}
	return 0;
}


int main(int argc, char *argv[]) {
	if(argc!=2) printf("Invalid parameters.\nUsage: client HOST:PORT:USER_ID:NICK\n");
	else {
		char text[1024-SOCKET_HEADER-1];
		uint8_t data[1024],*p;
		Main app(APP_SOCKETS);
		app.open(argc,argv);
		client = new Client(client_listener);
		client->start(argv[1]);
		while(1) {
			gets(text);
			if(!strcmp(text,"exit")) break;
			p = data;
			pack_header(&p,0);
			sprintf((char *)p,"%s> %s",client->getNick(),text);
			client->send(data,(size_t)(SOCKET_HEADER+strlen((char *)p)+1));
		}
		client->stop();
		delete(client);
		app.close();
	}
	return 0;
}


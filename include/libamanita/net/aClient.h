#ifndef _LIBAMANITA_NET_CLIENT_H
#define _LIBAMANITA_NET_CLIENT_H

/**
 * @file libamanita/net/aClient.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <libamanita/net/aSocket.h>



/** A client class for TCP networking.
 *
 * This class connects to a server over the network and handles communication. It uses
 * a simple built in protocol for communication.
 */
class aClient : public aSocket {
private:
	uint32_t id;
	char *nick;
#ifndef SOCKET_NOCIPHER
	uint32_t *key;
	int keylen;
#endif /*SOCKET_NOCIPHER*/

	static thread_func_t _run(void *p) { ((aClient *)p)->run();return 0; }
	void run();

public:
	aClient(socket_event_handler seh);
	~aClient();

	bool start(const char *con);
	bool start(const char *h,uint16_t p,uint32_t i,const char *n);
	void stop(bool kill=true);

	int send(uint8_t *d,size_t l);

	void setID(uint32_t i) { id = i; }
	uint32_t getID() { return id; }
	void setNick(const char *n);
	const char *getNick() { return nick; }
#ifndef SOCKET_NOCIPHER
	void setKey(const uint32_t *k,size_t l);
#endif /*SOCKET_NOCIPHER*/
};


#endif /* _LIBAMANITA_NET_CLIENT_H */


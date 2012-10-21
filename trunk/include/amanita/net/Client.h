#ifndef _AMANITA_NET_CLIENT_H
#define _AMANITA_NET_CLIENT_H

/**
 * @file amanita/net/Client.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <amanita/net/Socket.h>


/** Amanita Namespace */
namespace a {


/** A client class for TCP networking.
 *
 * This class connects to a server over the network and handles communication. It uses
 * a simple built in protocol for communication.
 * @ingroup net
 */
class Client : public Socket {
private:
	uint32_t id;
	char *nick;
#ifndef SOCKET_NOCIPHER
	uint32_t *key;
	int keylen;
#endif /*SOCKET_NOCIPHER*/

	static void _run(void *p) { ((Client *)p)->run(); }
	void run();

public:
	Client(socket_event_handler seh);
	~Client();

	bool start(const char *con);
	bool start(const char *h,uint16_t p,uint32_t i,const char *n);
	void stop(bool kill=false);

	int send(uint8_t *d,size_t l);

	void setID(uint32_t i) { id = i; }
	uint32_t getID() { return id; }
	void setNick(const char *n);
	const char *getNick() { return nick; }
#ifndef SOCKET_NOCIPHER
	void setKey(const uint32_t *k,size_t l);
#endif /*SOCKET_NOCIPHER*/
};

}; /* namespace a */


using namespace a;


#endif /* _AMANITA_NET_CLIENT_H */


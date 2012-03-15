
#include "../_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


/*#ifdef USE_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#define http_close(s) SDLNet_TCP_Close(s)
#define http_send(s,d,l) SDLNet_TCP_Send((s),(d),(l))
#define http_recv(s,d,l) SDLNet_TCP_Recv((s),(d),(l))
#define http_clock SDL_GetTicks

typedef TCPsocket http_socket_t;
typedef int http_clock_t;
#endif*/

#ifdef USE_GTK
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>

#define http_close(s) ::close(s)
#define http_send(s,d,l) ::send((s),(d),(l),0)
#define http_recv(s,d,l) ::recv((s),(d),(l),0)
#define http_clock clock

typedef int http_socket_t;
typedef clock_t http_clock_t;
#endif

#ifdef USE_WIN32
#include <winsock2.h>
#include <time.h>

#define http_close(s) ::closesocket(s)
#define http_send(s,d,l) ::send((s),(d),(l),0)
#define http_recv(s,d,l) ::recv((s),(d),(l),0)
#define http_clock clock

typedef SOCKET http_socket_t;
typedef clock_t http_clock_t;
#endif

#include <amanita/aApplication.h>
#include <amanita/aString.h>
#include <amanita/aRandom.h>
#include <amanita/net/aHttp.h>


const char *http_methods[] = {
	"HEADER",
	"GET",
	"POST",
};

const char *http_headers[] = {
	"Accept",
	"Accept-Charset",
	"Accept-Encoding",
	"Accept-Language",
	"Accept-Ranges",
	"Age",
	"Allow",
	"Authorization",
	"Cache-Control",
	"Connection",
	"Content-Disposition",
	"Content-Encoding",
	"Content-Language",
	"Content-Length",
	"Content-Location",
	"Content-MD5",
	"Content-Range",
	"Content-Type",
	"Cookie",
	"Date",
	"ETag",
	"Expect",
	"Expires",
	"From",
	"Host",
	"If-Match",
	"If-Modified-Since",
	"If-None-Match",
	"If-Range",
	"If-Unmodified-Since",
	"Last-Modified",
	"Location",
	"Max-Forwards",
	"Pragma",
	"Proxy-Authenticate",
	"Proxy-Authorization",
	"Range",
	"Referer",
	"Refresh",
	"Retry-After",
	"Server",
	"Set-Cookie",
	"TE",
	"Trailer",
	"Transfer-Encoding",
	"Upgrade",
	"User-Agent",
	"Via",
	"Warn",
	"Warning",
	"WWW-Authenticate",
};

const char *http_mimes[] = {
	"application/x-www-form-urlencoded",
	"application/octet-stream",
	"multipart/form-data",
	"multipart/mixed",
};

struct packet {
	char *file;
	char *content;
	bool loaded;
	bool binary;
	void *data;
	long len;
};


aHttp::aHttp() : timeout(-1),multipart(0),ver(.0f),status(0) {}

aHttp::aHttp(aApplication &a) : timeout(-1),multipart(0),ver(.0f),status(0) {
	setUserAgent(a.getUserAgent());
}

aHttp::~aHttp() {
	clearForm();
}

void aHttp::setRequestHeader(const char *key,const char *value) {
	if(!key || !*key) return;
	if(value) headers.put(key,value);
	else if(headers.contains(key)) headers.remove(key);
}

void aHttp::setRequestHeader(HTTP_HEADER key,const char *value) {
	setRequestHeader(http_headers[key],value);
}

void aHttp::setFormValue(const char *key,const char *value, ...) {
	if(!key || !*key) return;
	removeFormValue(key);
	if(value) {
		aString val;
		va_list args;
		va_start(args,value);
		val.vappendf(value,args);
		va_end(args);
		form.put(key,val);
	}
}

void aHttp::setFormFile(const char *key,const char *file,const char *content,bool binary,void *data,size_t len) {
	if(!file || !*file) return;
	removeFormValue(file);
	if(*content && (!data || len>0)) {
		packet *p = (packet *)malloc(sizeof(packet));
		bool loaded = false;
		if(!data) {
			FILE *fp = fopen(file,binary? "rb" : "r");
			if(fp) {
				fseek(fp,0,SEEK_END);
				len = ftell(fp);
				if(len>0) {
					fseek(fp,0,SEEK_SET);
					data = malloc(len);
					if(binary) {
						size_t n;
						n = fread(data,len,1,fp);
					} else {
						int c;
						char *p1 = (char *)data;
						for(len=0; (c=fgetc(fp))!=EOF && c; *p1++ = c,len++);
						*p1 = '\0';
					}
					loaded = true;
				}
				fclose(fp);
			}
		}
		if(!data || len<=0) free(p);
		else {
			multipart++;
			*p = (packet){ strdup(file),strdup(content),loaded,binary,data,len };
			form.put(key,(void *)p);
		}
	}
}

void aHttp::removeFormValue(const char *key) {
	type_t type;
	void *p = (void *)form.remove(key,type);
	if(p && type==TYPE_VOID_P) {
		packet &pack = *(packet *)p;
		free(pack.file);
		free(pack.content);
		if(pack.loaded) free(pack.data);
		free(p);
		multipart--;
	}
}

void aHttp::clearForm() {
	if(form.size()==0) return;
	aHashtable::iterator iter = form.iterate();
	void *p;
	while((p=(void *)iter.next()))
		if(iter.valueType()==TYPE_VOID_P) {
			packet &pack = *(packet *)p;
			free(pack.file);
			free(pack.content);
			if(pack.loaded) free(pack.data);
			free(p);
		}
	form.removeAll();
	multipart = 0;
}


const char *aHttp::get(const char *host,const char *url) {
	return request(host,url,HTTP_METHOD_GET);
}

const char *aHttp::getf(const char *host,const char *url, ...) {
	aString str;
	va_list args;
	va_start(args,url);
	str.vappendf(url,args);
	va_end(args);
	return get(host,str.toCharArray());
}

const char *aHttp::post(const char *host,const char *url) {
	aString file;
	aHashtable::iterator iter = form.iterate();
	if(multipart>0) {
		aString boundary;
		boundary.append('-',(size_t)12);
		for(int i=0; i<12; i++) boundary.append(rnd.alphanum());
		void *p;
		char *p1;
		const char *s[] = {
			"--","\r\n","\r\n\r\n",": ",
			" boundary="," name=\""," filename=\"","form-data",
			"Content-Transfer-Encoding: binary",
		};
debug_output("Boundary 1: %s\n",boundary.toCharArray());
		while((p=(void *)iter.next())) {
			if(iter.valueType()==TYPE_VOID_P) p1 = (char *)((packet *)p)->data;
			else p1 = (char *)p;
			while((p1=strstr(p1,boundary.toCharArray()))) boundary.append(rnd.alphanum());
		}
		iter = form.iterate();
		while((p=(void *)iter.next())) if(iter.valueType()==TYPE_CHAR_P)
			file << s[0] << boundary << s[1] // "--".boundary."\r\n"
				<< http_headers[HTTP_CONTENT_DISPOSITION] << s[3] << s[7]
					<< ';' << s[5] << (const char *)iter.key() << '"' << s[2]
						<< (const char *)p << s[1];
		iter = form.iterate();
		while((p=(void *)iter.next())) if(iter.valueType()==TYPE_VOID_P) {
			file << s[0] << boundary << s[1] // "--".boundary."\r\n"
				<< http_headers[HTTP_CONTENT_DISPOSITION] << s[3] << s[7]
					<< ';' << s[5] << (const char *)iter.key() << '"'
						<< ';' << s[6] << ((packet *)p)->file << '"' << s[1]
							<< http_headers[HTTP_CONTENT_TYPE] << s[3] << ((packet *)p)->content << s[1];
			if(((packet *)p)->binary) file << s[8] << s[1];
			file << s[1];
			file.append((const char *)((packet *)p)->data,((packet *)p)->len);
			file << s[1];
		}
		file << s[0] << boundary << s[0] << s[1];
		char mime[32+boundary.length()];
		sprintf(mime,"%s;%s%s",http_mimes[HTTP_MULTIPART_FORM_DATA],s[4],boundary.toCharArray());
		headers.put(http_headers[HTTP_CONTENT_TYPE],mime);
	} else {
		aString key,value;
		if(form) while(iter.next()) {
			if(file) file << '&';
			key = (const char *)iter.key();
			key.encodeURL();
			value = (const char *)iter.value();
			value.encodeURL();
			file << key << '=' << value;
		}
		headers.put(http_headers[HTTP_CONTENT_TYPE],http_mimes[HTTP_FORM_URLENCODED]);
	}
	return request(host,url,HTTP_METHOD_POST,file.toCharArray(),file.length());
}

const char *aHttp::postf(const char *host,const char *url, ...) {
debug_output("aHttp::postf(host=%s,url=%s)\n",host,url);
	aString str;
	va_list args;
	va_start(args,url);
	str.vappendf(url,args);
	va_end(args);
debug_output("aHttp::postf(2: host=%s,url=%s)\n",host,str.toCharArray());
	return post(host,str.toCharArray());
}


const char *aHttp::request(const char *host,const char *url,HTTP_METHOD method,const char *data,size_t len) {
	response.removeAll();
	body.clear();

debug_output("aHttp::request(host=%s,url=%s)\n",host,url);

	int error = 0;
	http_clock_t t1 = http_clock(),t2 = t1,t3 = t1,t4 = t1,t5 = t1;

/*#ifdef USE_SDL
	IPaddress ip;
	TCPsocket sock = 0;
	SDLNet_SocketSet set = 0;
	if(!(set=SDLNet_AllocSocketSet(1))) error = 1;
	else if(SDLNet_ResolveHost(&ip,host,80)==-1) error = 2;
	else {
		if(!(sock=SDLNet_TCP_Open(&ip))) error = 3;
		else if(SDLNet_TCP_AddSocket(set,sock)==-1) error = 4;
		else {

#elif defined(USE_GTK) || defined(USE_WIN32)*/
	http_socket_t sock;
	hostent *hostinfo;
#ifdef USE_GTK
	sockaddr_in address;
#endif
#ifdef USE_WIN32
	SOCKADDR_IN address;
#endif
	fd_set set,test;
#ifdef USE_GTK
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1) error = 1;
#endif
#ifdef USE_WIN32
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET) error = 1;
#endif
	else if(!(hostinfo=gethostbyname(host))) error = 2;
	else {
		address.sin_addr = *(in_addr *)*hostinfo->h_addr_list;
		address.sin_family = AF_INET;
		address.sin_port = htons(80);

#ifdef USE_GTK
		if(connect(sock,(sockaddr *)&address,sizeof(address))<0) error = 3;
#endif
#ifdef USE_WIN32
		if(connect(sock,(SOCKADDR *)&address,sizeof(SOCKADDR_IN))!=0) error = 3;
#endif
		else {
			FD_ZERO(&set);
			FD_SET(sock,&set);
//#endif

			char buf[2049];
			t2 = http_clock();
			{
				if(!headers.contains(http_headers[HTTP_HOST])) headers.put(http_headers[HTTP_HOST],host);
				if(!headers.contains(http_headers[HTTP_CONNECTION])) headers.put(http_headers[HTTP_CONNECTION],"close");
				if(data && len==0) len = strlen(data);
				sprintf(buf,"%lu",(unsigned long)len);
				headers.put(http_headers[HTTP_CONTENT_LENGTH],buf);
				aString header(2048);
				header << http_methods[method] << " /" << url << " HTTP/1.1\r\n";
				aHashtable::iterator iter = headers.iterate();
				while(iter.next())
					header << (const char *)iter.key() << ": " << (const char *)iter.value() << "\r\n";
				header << "\r\n";
debug_output("aHttp::request(header=\"%s\",len=%lu)\n",header.toCharArray(),(unsigned long)header.length());
				if(data && len) {
debug_output("aHttp::request(data=\"%s\",len=%lu)\n",data,(unsigned long)len);
					http_send(sock,header.toCharArray(),header.length());
					http_send(sock,data,len+1);
				} else http_send(sock,header.toCharArray(),header.length()+1);
			}

			t3 = http_clock();

/*#ifdef USE_SDL
			if(SDLNet_CheckSockets(set,(uint32_t)-1)<=0) error = 5;
			else {

#elif defined(USE_GTK) || defined(USE_WIN32)*/
			test = set;
			select(FD_SETSIZE,&test,0,0,0);
			if(!FD_ISSET(sock,&test)) error = 5;
			else {
//#endif

				t4 = http_clock();
				int r = http_recv(sock,buf,2048);
debug_output("aHttp::request(response=\"%s\",r=%d)\n",buf,r);
				if(r<=0) error = 6;
				else {
					int i,n;
					char *p1 = strstr(buf,"\r\n\r\n"),*p2,*p3,*pb;
//debug_output("aHttp::request(p1=\"%s\")\n",p1);
					if(!p1) error = 7;
					else {
						bool chunked;
						p1[2] = '\0',pb = p1+4;
						response.put("Header",buf);

debug_output("aHttp::request(split header: %s)\n",buf);

						{ // Split header
							p1 = strstr(buf,"\r\n");
							while(p1) {
								p1 += 2;
								p2 = strstr(p1,": "),p3 = 0;
								if(p2) {
									p3 = strstr(p2,"\r\n");
									if(p3) {
										*p2 = '\0',p2 += 2,*p3 = '\0';
debug_output("aHttp::request(header: %s = %s)\n",p1,p2);
										response.put(p1,p2);
									}
								}
								p1 = p3;
							}
							response.put("HTTP",buf);
							if(!strncmp(buf,"HTTP/",5)) sscanf(buf,"HTTP/%f %d",&ver,&status);
						}

						p1 = (char *)response.getString(http_headers[HTTP_TRANSFER_ENCODING]);
debug_output("aHttp::request(encoding: %s)\n",p1);
						//chunked = p1? strcmp(p1,"identity")!=0 : true;
						p2 = (char *)response.getString(http_headers[HTTP_CONTENT_LENGTH]);
debug_output("aHttp::request(content length: %s)\n",p2);
						n = p2? atol(p2) : 0;
						chunked = p2? false : true;

						r -= (int)(pb-buf);
debug_output("aHttp::request(n=%d,r=%d,chunked=%d)\n",n,r,chunked);
						for(int chunks=1; 1;) {
//debug_output("aHttp::request(loop: n=%d,r=%d,len=%d)\n",n,r,body.length());
							if(n>0 && r>0) {
//debug_output("aHttp::request(append_data[1]: n=%d,r=%d,len=%d)\n",n,r,body.length());
								if(n>r) {
									body.append((const char *)pb,(size_t)r);
									n -= r;
									pb = buf;
									r = 0;
								} else {
									body.append((const char *)pb,(size_t)n);
									r -= n;
									pb += n;
									n = 0;
								}
//debug_output("aHttp::request(append_data[2]: n=%d,r=%d,len=%d)\n",n,r,body.length());
							}
							if(n==0) {
//debug_output("aHttp::request(read_length[1]: n=%d,r=%d,len=%d)\n",n,r,body.length());
								if(chunked) {
									if(r<32) {
										if(r>0) memmove(buf,pb,r);
										pb = buf;
										i = http_recv(sock,pb+r,2048-r);
										if(i<0) break;
										r += i;
										pb[r] = '\0';
									}
									if(chunks>1) pb += 2;
									n = aString::fromHex(pb);
//debug_output("aHttp::request(read_length: n=%d,r=%d,len=%d)\n",n,r,body.length());
									if(n==0) break;
									p1 = strstr(pb,"\r\n")+2;
									r -= p1-pb;
									pb = p1;
									body.increaseCapacity(n);
									chunks++;
								} else break;
							}
							if(n>0 && r==0) {
								pb = buf;
								while(n>0) {
									r = http_recv(sock,pb,2048);
									if(r<=0) break;
									pb[r] = '\0';
									if(n>2048) {
//debug_output("aHttp::request(read_data[1]: n=%d,r=%d,len=%d)\n",n,r,body.length());
										body.append((const char *)pb,(size_t)r);
										n -= r;
										r = 0;
//debug_output("aHttp::request(read_data[2]: n=%d,r=%d,len=%d)\n",n,r,body.length());
									} else break;
								}
								if(r<0) break;
							}
						}
							
/*
						body.setCapacity(len);
						n = (uint64_t)(r-(pb-buf));
						if(n>0) body.append(pb,n);
debug_output("aHttp::request(len=%lu),n=%d\n",len,n);

						if(len-n<=0) body.append('\0');
						else {
							for(n=len-n; 1; n-=r) {
								if(n<=0) {
									if(chunked) {
										r = http_recv(sock,buf,1024);
										if(r<=0) break;
										len = aString::fromHex(&buf[2]);
										p2 = strstr(&buf[2],"\r\n")+2;
										body.increaseCapacity(len);
										n = (uint64_t)(r-(p2-buf));
										if(n>0) body.append(p2,n);
										n = len-n;
									} else break;
								}
								r = http_recv(sock,buf,n>1024? 1024 : n);
								if(r<=0) break;
debug_output("aHttp::request(r=%d)\n",r);
								body.append(buf,r);
							}
						}
*/
						if(r<0) error = 8;
						t5 = http_clock();
					}
				}
			}

		}
		if(sock) http_close(sock);
	}
/*#ifdef USE_SDL
	if(set) SDLNet_FreeSocketSet(set);
#endif*/
	if(error>0) {
		body.clear();
/*#ifdef USE_SDL
		fprintf(stderr,"HTTP Error %d: %s\n",error,SDLNet_GetError());*/
#ifdef USE_GTK
		perror("HTTP Error");
#endif
#ifdef USE_WIN32
		fprintf(stderr,"HTTP Error %d: %d\n",error,WSAGetLastError());
#endif
		fflush(stderr);
		body.free();
	}
	headers.remove(http_headers[HTTP_HOST]);
	clearForm();

fprintf(stdout,"Time alltogether: %d\nTime to init: %d\nTime to send: %d\nTime until response: %d\nTime to download: %d\n"
	"header[%s]\nlen=%lu\nver=%.1f\nstatus=%d\n",
	(int)(t5-t1),(int)(t2-t1),(int)(t3-t2),(int)(t4-t3),(int)(t5-t4),response.getString("Header"),(unsigned long)body.length(),ver,status);
fflush(stdout);
	return body.toCharArray();
}

const char *aHttp::getResponseHeader(const char *key) {
	return response.getString(key);
}

const char *aHttp::getResponseHeader(HTTP_HEADER key) {
	if(key>=HTTP_ACCEPT && key<=HTTP_USER_AGENT) return response.getString(http_headers[key]);
	return 0;
}



#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#ifdef LIBAMANITA_SDL
	#include <SDL/SDL.h>
	#include <SDL/SDL_net.h>

	#define http_close(s) SDLNet_TCP_Close(s)
	#define http_send(s,d,l) SDLNet_TCP_Send((s),(d),(l))
	#define http_recv(s,d,l) SDLNet_TCP_Recv((s),(d),(l))
	#define http_clock SDL_GetTicks

	typedef TCPsocket http_socket_t;
	typedef int http_clock_t;

#elif defined __linux__
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

#endif /* LIBAMANITA_SDL */

#include <libamanita/aString.h>
#include <libamanita/aRandom.h>
#include <libamanita/net/aHttp.h>


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


aHttp::aHttp() {
	timeout = -1;
	multipart = 0;
	ver = .0f;
	status = 0;
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
fprintf(stderr,"Boundary 1: %s\n",boundary.toCharArray());
		while((p=(void *)iter.next())) {
			if(iter.valueType()==TYPE_VOID_P) p1 = (char *)((packet *)p)->data;
			else p1 = (char *)p;
			while((p1=strstr(p1,boundary.toCharArray()))) boundary.append(rnd.alphanum());
		}
		iter = form.iterate();
		while((p=(void *)iter.next())) if(iter.valueType()==TYPE_CHAR_P)
			file.append(s[0]).append(boundary).append(s[1]). // "--".boundary."\r\n"
				append(http_headers[HTTP_CONTENT_DISPOSITION]).append(s[3]).append(s[7]).
					append(';').append(s[5]).append((const char *)iter.key()).append('"').append(s[2]).
						append((const char *)p).append(s[1]);
		iter = form.iterate();
		while((p=(void *)iter.next())) if(iter.valueType()==TYPE_VOID_P) {
			file.append(s[0]).append(boundary).append(s[1]). // "--".boundary."\r\n"
				append(http_headers[HTTP_CONTENT_DISPOSITION]).append(s[3]).append(s[7]).
					append(';').append(s[5]).append((const char *)iter.key()).append('"').
						append(';').append(s[6]).append(((packet *)p)->file).append('"').append(s[1]).
							append(http_headers[HTTP_CONTENT_TYPE]).append(s[3]).append(((packet *)p)->content).append(s[1]);
			if(((packet *)p)->binary) file.append(s[8]).append(s[1]);
			file.append(s[1]).append((const char *)((packet *)p)->data,((packet *)p)->len).append(s[1]);
		}
		file.append(s[0]).append(boundary).append(s[0]).append(s[1]);
		char mime[32+boundary.length()];
		sprintf(mime,"%s;%s%s",http_mimes[HTTP_MULTIPART_FORM_DATA],s[4],boundary.toCharArray());
		headers.put(http_headers[HTTP_CONTENT_TYPE],mime);
	} else {
		aString key,value;
		if(form.size()>0) while(iter.next()) {
			if(file.length()>0) file.append('&');
			key = (const char *)iter.key(),value = (const char *)iter.value();
			file.append(key.encodeURL()).append("=").append(value.encodeURL());
		}
		headers.put(http_headers[HTTP_CONTENT_TYPE],http_mimes[HTTP_FORM_URLENCODED]);
	}
	return request(host,url,HTTP_METHOD_POST,file.toCharArray(),file.length());
}

const char *aHttp::postf(const char *host,const char *url, ...) {
fprintf(stderr,"aHttp::postf(host=%s,url=%s)\n",host,url);
fflush(stderr);
	aString str;
	va_list args;
	va_start(args,url);
	str.vappendf(url,args);
	va_end(args);
fprintf(stderr,"aHttp::postf(2: host=%s,url=%s)\n",host,str.toCharArray());
fflush(stderr);
	return post(host,str.toCharArray());
}


const char *aHttp::request(const char *host,const char *url,HTTP_METHOD method,const char *data,size_t len) {
	response.removeAll();
	body.clear();

fprintf(stderr,"aHttp::request(host=%s,url=%s)\n",host,url);
fflush(stderr);

	int error = 0;
	http_clock_t t1 = http_clock(),t2 = t1,t3 = t1,t4 = t1,t5 = t1;

#ifdef LIBAMANITA_SDL
	IPaddress ip;
	TCPsocket sock = 0;
	SDLNet_SocketSet set = 0;
	if(!(set=SDLNet_AllocSocketSet(1))) error = 1;
	else if(SDLNet_ResolveHost(&ip,host,80)==-1) error = 2;
	else {
		if(!(sock=SDLNet_TCP_Open(&ip))) error = 3;
		else if(SDLNet_TCP_AddSocket(set,sock)==-1) error = 4;
		else {

#elif defined __linux__
	int sock;
	hostent *hostinfo;
	sockaddr_in address;
	fd_set set,test;
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1) error = 1;
	else if(!(hostinfo=gethostbyname(host))) error = 2;
	else {
		address.sin_addr = *(in_addr *)*hostinfo->h_addr_list;
		address.sin_family = AF_INET;
		address.sin_port = htons(80);

		if(connect(sock,(sockaddr *)&address,sizeof(address))<0) error = 3;
		else {
			FD_ZERO(&set);
			FD_SET(sock,&set);
#endif /* LIBAMANITA_SDL */

			char buf[2049];
			t2 = http_clock();
			{
				if(!headers.contains(http_headers[HTTP_HOST])) headers.put(http_headers[HTTP_HOST],host);
				if(!headers.contains(http_headers[HTTP_CONNECTION])) headers.put(http_headers[HTTP_CONNECTION],"close");
				if(data && len==0) len = strlen(data);
				sprintf(buf,"%zu",len);
				headers.put(http_headers[HTTP_CONTENT_LENGTH],buf);
				aString header(2048);
				header.append(http_methods[method]).append(" /").append(url).append(" HTTP/1.1\r\n");
				aHashtable::iterator iter = headers.iterate();
				while(iter.next())
					header.append((const char *)iter.key()).append(": ").append((const char *)iter.value()).append("\r\n");
				header.append("\r\n");
fprintf(stderr,"aHttp::request(header=\"%s\",len=%zu)\n",header.toCharArray(),header.length());
fflush(stderr);
				if(data && len) {
fprintf(stderr,"aHttp::request(data=\"%s\",len=%zu)\n",data,len);
fflush(stderr);
					http_send(sock,header.toCharArray(),header.length());
					http_send(sock,data,len+1);
				} else http_send(sock,header.toCharArray(),header.length()+1);
			}

			t3 = http_clock();

#ifdef LIBAMANITA_SDL
			if(SDLNet_CheckSockets(set,(uint32_t)-1)<=0) error = 5;
			else {

#elif defined __linux__
			test = set;
			select(FD_SETSIZE,&test,0,0,0);
			if(!FD_ISSET(sock,&test)) error = 5;
			else {
#endif /* LIBAMANITA_SDL */

				t4 = http_clock();
				int r = http_recv(sock,buf,2048);
fprintf(stderr,"aHttp::request(response=\"%s\",r=%d)\n",buf,r);
fflush(stderr);
				if(r<=0) error = 6;
				else {
					int i,n;
					char *p1 = strstr(buf,"\r\n\r\n"),*p2,*p3,*pb;
//fprintf(stderr,"aHttp::request(p1=\"%s\")\n",p1);
//fflush(stderr);
					if(!p1) error = 7;
					else {
						bool chunked;
						p1[2] = '\0',pb = p1+4;
						response.put("Header",buf);

fprintf(stderr,"aHttp::request(split header: %s)\n",buf);
fflush(stderr);

						{ // Split header
							p1 = strstr(buf,"\r\n");
							while(p1) {
								p1 += 2;
								p2 = strstr(p1,": "),p3 = 0;
								if(p2) {
									p3 = strstr(p2,"\r\n");
									if(p3) {
										*p2 = '\0',p2 += 2,*p3 = '\0';
fprintf(stderr,"aHttp::request(header: %s = %s)\n",p1,p2);
fflush(stderr);
										response.put(p1,p2);
									}
								}
								p1 = p3;
							}
							response.put("HTTP",buf);
							if(!strncmp(buf,"HTTP/",5)) sscanf(buf,"HTTP/%f %d",&ver,&status);
						}

						p1 = response.getString(http_headers[HTTP_TRANSFER_ENCODING]);
fprintf(stderr,"aHttp::request(encoding: %s)\n",p1);
fflush(stderr);
						//chunked = p1? strcmp(p1,"identity")!=0 : true;
						p2 = response.getString(http_headers[HTTP_CONTENT_LENGTH]);
fprintf(stderr,"aHttp::request(content length: %s)\n",p2);
fflush(stderr);
						n = p2? atol(p2) : 0;
						chunked = p2? false : true;

						r -= (int)(pb-buf);
fprintf(stderr,"aHttp::request(n=%d,r=%d,chunked=%d)\n",n,r,chunked);
fflush(stderr);
						for(int chunks=1; 1;) {
//fprintf(stderr,"aHttp::request(loop: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
							if(n>0 && r>0) {
//fprintf(stderr,"aHttp::request(append_data[1]: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
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
//fprintf(stderr,"aHttp::request(append_data[2]: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
							}
							if(n==0) {
//fprintf(stderr,"aHttp::request(read_length[1]: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
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
//fprintf(stderr,"aHttp::request(read_length: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
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
//fprintf(stderr,"aHttp::request(read_data[1]: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
										body.append((const char *)pb,(size_t)r);
										n -= r;
										r = 0;
//fprintf(stderr,"aHttp::request(read_data[2]: n=%d,r=%d,len=%d)\n",n,r,body.length());
//fflush(stderr);
									} else break;
								}
								if(r<0) break;
							}
						}
							
/*
						body.setCapacity(len);
						n = (uint64_t)(r-(pb-buf));
						if(n>0) body.append(pb,n);
fprintf(stderr,"aHttp::request(len=%lu),n=%d\n",len,n);
fflush(stderr);

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
fprintf(stderr,"aHttp::request(r=%d)\n",r);
fflush(stderr);
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
#ifdef LIBAMANITA_SDL
	if(set) SDLNet_FreeSocketSet(set);
#elif defined __linux__
#endif /* LIBAMANITA_SDL */
	if(error>0) {
		body.clear();
#ifdef LIBAMANITA_SDL
		fprintf(stderr,"HTTP Error %d: %s\n",error,SDLNet_GetError());
#elif defined __linux__
		perror("HTTP Error");
#endif /* LIBAMANITA_SDL */
		fflush(stderr);
		body.free();
	}
	headers.remove(http_headers[HTTP_HOST]);
	clearForm();

fprintf(stderr,"Time alltogether: %d\nTime to init: %d\nTime to send: %d\nTime until response: %d\nTime to download: %d\n"
	"header[%s]\nlen=%zu\nver=%.1f\nstatus=%d\n",
	(int)(t5-t1),(int)(t2-t1),(int)(t3-t2),(int)(t4-t3),(int)(t5-t4),response.getString("Header"),body.length(),ver,status);
fflush(stderr);
	return body.toCharArray();
}

const char *aHttp::getResponseHeader(const char *key) {
	return response.getString(key);
}

const char *aHttp::getResponseHeader(HTTP_HEADER key) {
	if(key>=HTTP_ACCEPT && key<=HTTP_USER_AGENT) return response.getString(http_headers[key]);
	return 0;
}


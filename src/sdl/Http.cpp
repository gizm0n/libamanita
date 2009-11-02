
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <libamanita/String.h>
#include <libamanita/Random.h>
#include <libamanita/sdl/Http.h>


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
	"Date",
	"Expect",
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
	"Proxy-Authorization",
	"Range",
	"Referer",
	"Server",
	"TE",
	"User-Agent",
};

const char *http_mimes[] = {
	"application/x-www-form-urlencoded",
	"application/octet-stream",
	"multipart/form-data",
	"multipart/mixed",
};


Http::Http() {
	timeout = -1;
	multipart = 0;
	ver = .0f;
	status = 0;
}

Http::~Http() {
	clearForm();
}

void Http::setRequestHeader(const char *key,const char *value) {
	if(!key || !*key) return;
	if(value) headers.put(key,value);
	else if(headers.contains(key)) headers.remove(key);
}

void Http::setRequestHeader(HTTP_HEADER key,const char *value) {
	setRequestHeader(http_headers[key],value);
}

void Http::setFormValue(const char *key,const char *value, ...) {
	if(!key || !*key) return;
	removeFormValue(key);
	if(value) {
		String val;
		va_list args;
		va_start(args,value);
		val.vappendf(value,args);
		va_end(args);
		form.put(key,val);
	}
}

void Http::setFormFile(const char *key,const char *file,const char *content,bool binary,void *data,size_t len) {
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

void Http::removeFormValue(const char *key) {
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

void Http::clearForm() {
	if(form.size()==0) return;
	Hashtable::iterator iter = form.iterate();
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


const char *Http::get(const char *host,const char *url) {
	return request(host,url,HTTP_METHOD_GET);
}

const char *Http::getf(const char *host,const char *url, ...) {
	String u;
	va_list args;
   va_start(args,url);
	u.vappendf(url,args);
   va_end(args);
	return get(host,u.toString());
}

const char *Http::post(const char *host,const char *url) {
	String file;
	Hashtable::iterator iter = form.iterate();
	if(multipart>0) {
		String boundary;
		boundary.append('-',(size_t)12);
		for(int i=0; i<12; i++) boundary.append(rnd.alphanum());
		void *p;
		char *p1;
		const char *s[] = {
			"--","\r\n","\r\n\r\n",": ",
			" boundary="," name=\""," filename=\"","form-data",
			"Content-Transfer-Encoding: binary",
		};
fprintf(stderr,"Boundary 1: %s\n",boundary.toString());
		while((p=(void *)iter.next())) {
			if(iter.valueType()==TYPE_VOID_P) p1 = (char *)((packet *)p)->data;
			else p1 = (char *)p;
			while((p1=strstr(p1,boundary.toString()))) boundary.append(rnd.alphanum());
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
		sprintf(mime,"%s;%s%s",http_mimes[HTTP_MULTIPART_FORM_DATA],s[4],boundary.toString());
		headers.put(http_headers[HTTP_CONTENT_TYPE],mime);
	} else {
		String key,value;
		if(form.size()>0) while(iter.next()) {
			if(file.length()>0) file.append('&');
			key = (const char *)iter.key(),value = (const char *)iter.value();
			file.append(key.encodeURL()).append("=").append(value.encodeURL());
		}
		headers.put(http_headers[HTTP_CONTENT_TYPE],http_mimes[HTTP_FORM_URLENCODED]);
	}
	return request(host,url,HTTP_METHOD_POST,file.toString(),file.length());
}

const char *Http::postf(const char *host,const char *url, ...) {
	String u;
	va_list args;
   va_start(args,url);
	u.vappendf(url,args);
   va_end(args);
	return post(host,u.toString());
}


const char *Http::request(const char *host,const char *url,HTTP_METHOD method,const char *data,size_t len) {
	response.removeAll();
	body.clear();

fprintf(stderr,"Http::get(host=%s,url=%s)\n",host,url);
fflush(stderr);

	int error = 0;
	IPaddress ip;
	TCPsocket sock = 0;
	SDLNet_SocketSet set = 0;
	int t1 = SDL_GetTicks(),t2 = t1,t3 = t1,t4 = t1,t5 = t1;
	if(!(set=SDLNet_AllocSocketSet(1))) error = 1;
	else if(SDLNet_ResolveHost(&ip,host,80)==-1) error = 2;
	else {
		if(!(sock=SDLNet_TCP_Open(&ip))) error = 3;
		else if(SDLNet_TCP_AddSocket(set,sock)==-1) error = 4;
		else {
			char buf[1025];
			t2 = SDL_GetTicks();
			{
				if(!headers.contains(http_headers[HTTP_HOST])) headers.put(http_headers[HTTP_HOST],host);
				if(!headers.contains(http_headers[HTTP_CONNECTION])) headers.put(http_headers[HTTP_CONNECTION],"close");
				if(data && len==0) len = strlen(data);
				sprintf(buf,"%zu",len);
				headers.put(http_headers[HTTP_CONTENT_LENGTH],buf);
				String header(1024);
				header.append(http_methods[method]).append(" /").append(url).append(" HTTP/1.1\r\n");
				Hashtable::iterator iter = headers.iterate();
				while(iter.next())
					header.append((const char *)iter.key()).append(": ").append((const char *)iter.value()).append("\r\n");
				header.append("\r\n");
fprintf(stderr,"Http::request(header=\"%s\",len=%zu)\n",header.toString(),header.length());
fflush(stderr);
				if(data && len) {
fprintf(stderr,"Http::request(data=\"%s\",len=%zu)\n",data,len);
fflush(stderr);
					SDLNet_TCP_Send(sock,header.toString(),header.length());
					SDLNet_TCP_Send(sock,data,len+1);
				} else SDLNet_TCP_Send(sock,header.toString(),header.length()+1);
			}

			t3 = SDL_GetTicks();
			if(SDLNet_CheckSockets(set,(Uint32)-1)<=0) error = 5;
			else {
				t4 = SDL_GetTicks();
				int r = SDLNet_TCP_Recv(sock,buf,1024);
fprintf(stderr,"Http::request(response=\"%s\")\n",buf);
fflush(stderr);
				if(r<=0) error = 6;
				else {
					int i,m,n;
					char *p1 = strstr(buf,"\r\n\r\n"),*p2,*p3,*pb;
					if(!p1) error = 7;
					else {
						bool chunked;
						p1[2] = '\0',pb = p1+4;
						response.put("Header",buf);

						{ // Split header
							p1 = strstr(buf,"\r\n");
							while(p1) {
								p2 = strstr(p1,": "),p3 = 0;
								if(p2) {
									p3 = strstr(p2,"\r\n");
									if(p3) {
										*p2 = '\0',p2 += 2,*p3 = '\0',p3 += 2;
										response.put(p1,p2);
									}
								}
								p1 = p3;
							}
							response.put("HTTP",buf);
							if(!strncmp(buf,"HTTP/",5)) sscanf(buf,"HTTP/%f %d",&ver,&status);
						}

						p1 = response.getString(http_headers[HTTP_CONTENT_ENCODING]);
						chunked = strcmp(p1,"identity")!=0;
						p2 = response.getString(http_headers[HTTP_CONTENT_LENGTH]);
						n = p2? atol(p2) : 0;

						r -= (int)(pb-buf);
						for(i=0; 1; i++) {
							if(n>0 && r>0) {
fprintf(stderr,"Http::request(n=%d,r=%d,pb=%s)\n",n,r,pb);
fflush(stderr);
								if(n>r) {
									body.append(pb,r);
									n -= r;
									pb = buf;
									r = 0;
								} else {
									body.append(pb,n);
									r -= n;
									pb += n;
									n = 0;
								}
							}
							if(n==0) {
								if(chunked || i==0) {
									if(r<32) {
										if(r>0) memmove(buf,pb,r);
										pb = buf;
										m = SDLNet_TCP_Recv(sock,pb+r,1024-r);
										if(m<0) break;
										r += m;
									}
									pb += 2;
									n = String::fromHex(pb);
fprintf(stderr,"Http::request(read_length: n=%d,r=%d,pb=%s)\n",n,r,pb);
fflush(stderr);
									if(n==0) break;
									pb = strstr(pb,"\r\n")+2;
									body.increaseCapacity(n);
								} else break;
							}
							if(n>0 && r==0) {
								pb = buf;
								while(n>0) {
									r = SDLNet_TCP_Recv(sock,pb,1024);
									if(r<=0) break;
									if(n>1024) {
fprintf(stderr,"Http::request(n=%d,r=%d,pb=%s)\n",n,r,pb);
fflush(stderr);
										body.append(pb,r);
										n -= r;
										r = 0;
									} else break;
								}
							}
						}
							
/*
						body.setCapacity(len);
						n = (uint64_t)(r-(pb-buf));
						if(n>0) body.append(pb,n);
fprintf(stderr,"Http::request(len=%lu),n=%d\n",len,n);
fflush(stderr);

						if(len-n<=0) body.append('\0');
						else {
							for(n=len-n; 1; n-=r) {
								if(n<=0) {
									if(chunked) {
										r = SDLNet_TCP_Recv(sock,buf,1024);
										if(r<=0) break;
										len = String::fromHex(&buf[2]);
										p2 = strstr(&buf[2],"\r\n")+2;
										body.increaseCapacity(len);
										n = (uint64_t)(r-(p2-buf));
										if(n>0) body.append(p2,n);
										n = len-n;
									} else break;
								}
								r = SDLNet_TCP_Recv(sock,buf,n>1024? 1024 : n);
								if(r<=0) break;
fprintf(stderr,"Http::request(r=%d)\n",r);
fflush(stderr);
								body.append(buf,r);
							}
						}
*/
						if(r<=0) error = 8;
						t5 = SDL_GetTicks();
					}
				}
			}

		}
		if(sock) SDLNet_TCP_Close(sock);
	}
	if(set) SDLNet_FreeSocketSet(set);
	if(error>0) {
		body.clear();
		fprintf(stderr,"Error %d: %s\n",error,SDLNet_GetError());
		fflush(stderr);
		body.free();
	}
	headers.remove(http_headers[HTTP_HOST]);
	clearForm();

fprintf(stderr,"Time alltogether: %d\nTime to init: %d\nTime to send: %d\nTime until response: %d\nTime to download: %d\n"
	"header[%s]\nbody[%s]\nlen=%zu\nver=%.1f\nstatus=%d\n",
	t5-t1,t2-t1,t3-t2,t4-t3,t5-t4,response.getString("Header"),body.toString(),body.length(),ver,status);
fflush(stderr);
	return body.toString();
}

const char *Http::getResponseHeader(const char *key) {
	return response.getString(key);
}

const char *Http::getResponseHeader(HTTP_HEADER key) {
	if(key>=HTTP_ACCEPT && key<=HTTP_USER_AGENT) return response.getString(http_headers[key]);
	return 0;
}



#include "_config.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <amanita/aString.h>


aObject_Inheritance(aString,aObject);


const char aString::upper_hex[17] = "0123456789ABCDEF";
const char aString::lower_hex[17] = "0123456789abcdef";

const char *aString::blank = "";

#ifdef __linux__
const char *aString::endline = "\n";
#elif defined _WIN32
const char *aString::endline = "\r\n";
#endif
const char *aString::whitespace = " \t\n\r";


aString::aString(size_t c) : aObject(),str(0),len(0),cap(0) {
	if(c>0) setCapacity(c);
}

aString::aString(const char *s,size_t l) : aObject() {
	if(s && *s) {
		if(!l) len = strlen(s);
		cap = len;
		str = (char *)malloc(cap+1);
		if(cap) memcpy(str,s,len+1);
		else *str = '\0';
	} else str = 0,len = 0,cap = 0;
}

aString::aString(aString *s) : aObject() {
	if(!s || !s->str) str = 0,len = 0,cap = 0;
	else {
		len = s->len,cap = s->cap,str = (char *)malloc(cap+1);
		if(*s->str) memcpy(str,s->str,len+1);
		else *str = '\0';
	}
}

aString::aString(aString &s) : aObject() {
	if(!s.str) str = 0,len = 0,cap = 0;
	else {
		len = s.len,cap = s.cap,str = (char *)malloc(cap+1);
		if(*s.str) memcpy(str,s.str,len+1);
		else *str = '\0';
	}
}

aString::~aString() {
	::free(str);
	str = 0,len = 0,cap = 0;
}

aString &aString::append(char c) {
	if(len==cap) resize(0);
	if(c!='\0') str[len++] = c;
	str[len] = '\0';
	return *this;
}
aString &aString::append(char c,size_t n) {
	resize(n);
	while(n--) str[len++] = c;
	str[len] = '\0';
	return *this;
}

aString &aString::append(const char *s,size_t l) {
	if(s) {
		if(!l) l = strlen(s);
		resize(l);
		memcpy(&str[len],s,l);
		len += l;
		str[len] = '\0';
	}
	return *this;
}
aString &aString::append(const char *s,size_t l,size_t n) {
	if(s && n>0) {
		if(!l) l = strlen(s);
		resize(l*n);
		while(n--) {
			memcpy(&str[len],s,l);
			len += l;
		}
		str[len] = '\0';
	}
	return *this;
}

#if __WORDSIZE < 64
aString &aString::appendi32(int32_t i) {
	if(i==0) return append('0');
	char s[13],*p = s+12,c = 0;
	*p-- = '\0';
	if(i<0) c = '-',i = -i;
	for(; i; i/=10) *p-- = '0'+(i%10);
	if(c) *p-- = c;
	return append(p+1);
}

aString &aString::appendu32(uint32_t i) {
	if(i==0) return append('0');
	char s[12],*p = s+11;
	*p-- = '\0';
	for(; i; i/=10) *p-- = '0'+(i%10);
	return append(p+1);
}
#endif

aString &aString::appendi64(int64_t i) {
	if(i==0) return append('0');
	char s[22],*p = s+21,c = 0;
	*p-- = '\0';
	if(i<0) c = '-',i = -i;
	for(; i; i/=10) *p-- = '0'+(i%10);
	if(c) *p-- = c;
	return append(p+1);
}

aString &aString::appendu64(uint64_t i) {
	if(i==0) return append('0');
	char s[21],*p = s+20;
	*p-- = '\0';
	for(; i; i/=10) *p-- = '0'+(i%10);
	return append(p+1);
}

aString &aString::append(double f,int n,char c) {
	int64_t n1 = (int64_t)f;
	uint32_t n2,m = 1;
	if(n>10) n = 10;
	while(0<n--) m *= 10;
	n2 = (uint32_t)round(fabs(f-n1)*m);
	return append(n1).append(c).append(n2);
}

aString &aString::appendHex(uint64_t i,bool upper) {
	if(i==0) return append('0');
	char h[17],*p = h+16;
	const char *s = upper? upper_hex : lower_hex;
	*p-- = '\0';
	for(; i; i>>=4) *p-- = s[i&0xf];
	return append(p+1);
}

aString &aString::appendBase(int64_t i,int base) {
	if(i==0) return append('0');
	char s[22],*p = s+21,c = 0,n;
	*p-- = '\0';
	if(i<0) c = '-',i = -i;
	for(; i; i/=base) n = i%base,*p-- = n<=9? '0'+n : 'A'+n-10;
	if(c) *p-- = c;
	return append(p+1);
}

aString &aString::appendBase(uint64_t i,int base) {
	if(i==0) return append('0');
	char s[22],*p = s+21,n;
	*p-- = '\0';
	for(; i; i/=base) n = i%base,*p-- = n<=9? '0'+n : 'A'+n-10;
	return append(p+1);
}

aString &aString::appendf(const char *f, ...) {
	va_list list;
	va_start(list,f);
	vappendf(f,list);
	va_end(list);
	return *this;
}

enum {
	F_PLUS			= 0x0001,
	F_MINUS			= 0x0002,
	F_HASH			= 0x0008,
	F_PRECISION		= 0x0080,
	F_h				= 0x0200,
	F_l				= 0x0400,
	F_ll				= 0x0800,
	F_L				= 0x1000,
	F_z				= 0x2000,
};

aString &aString::vappendf(const char *f,va_list list) {
	if(!f || !*f) return *this;
	int c;
	uint32_t flags,*w,w1,w2,w2default,l;
	char pad;
	while(1) {
debug_output("aString::vappendf(f=%s)\n",f);
		while((c=*f++) && c!='%') {
			if(len==cap) resize(0);
			str[len++] = c;
		}
		if(c=='\0') break;
		c = *f++,flags = 0,w=&w1,w1=0,w2=0,w2default=2,pad = ' ';
		while(c) {
			if(c=='0') pad = '0';
			else if(c==' ') pad = ' ';
			else if(c=='-') flags |= F_MINUS;
			else if(c=='+') flags |= F_PLUS;
			else if(c=='#') flags |= F_HASH;
			else if(c=='.') flags |= F_PRECISION,w = &w2;
			else if(c>='0' && c<='9') {
				*w = *w*10+(c-'0');
				if(flags&F_PRECISION) w2default = 0;
			} else if(c=='*') *w = va_arg(list,int);
			else if(c=='h') flags |= F_h;
			else if(c=='l') {
				if(flags&F_l) flags |= F_ll;
				else flags |= F_l;
			} else if(c=='L') flags |= F_L;
			else if(c=='z') flags |= F_z;
			else break;
			c = *f++;
		}
		switch(c) {
			case '%':
			case 'c':
			case 'C':
				if(c=='c' || c=='C') c = va_arg(list,int);
				if(w1>0) append((const char)c,w1);
				else append(c);
				break;
			case 'd':
			case 'D':
			case 'i':
			case 'I':
			{
				if(flags&F_z) {
					size_t n = va_arg(list,size_t);
					if((flags&F_PLUS) && n>=0) append('+');
					append(n);
				} else if(flags&F_ll) {
					long long n = va_arg(list,long long);
					if((flags&F_PLUS) && n>=0) append('+');
					append(n);
				} else if(flags&F_l) {
					long n = va_arg(list,long);
					if((flags&F_PLUS) && n>=0) append('+');
					append(n);
				} else {
					int n = va_arg(list,int);
					if((flags&F_PLUS) && n>=0) append('+');
					append(n);
				}
				break;
			}
			case 'u':
			case 'U':
			{
				if(flags&F_PLUS) append('+');
				if(flags&F_z) {
					size_t n = va_arg(list,size_t);
					append(n);
				} else if(flags&F_ll) {
					unsigned long long n = va_arg(list,unsigned long long);
					append(n);
				} else if(flags&F_l) {
					unsigned long n = va_arg(list,unsigned long);
					append(n);
				} else {
					unsigned int n = va_arg(list,unsigned int);
					append(n);
				}
				break;
			}
			case 'x':
			case 'X':
			{
				uint64_t n = 0;
				if(flags&F_PLUS) append('+');
				if(flags&F_z) n = (uint64_t)va_arg(list,size_t);
				else if(flags&F_ll) n = (uint64_t)va_arg(list,unsigned long long);
				else if(flags&F_l) n = (uint64_t)va_arg(list,unsigned long);
				else n = (uint64_t)va_arg(list,unsigned int);
				appendHex(n,16);
				break;
			}
			case 'f':
			case 'F':
			{
				double n = va_arg(list,double);
debug_output("n=%f,flags=%" PRIx32 "\n",n,flags);
				if((flags&F_PLUS) && n>=0.) append('+');
				append(n,(int)(w2default? ((flags&F_L)? 6 : w2default) : w2));
				break;
			}
			case 's':
			case 'S':
			{
				char *s = va_arg(list,char *);
debug_output("s=%s\n",s);
				l = strlen(s);
				if(w2 && l>w2) l = w2;
				if(w1 && !(flags&F_MINUS) && l<w1) append(pad,w1-l);
				append(s,l);
				if(w1 && (flags&F_MINUS) && l<w1) append(pad,w1-l);
				break;
			}
		}
	}
	str[len] = '\0';
	return *this;
}

aString &aString::appendUntil(const char **s,const char *end,const char *trim,bool uesc) {
	if(!s || !*s || !**s) return *this;
	if(!trim) trim = end;
	if(!end || !*end) {
		append(*s);
		*s += strlen(*s);
		return *this;
	}
	const char *s2 = *s;
	int c,c2 = -1,t = trim && *trim? 0 : 1;
	unsigned long l2 = 0;
//debug_output("aString::appendUntil: ");
	while((c=*s2++)!='\0') {
//debug_putc(c);
		if(uesc) {
			if(c=='/') {
				c = *s2++;
				if(c=='/') {
					while((c=*s2++)!='\0' && c!='\r' && c!='\n');
					if(c=='\0') break;
					s2--;
					continue;
				} else if(c=='*') {
					c2 = -1;
					while((c=*s2++)!='\0') { if(c=='/' && c2=='*') break;c2 = c; }
					c2 = -1;
					continue;
				} else s2--,c = '/';
			} else if(c=='\\') {
				if((c=*s2++)=='\0') break;
				else if(c=='\\') c2 = '\\';
				else if(c=='n') c2 = '\n';
				else if(c=='t') c2 = '\t';
				else if(c=='r') c2 = '\r';
				else if(c=='f') c2 = '\f';
				else if(c=='v') c2 = '\v';
				else if(c=='b') c2 = '\b';
				else if(c=='a') c2 = '\a';
				else if(c=='0') c2 = '\0';
				else if(c=='\n' || c=='\r') {
					if((c2=*s2++)=='\0') break;
					else if((c=='\n' && c2!='\r') || (c=='\r' && c2!='\n')) s2--;
					c2 = -1;
					continue;
				}
				else c2 = c;
			}
		}
		if(!t && !strchr(trim,c)) t = 1;
		if(t) {
			if(end && strchr(end,c)) break;
			if(len==cap) resize(0);
			if(c2!=-1) str[len++] = (char)c2,c2 = -1,l2 = len;
			else str[len++] = (char)c;
		}
	}
	if(trim && *trim!='\0') while(len>l2 && strchr(trim,str[len-1])) len--;
	str[len] = '\0';
//debug_output("[%s]\n",str);
	*s = s2;
	return *this;
}

aString &aString::appendUntil(FILE *fp,const char *end,const char *trim,bool uesc) {
	if(!fp) return *this;
	if(!trim) trim = end;
	if(!end) {
		fpos_t pos;
		if(!fgetpos(fp,&pos)) {
			long fpos = ftell(fp);
			fseek(fp,0,SEEK_END);
			long fend = ftell(fp);
			if(fpos>=0 && fend>=0 && fend-fpos>(long)cap) resize(fend-fpos);
			fsetpos(fp,&pos);
		}
	}
	int c,c2 = -1,t = trim && *trim? 0 : 1;
	unsigned long l2 = 0;
//debug_output("aString::appendUntil: ");
	while((c=fgetc(fp)) && c!=EOF) {
//debug_putc(c);
		if(uesc) {
			if(c=='/') {
				c = fgetc(fp);
				if(c=='/') {
					while((c=fgetc(fp))!=EOF && c!='\r' && c!='\n');
					if(c==EOF) break;
					ungetc(c,fp);
					continue;
				} else if(c=='*') {
					c2 = -1;
					while((c=fgetc(fp))!=EOF) { if(c=='/' && c2=='*') break;c2 = c; }
					c2 = -1;
					continue;
				} else { ungetc(c,fp);c = '/'; }
			} else if(c=='\\') {
				if((c=fgetc(fp))==EOF) break;
				else if(c=='\\') c2 = '\\';
				else if(c=='n') c2 = '\n';
				else if(c=='t') c2 = '\t';
				else if(c=='r') c2 = '\r';
				else if(c=='f') c2 = '\f';
				else if(c=='v') c2 = '\v';
				else if(c=='b') c2 = '\b';
				else if(c=='a') c2 = '\a';
				else if(c=='0') c2 = '\0';
				else if(c=='\n' || c=='\r') {
					if((c2=fgetc(fp))==EOF) break;
					else if((c=='\n' && c2!='\r') || (c=='\r' && c2!='\n')) ungetc(c2,fp);
					c2 = -1;
					continue;
				}
				else c2 = c;
			}
		}
		if(!t && !strchr(trim,c)) t = 1;
		if(t) {
			if(end && strchr(end,c)) break;
			if(len==cap) resize(0);
			if(c2!=-1) str[len++] = (char)c2,c2 = -1,l2 = len;
			else str[len++] = (char)c;
		}
	}
	if(trim && *trim!='\0') while(len>l2 && strchr(trim,str[len-1])) len--;
	str[len] = '\0';
//debug_output("[%s]\n",str);
	return *this;
}

aString &aString::include(const char *fn) {
	FILE *fp = fopen(fn,"r");
	append(fp,true);
	fclose(fp);
	return *this;
}

aString &aString::includef(const char *format, ...) {
	char buf[128];
	va_list args;
   va_start(args,format);
	vsnprintf(buf,127,format,args);
   va_end(args);
	return include(buf);
}

aString &aString::print(FILE *fp) {
	if(fp) {
		size_t n;
		n = fwrite(str,len,1,fp);
	}
	return *this;
}

aString &aString::println(FILE *fp) {
	if(fp) {
		size_t n;
		n = fwrite(str,len,1,fp);
		n = fwrite(endline,strlen(endline),1,fp);
	}
	return *this;
}

void aString::printUTF8(char *d,const char *s,size_t offset,size_t len) {
	char c;
	size_t i,n;
	if(offset) for(i=0; *s!='\0' && i<offset; i++) {
		if(*s&0x80) {
			for(c=(*s<<1),n=1; c&0x80 && n<8; c<<=1,n++);
			while(n--) s++;
		} else s++;
	}
	if(len) for(i=0; *s!='\0' && i<len; i++) {
		if(*s&0x80) {
			for(c=(*s<<1),n=1; c&0x80 && n<8; c<<=1,n++);
			while(n--) *d++ = *s++;
		} else *d++ = *s++;
	}
	*d = '\0';
}

long aString::indexOf(const char *s,size_t l) {
	if(str && *str && s && *s) {
		if(!l) l = strlen(s);
		for(size_t i=0,n=len-l; i<n; i++)
			if(str[i]==*s && str[i+1]==s[1] && strncmp(&str[i],s,l)==0) return i;
	}
	return -1;
}

bool aString::startsWith(const char *s,size_t l) { return str && s? (strncmp(str,s,l? l : strlen(s))==0) : false; }
bool aString::equals(const char *s) { return str && s? strcmp(str,s)==0 : false; }
int aString::compare(const char *s) { return str && s? strcmp(str,s) : (str? -256 : 256); }

size_t aString::count(const char *s) {
	if(!s || !*s || !str || !len) return 0;
	char *p = str;
	size_t n = 0,sl = strlen(s);
	for(; *p && (p=strstr(p,s)); p+=sl,n++);
	return n;
}

size_t aString::count(char c) {
	if(c=='\0' || !str || !len) return 0;
	char *p = str;
	size_t n = 0;
	for(; *p; p++) if(*p==c) n++;
	return n;
}

aString &aString::replace(const char *s,const char *r) {
	if(!s || !*s || !r) return *this;
	size_t n;
	n = count(s);
	if(n>0) {
		char *p = str,*p1 = p,*p2;
		size_t sl = strlen(s);
		cap = cap+(strlen(r)-sl)*n;
		str = (char *)malloc(cap+1);
		*str = '\0';
		len = 0;
		for(; *p1 && (p2=strstr(p1,s)); p1=p2+sl) {
			n = (size_t)(p2-p1);
			if(n) append(p1,n);
			append(r);
		}
		if(*p1) append(p1);
		::free(p);
	}
	return *this;
}

aString &aString::escape() {
	if(str && len) {
		size_t i,n = 0;
		for(i=0; i<len; i++) if(isEscSpace(str[i])) n++;
		if(n) {
			if(len+n>=cap) resize(n);
			char c;
			for(i=len-1,c=str[i],n=0; i>=0; c=str[--i]) {
				if(c=='\t') str[i-- +n] = '\\',c = '\\',n++,len++;
				else if(c=='\n') str[i-- +n] = '\\',c = '\\',n++,len++;
				str[i+n] = c;
			}
			str[len] = '\0';
		}
	}
	return *this;
}

aString &aString::unescape() {
	if(str && len) {
		size_t i,n;
		char c;
		for(i=0,n=0,c=str[i]; i<len; i++,c=str[i+n]) {
			if(c=='\\') {
				n++,c = str[i+n],len--;
				if(c=='t') c = '\t';
				else if(c=='n') c = '\n';
			}
			str[i] = c;
		}
		str[len] = '\0';
	}
	return *this;
}

aString &aString::quote(const char c) {
	if(c && str && len) {
		if(len+2>=cap) resize(0);
		*str = c;
		for(size_t i=0; i<len; i++) str[i+1] = str[i];
		str[++len] = c,str[++len] = '\0';
	}
	return *this;
}

aString &aString::unquote() {
	if(str && len>1) {
		char c = *str;
		if((c=='"' || c=='\'') && str[len-1]==c) {
			for(size_t i=1; i<len; i++) str[i-1] = str[i];
			str[len-=2] = '\0';
		}
	}
	return *this;
}

aString &aString::encodeURL() {
	if(str && len) {
		size_t i,n = 0;
		for(i=0; i<len; i++) if(isURLEncoded(str[i]) && str[i]!=' ') n += 2;
		if(n) {
			if(len+n>=cap) resize(n);
			unsigned char c;
			char *p1=&str[len-1],*p2=&str[len+n];
			*p2-- = '\0',len += n;
			while(p1>=str) {
				if(*p1==' ') *p2-- = '+',p1--;
				else if(isURLEncoded(*p1)) {
					c = *p1, c &= 0xf,*p2-- = (c>9? 'A'+c-10 : '0'+c);
					c = *p1,c >>= 4,*p2-- = (c>9? 'A'+c-10 : '0'+c);
					*p2-- = '%',p1--;
				} else *p2-- = *p1--;
			}
		}
	}
	return *this;
}

aString &aString::decodeURL() {
	if(str && len) {
		size_t n = 0;
		int c1,c2;
		char *p1 = str,*p2 = p1,*p3 = &str[len];
		while(p1<=p3) {
			if(*p1=='+') *p2++ = ' ',p1++;
			else if(*p1=='%') {
				c1 = p1[1],c1 = c1>='a' && c1<='f'? (c1+10-'a') : (c1>='A' && c1<='F'? (c1+10-'A') : (c1>='0' && c1<='9'? c1-'0' : -1));
				c2 = p1[2],c2 = c2>='a' && c2<='f'? (c2+10-'a') : (c2>='A' && c2<='F'? (c2+10-'A') : (c2>='0' && c2<='9'? c2-'0' : -1));
				if(c1==-1) *p2++ = *p1++;
				else if(c2==-1) p1 += 2,*p2++ = c1,n++;
				else p1 += 3,*p2++ = (c1<<4)|c2,n += 2;
			} else *p2++ = *p1++;
		}
		len -= n;
	}
	return *this;
}

aString &aString::encodeHTML() {
	if(str && len) {
		size_t i,n = 0;
		const entity *e;
		for(i=0; i<len; i++) {
			e = &HTMLentities[(unsigned char)str[i]];
			if(e->len) n += e->len+1;
		}
		if(n) {
			if(len+n>=cap) resize(n);
			char *p1=&str[len-1],*p2=&str[len+n];
			*p2-- = '\0',len += n;
			while(p1>=str) {
				e = &HTMLentities[(unsigned char)*p1];
				if(e->len) {
					*p2 = ';',p2 -= e->len;
					memcpy(p2,e->name,e->len);
					--p2,*p2-- = '&',p1--;
				} else *p2-- = *p1--;
			}
		}
	}
	return *this;
}

aString &aString::decodeHTML() {
	if(str && len) {
		size_t i,n = 0;
		const entity *e;
		char *p1 = str,*p2 = p1,*p3 = &str[len];
		while(p1<=p3) {
			if(*p1=='&') {
				p1++;
				if(*p1=='#') {
					p1++,n++;
					if(*p1=='x' || *p1=='X') i = fromHex(++p1),n++;
					else i = atoi(p1);
					if(i>255) *p2++ = '?';
					else *p2++ = i;
					while(*p1!=';' && isHex(*p1)) p1++,n++;
					p1++,n++;
				} else {
					for(i=0; i<255; i++) {
						e = &HTMLentities[i];
						if(e->len && *p1==*e->name && !strncmp(p1,e->name,e->len)) {
							*p2++ = i,p1 += e->len,n += e->len;
							break;
						}
					}
					if(*p1==';') p1++,n++;
				}
			} else *p2++ = *p1++;
		}
		len -= n;
	}
	return *this;
}

aString &aString::free() {
	::free(str);
	str = 0,len = 0,cap = 0;
	return *this;
}

void aString::resize(size_t n) {
	if(n<0 || len+n+n<cap) return;
	if(!cap) cap = 1;
	if(n && len+n>=cap) cap = len+n+1;
	if(cap<1024) cap <<= 1;
	else cap += 1024;
	if(!str) str = (char *)malloc(cap+1);
	else str = (char *)realloc(str,cap+1);
	str[cap] = '\0';
//debug_output("aString::resize(len=%d,n=%d,cap=%d)\n",len,n,cap);
}

void aString::setCapacity(size_t n) {
	if(!n) return;
	cap = n;
	if(!str) str = (char *)malloc(cap+1);
	else str = (char *)realloc(str,cap+1);
	str[cap] = '\0';
//debug_output("aString::resize(len=%d,n=%d,cap=%d)\n",len,n,cap);
}

int aString::toInt() {
	char *p = str;
	while(isSpace(*p)) p++;
	if(*p=='0' && *++p=='x') return fromHex(++p);
	return atoi(p);
}

size_t aString::toIntArray(int *n,char c) {
	if(!n || c=='\0' || !str || !len) return 0;
	size_t i;
	char *p1 = str,*p2;
	while(*p1 && !isDigit(*p1)) p1++;
	if(!*p1) return 0;
	for(i=0; p1 && *p1; i++,p1=p2) {
		p2 = strchr(p1,c);
		if(p2) *p2++ = '\0';
		if(*p1=='0' && *++p1=='x') n[i] = fromHex(++p1);
		else n[i] = atoi(p1);
	}
	return i;
}


size_t aString::nextWord(const char **s,const char *c) {
	size_t l = 0;
	const char *p = *s;
	while(*p && !strchr(c,*p)) p++,l++;
	*s = *p==' '? p+1 : 0;
	return l;
}


uint64_t aString::fromHex(const char *str) {
	if(!str) return 0;
	uint64_t n = 0;
	for(int i=0; isHex(*str) && i<16; i++) n = (n<<4)|fromHex(*str),str++;
	return n;
}

char *aString::toHex(char *h,uint64_t i,bool upper) {
	int u = 0;
	const char *s = upper? upper_hex : lower_hex;
	for(uint64_t n=i; n; n>>=4) u++;
	for(h+=u,*h='\0'; i; i>>=4) *--h = s[i&0xf];//(u=(i&0xf))<10? '0'+u : 'a'+(u-10);
	return h;
}

char *aString::toLower(char *str) { char *s = str;while(*s) *s = toLower(*s),s++;return str; }
char *aString::toUpper(char *str) { char *s = str;while(*s) *s = toUpper(*s),s++;return str; }

int aString::stricmp(const char *str1,const char *str2) {
	char c1 = toLower(*str1++),c2 = toLower(*str2++);
	for(; c1 && c2; c1=toLower(*str1++),c2=toLower(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (!c1 && !c2)? 0 : (!c1? -1 : 1);
}

int aString::strnicmp(const char *str1,const char *str2,size_t n) {
	char c1 = toLower(*str1++),c2 = toLower(*str2++);
	for(; --n && c1 && c2; c1=toLower(*str1++),c2=toLower(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (!n || (!c1 && !c2))? 0 : (!c1? -1 : 1);
}

char *aString::stristr(char *str1,const char *str2) {
	int l = strlen(str2),c1,c2 = toLower(*str2);
	for(; (c1=toLower(*str1)); str1++) if(c1==c2 && strnicmp(str1,str2,l)) return str1;
	return 0;
}

int aString::countTokens(char *str,const char *delim,bool cins) {
	int l = strlen(delim),n = 0;
	if(cins) for(; str && *str; n++) { if((str=stristr(str,delim))) str += l; }
	else for(; str && *str; n++) { if((str=strstr(str,delim))) str += l; }
	return n;
}

char **aString::split(char **list,char *str,const char *delim,bool cins) {
	size_t l = strlen(delim),n = 0;
	if(cins) for(; str && *str; n++) { list[n] = str;if((str=stristr(str,delim))) *str = '\0',str += l; }
	else for(; str && *str; n++) { list[n] = str;if((str=strstr(str,delim))) *str = '\0',str += l; }
	return list;
}

size_t aString::trim(char *str) {
	if(!str || !*str) return 0;
	size_t l = 0;
	char *p = str;
	while(*p && isSpace(*p)) p++;
	while(*p) *str++ = *p++,l++;
	if(l) {
		while(isSpace(*--str)) l--;
		*++str = '\0';
	} else *str = '\0';
	return l;
}


aString operator+(aString &s,const char c) { aString s1(s.length()+1);s1.append(s).append(c);return s1; }
aString operator+(const char c,aString &s) { aString s1(s.length()+1);s1.append(c).append(s);return s1; }
aString operator+(aString &s,aString *s1) { aString s2(s.length()+(s1? s1->length() : 0));s2.append(s).append(s1);return s2; }
aString operator+(aString *s,aString &s1) { aString s2((s? s->length() : 0)+s1.length());s2.append(s).append(s1);return s2; }
aString operator+(aString &s,aString &s1) { aString s2(s.length()+s1.length());s2.append(s).append(s1);return s2; }
aString operator+(aString &s,const char *s1) { aString s2(s);s2.append(s1);return s2; }
aString operator+(const char *s,aString &s1) { aString s2(s);s2.append(s1);return s2; }
aString operator+(aString &s,int16_t i) { aString s1(s.length()+6);s1.append(s).append((int32_t)i);return s1; }
aString operator+(int16_t i,aString &s) { aString s1(6+s.length());s1.append((int32_t)i).append(s);return s1; }
aString operator+(aString &s,uint16_t i) { aString s1(s.length()+5);s1.append(s).append((uint16_t)i);return s1; }
aString operator+(uint16_t i,aString &s) { aString s1(5+s.length());s1.append((uint16_t)i).append(s);return s1; }
aString operator+(aString &s,int32_t i) { aString s1(s.length()+11);s1.append(s).append(i);return s1; }
aString operator+(int32_t i,aString &s) { aString s1(11+s.length());s1.append(i).append(s);return s1; }
aString operator+(aString &s,uint32_t i) { aString s1(s.length()+10);s1.append(s).append(i);return s1; }
aString operator+(uint32_t i,aString &s) { aString s1(10+s.length());s1.append(i).append(s);return s1; }
aString operator+(aString &s,int64_t i) { aString s1(s.length()+21);s1.append(s).append(i);return s1; }
aString operator+(int64_t i,aString &s) { aString s1(21+s.length());s1.append(i).append(s);return s1; }
aString operator+(aString &s,uint64_t i) { aString s1(s.length()+20);s1.append(s).append(i);return s1; }
aString operator+(uint64_t i,aString &s) { aString s1(20+s.length());s1.append(i).append(s);return s1; }
aString operator+(aString &s,float f) { aString s1(s.length()+16ul);s1.append(s).append(f);return s1; }
aString operator+(float f,aString &s) { aString s1(16ul+s.length());s1.append(f).append(s);return s1; }
aString operator+(aString &s,double d) { aString s1(s.length()+32ul);s1.append(s).append(d);return s1; }
aString operator+(double d,aString &s) { aString s1(32ul+s.length());s1.append(d).append(s);return s1; }






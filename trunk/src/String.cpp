
#include "config.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <libamanita/String.h>


RttiObjectInheritance(String,Object);


//#define WIN32 1

const char *String::blank = "";

//#ifdef WIN32
const char *String::endline = "\r\n";
//#else
//char *String::endline = "\n";
//#endif
const char *String::whitespace = " \t\n\r";


String::String(size_t c) : Object(),str(0),len(0ul),cap(0ul) {
	if(c>0) setCapacity(c);
}

String::String(const char *s,size_t l) : Object() {
	if(s && *s) {
		if(!l) len = strlen(s);
		cap = len;
		str = (char *)malloc(cap+1ul);
		if(cap) memcpy(str,s,len+1ul);
		else *str = '\0';
	} else str = 0,len = 0ul,cap = 0ul;
}

String::String(String *s) : Object() {
	if(!s || !s->str) str = 0,len = 0ul,cap = 0ul;
	else {
		len = s->len,cap = s->cap,str = (char *)malloc(cap+1);
		if(*s->str) memcpy(str,s->str,len+1ul);
		else *str = '\0';
	}
}

String::String(String &s) : Object() {
	if(!s.str) str = 0,len = 0ul,cap = 0ul;
	else {
		len = s.len,cap = s.cap,str = (char *)malloc(cap+1);
		if(*s.str) memcpy(str,s.str,len+1ul);
		else *str = '\0';
	}
}

String::~String() {
	::free(str);
	str = 0,len = 0ul,cap = 0ul;
}

String &String::append(char c) {
	if(len==cap) resize(0ul);
	if(c!='\0') str[len++] = c;
	str[len] = '\0';
	return *this;
}
String &String::append(char c,size_t n) {
	resize(n);
	while(n--) str[len++] = c;
	str[len] = '\0';
	return *this;
}

String &String::append(const char *s,size_t l) {
	if(s && *s) {
		if(!l) l = strlen(s);
		resize(l);
		memcpy(&str[len],s,l);
		len += l;
		str[len] = '\0';
	}
	return *this;
}
String &String::append(const char *s,size_t l,size_t n) {
	if(s && *s && n>0ul) {
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

String &String::append(long l) {
	if(l==0l) return append('0');
	char s[13],*p = s+12,c = 0;
	*p-- = '\0';
	if(l<0L) c = '-',l = -l;
	for(; l; l/=10) *p-- = '0'+(l%10);
	if(c) *p-- = c;
	return append(p+1);
}
String &String::append(long l,int base) {
	if(l==0l) return append('0');
	char s[sizeof(l)*8+1],*p = s+sizeof(s)-1,c = 0,n;
	*p-- = '\0';
	if(l<0L) c = '-',l = -l;
	for(; l; l/=base) n = l%base,*p-- = n<=9? '0'+n : 'A'+n-10;
	if(c) *p-- = c;
	return append(p+1);
}
String &String::append(unsigned long ul) {
	if(ul==0) return append('0');
	char s[12],*p = s+11;
	*p-- = '\0';
	for(; ul; ul/=10L) *p-- = '0'+(ul%10L);
	return append(p+1);
}
String &String::append(double f,int n,char c) {
	long long n1 = (long long)f;
	unsigned long n2,m = 1;
	if(n>10) n = 10;
	while(0<n--) m *= 10;
	n2 = (unsigned long)round(fabs(f-n1)*m);
	return append(n1).append(c).append(n2);
}
String &String::append(long long ll) {
	if(ll==0) return append('0');
	char s[22],*p = s+21,c = 0;
	*p-- = '\0';
	if(ll<0) c = '-',ll = -ll;
	for(; ll; ll/=10) *p-- = '0'+(ll%10);
	if(c) *p-- = c;
	return append(p+1);
}
String &String::append(unsigned long long ull) {
	if(ull==0) return append('0');
	char s[21],*p = s+20;
	*p-- = '\0';
	for(; ull; ull/=10) *p-- = '0'+(ull%10);
	return append(p+1);
}

String &String::appendUntil(const char *s,const char *end,bool uesc) {
	if(!s || !*s) return *this;
	char c;
	while((c=*s++)) {
		if(c=='\\' && uesc) {
			c = *s++;
			if(c=='n') c = '\n';
			else if(c=='t') c = '\t';
			else if(!c) break;
		}
		if(strchr(end,c)) break;
		if(len==cap) resize(0);
		str[len++] = c;
	}
	str[len] = '\0';
	return *this;
}


String &String::appendf(const char *f, ...) {
	va_list list;
	va_start(list,f);
	vappendf(f,list);
	va_end(list);
	return *this;
}

enum {
	F_PLUS			= 0x001,
	F_MINUS			= 0x002,
	F_HASH			= 0x008,
	F_PRECISION		= 0x080,
	F_h				= 0x200,
	F_l				= 0x400,
	F_L				= 0x800,
};

String &String::vappendf(const char *f,va_list list) {
	if(!f || !*f) return *this;
	int c;
	unsigned long flags,*w,w1,w2,w2default,l;
	char pad;
	while(1) {
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
			else if(c=='l') flags |= F_l;
			else if(c=='L') flags |= F_L;
			else break;
			c=*f++;
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
				long n = va_arg(list,long);
//				if(w1 && !(flags&F_MINUS) && l<w1) append(w1-l,pad);
				if((flags&F_PLUS) && n>=0) append('+');
				append(n);
//				if(w1 && (flags&F_MINUS) && l<w1) repeat(w1-l,pad);
				break;
			}
			case 'u':
			case 'U':
			{
				unsigned long n = va_arg(list,unsigned long);
//				if(w1 && !(flags&F_MINUS) && l<w1) append(w1-l,pad);
				if((flags&F_PLUS) && n>=0) append('+');
				append(n);
//				if(w1 && (flags&F_MINUS) && l<w1) repeat(w1-l,pad);
				break;
			}
			case 'f':
			case 'F':
			{
				double n = va_arg(list,double);
fprintf(stderr,"n=%f,flags=%lx\n",n,flags);
//				if(w1 && !(flags&F_MINUS) && l<w1) append(w1-l,pad);
				if((flags&F_PLUS) && n>=0.) append('+');
				append(n,w2default? ((flags&F_L)? 6 : w2default) : w2);
//				if(w1 && (flags&F_MINUS) && l<w1) repeat(w1-l,pad);
				break;
			}
			case 's':
			case 'S':
			{
				char *s = va_arg(list,char *);
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

String &String::appendUntil(FILE *fp,const char *end,const char *trim,bool uesc) {
	if(fp==NULL) return *this;
	if(trim==NULL) trim = end;
	if(end==NULL) {
		fpos_t pos;
		if(!fgetpos(fp,&pos)) {
			long fpos = ftell(fp);
			fseek(fp,0,SEEK_END);
			long fend = ftell(fp);
			if(fpos>=0l && fend>=0l && fend-fpos>(long)cap) resize(fend-fpos);
			fsetpos(fp,&pos);
		}
	}
	int c,c2 = -1,t = trim && *trim? 0 : 1;
	unsigned long l2 = 0ul;
	while((c=fgetc(fp)) && c!=EOF) {
// fputc(c,stderr);
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
		if(!t && strchr(trim,c)==NULL) t = 1;
		if(t) {
			if(end && strchr(end,c)!=NULL) break;
			if(len==cap) resize(0);
			if(c2!=-1) str[len++] = (char)c2,c2 = -1,l2 = len;
			else str[len++] = (char)c;
		}
	}
	if(trim && *trim!='\0') while(len>l2 && strchr(trim,str[len-1])!=NULL) len--;
	str[len] = '\0';
// fprintf(stderr,"[%s]\n",str);
	return *this;
}

String &String::print(FILE *fp) {
	if(fp) {
		size_t n;
		n = fwrite(str,len,1,fp);
	}
	return *this;
}
String &String::println(FILE *fp) {
	if(fp) {
		size_t n;
		n = fwrite(str,len,1,fp);
		n = fwrite(endline,strlen(endline),1,fp);
	}
	return *this;
}

long String::indexOf(const char *s,size_t l) {
	if(str && *str && s && *s) {
		if(!l) l = strlen(s);
		for(size_t i=0ul,n=len-l; i<n; i++)
			if(str[i]==*s && str[i+1]==s[1] && strncmp(&str[i],s,l)==0) return i;
	}
	return -1l;
}
bool String::startsWith(const char *s,size_t l) { return str && s? (strncmp(str,s,l? l : strlen(s))==0) : false; }
bool String::equals(const char *s) { return str && s? strcmp(str,s)==0 : false; }
int String::compare(const char *s) { return str && s? strcmp(str,s) : (str? -256 : 256); }

String &String::escape() {
	if(str && len) {
		size_t i,n = 0ul;
		for(i=0ul; i<len; i++) if(isEscSpace(str[i])) n++;
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
String &String::unescape() {
	if(str && len) {
		size_t i,n;
		char c;
		for(i=0ul,n=0ul,c=str[i]; i<len; i++,c=str[i+n]) {
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

String &String::quote(const char c) {
	if(c && str && len) {
		if(len+2ul>=cap) resize(0ul);
		*str = c;
		for(size_t i=0ul; i<len; i++) str[i+1ul] = str[i];
		str[++len] = c,str[++len] = '\0';
	}
	return *this;
}
String &String::unquote() {
	if(str && len>1ul) {
		char c = *str;
		if((c=='"' || c=='\'') && str[len-1]==c) {
			for(size_t i=1ul; i<len; i++) str[i-1ul] = str[i];
			str[len-=2ul] = '\0';
		}
	}
	return *this;
}

String &String::encodeURL() {
	if(str && len) {
		size_t i,n = 0ul;
		for(i=0ul; i<len; i++) if(isURLEncoded(str[i]) && str[i]!=' ') n += 2ul;
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
String &String::decodeURL() {
	if(str && len) {
		size_t n = 0ul;
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

String &String::encodeHTML() {
	if(str && len) {
		size_t i,n = 0ul;
		const entity *e;
		for(i=0ul; i<len; i++) {
			e = &HTMLentities[(unsigned char)str[i]];
			if(e->len) n += e->len+1ul;
		}
		if(n) {
			if(len+n>=cap) resize(n);
			char *p1=&str[len-1ul],*p2=&str[len+n];
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
String &String::decodeHTML() {
	if(str && len) {
		size_t i,n = 0ul;
		const entity *e;
		char *p1 = str,*p2 = p1,*p3 = &str[len];
		while(p1<=p3) {
			if(*p1=='&') {
				p1++;
				if(*p1=='#') {
					p1++,n++;
					if(*p1=='x' || *p1=='X') i = fromHex(++p1),n++;
					else i = atoi(p1);
					if(i>255ul) *p2++ = '?';
					else *p2++ = i;
					while(*p1!=';' && isHex(*p1)) p1++,n++;
					p1++,n++;
				} else {
					for(i=0ul; i<255ul; i++) {
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

String &String::free() {
	::free(str);
	str = 0,len = 0ul,cap = 0ul;
	return *this;
}

void String::resize(size_t n) {
	if(n<0ul || len+n+n<cap) return;
	if(!cap) cap = 1ul;
	if(n && len+n>=cap) cap = len+n+1ul;
	if(cap<1024ul) cap <<= 1ul;
	else cap += 1024ul;
	if(str==NULL) str = (char *)malloc(cap+1ul);
	else str = (char *)realloc(str,cap+1ul);
	str[cap] = '\0';
//fprintf(stderr,"String::resize(len=%d,n=%d,cap=%d)\n",len,n,cap);
//fflush(stderr);
}

void String::setCapacity(size_t n) {
	if(!n) return;
	cap = n;
	if(!str) str = (char *)malloc(cap+1ul);
	else str = (char *)realloc(str,cap+1ul);
	str[cap] = '\0';
//fprintf(stderr,"String::resize(len=%d,n=%d,cap=%d)\n",len,n,cap);
//fflush(stderr);
}

int String::toInt() {
	char *p = str;
	while(isSpace(*p)) p++;
	if(*p=='0' && *++p=='x') return fromHex(++p);
	return atoi(p);
}


size_t String::nextWord(const char **s,const char *c) {
	size_t l = 0ul;
	const char *p = *s;
	while(*p && !strchr(c,*p)) p++,l++;
	*s = *p==' '? p+1 : 0;
	return l;
}


unsigned long String::fromHex(char *str) {
	if(str==NULL) return 0ul;
	unsigned long n = 0ul;
	while(isHex(*str)) n = (n<<4)|fromHex(*str),str++;
	return n;
}

char *String::toHex(char *h,unsigned long i) {
	char u = 0;
	for(int n=i; n; n>>=4) u++;
	for(h+=u,*h='\0'; i; i>>=4) *--h = "0123456789abcdef"[i&0xf];//(u=(i&0xf))<10? '0'+u : 'a'+(u-10);
	return h;
}

char *String::toLower(char *str) { char *s = str;while(*s) *s = toLower(*s),s++;return str; }
char *String::toUpper(char *str) { char *s = str;while(*s) *s = toUpper(*s),s++;return str; }

int String::stricmp(const char *str1,const char *str2) {
	char c1 = toLower(*str1++),c2 = toLower(*str2++);
	for(; c1 && c2; c1=toLower(*str1++),c2=toLower(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (!c1 && !c2)? 0 : (!c1? -1 : 1);
}

int String::strnicmp(const char *str1,const char *str2,size_t n) {
	char c1 = toLower(*str1++),c2 = toLower(*str2++);
	for(; --n && c1 && c2; c1=toLower(*str1++),c2=toLower(*str2++)) if(c1!=c2) return c1<c2? -1 : 1;
	return (!n || (!c1 && !c2))? 0 : (!c1? -1 : 1);
}

char *String::stristr(char *str1,const char *str2) {
	int l = strlen(str2),c1,c2 = toLower(*str2);
	for(; (c1=toLower(*str1)); str1++) if(c1==c2 && strnicmp(str1,str2,l)) return str1;
	return 0;
}

int String::countTokens(char *str,const char *delim,bool cins) {
	int l = strlen(delim),n = 0;
	if(cins) for(; str && *str; n++) { if((str=stristr(str,delim))) str += l; }
	else for(; str && *str; n++) { if((str=strstr(str,delim))) str += l; }
	return n;
}

char **String::split(char **list,char *str,const char *delim,bool cins) {
	size_t l = strlen(delim),n = 0ul;
	if(cins) for(; str && *str; n++) { list[n] = str;if((str=stristr(str,delim))) *str = '\0',str += l; }
	else for(; str && *str; n++) { list[n] = str;if((str=strstr(str,delim))) *str = '\0',str += l; }
	return list;
}

size_t String::trim(char *str) {
	if(!str || !*str) return 0ul;
	size_t l = 0ul;
	char *p = str;
	while(*p && isSpace(*p)) p++;
	while(*p) *str++ = *p++,l++;
	if(l) {
		while(isSpace(*--str)) l--;
		*++str = '\0';
	} else *str = '\0';
	return l;
}


String operator+(String &s,const char c) { String s1(s.length()+1ul);s1.append(s).append(c);return s1; }
String operator+(const char c,String &s) { String s1(s.length()+1ul);s1.append(c).append(s);return s1; }
String operator+(String &s,String *s1) { String s2(s.length()+(s1? s1->length() : 0ul));s2.append(s).append(s1);return s2; }
String operator+(String *s,String &s1) { String s2((s? s->length() : 0ul)+s1.length());s2.append(s).append(s1);return s2; }
String operator+(String &s,String &s1) { String s2(s.length()+s1.length());s2.append(s).append(s1);return s2; }
String operator+(String &s,const char *s1) { String s2(s);s2.append(s1);return s2; }
String operator+(const char *s,String &s1) { String s2(s);s2.append(s1);return s2; }
String operator+(String &s,short s1) { String s2(s.length()+6ul);s2.append(s).append((long)s1);return s2; }
String operator+(short s,String &s1) { String s2(6ul+s1.length());s2.append((long)s).append(s1);return s2; }
String operator+(String &s,unsigned short s1) { String s2(s.length()+5ul);s2.append(s).append((unsigned long)s1);return s2; }
String operator+(unsigned short s,String &s1) { String s2(5ul+s1.length());s2.append((unsigned long)s).append(s1);return s2; }
String operator+(String &s,int i) { String s1(s.length()+11ul);s1.append(s).append((long)i);return s1; }
String operator+(int i,String &s) { String s1(11ul+s.length());s1.append((long)i).append(s);return s1; }
String operator+(String &s,unsigned int i) { String s1(s.length()+10ul);s1.append(s).append((unsigned long)i);return s1; }
String operator+(unsigned int i,String &s) { String s1(10ul+s.length());s1.append((unsigned long)i).append(s);return s1; }
String operator+(String &s,long l) { String s1(s.length()+11ul);s1.append(s).append(l);return s1; }
String operator+(long l,String &s) { String s1(11ul+s.length());s1.append(l).append(s);return s1; }
String operator+(String &s,unsigned long ul) { String s1(s.length()+10ul);s1.append(s).append(ul);return s1; }
String operator+(unsigned long ul,String &s) { String s1(10ul+s.length());s1.append(ul).append(s);return s1; }
String operator+(String &s,float f) { String s1(s.length()+16ul);s1.append(s).append(f);return s1; }
String operator+(float f,String &s) { String s1(16ul+s.length());s1.append(f).append(s);return s1; }
String operator+(String &s,double d) { String s1(s.length()+32ul);s1.append(s).append(d);return s1; }
String operator+(double d,String &s) { String s1(32ul+s.length());s1.append(d).append(s);return s1; }
String operator+(String &s,long long ll) { String s1(s.length()+21ul);s1.append(s).append(ll);return s1; }
String operator+(long long ll,String &s) { String s1(21ul+s.length());s1.append(ll).append(s);return s1; }
String operator+(String &s,unsigned long long ull) { String s1(s.length()+20ul);s1.append(s).append(ull);return s1; }
String operator+(unsigned long long ull,String &s) { String s1(20ul+s.length());s1.append(ull).append(s);return s1; }







#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <libamanita/Word.h>
#include <libamanita/Random.h>


const char *Word::vowels = "aaaaaeeeeeiiiiooouuuyy";
const char *Word::vowels2 = "aaaaaeeeeeiiiiooouuu";
const char *Word::consonants = "bbbccdddffggghjjkkkllmmmnnnpppqrrrsssstttvvvwxz";
const char *Word::consonants2 = "bbbdddffgggkkkllmmmnnnppprrrsssstttvv";
const char *Word::singles = "chqwxz";
const char *Word::doubles = "bdfglmnprstaei";
const char *Word::unaccepted = "bk,bm,bp,bv,dk,fv,gf,gk,kg,pb,pk,pm,pv,sb,tb,td";


Word::Word(const char *v,const char *v2,const char *c,const char *c2,const char *s,const char *d,const char *u) {
	vl = strlen(this->v=strdup(v? v : Word::vowels));
	v2l = strlen(this->v2=strdup(v2? v2 : Word::vowels2));
	cl = strlen(this->c=strdup(c? c : Word::consonants));
	c2l = strlen(this->c2=strdup(c2? c2 : Word::consonants2));
	sl = strlen(this->s=strdup(s? s : Word::singles));
	dl = strlen(this->d=strdup(d? d : Word::doubles));
	ul = strlen(this->u=strdup(u? u : Word::unaccepted));
	rnd = &::rnd,str = 0,len = 0;
}

Word::~Word() {
	free(v);
	free(v2);
	free(c);
	free(c2);
	free(s);
	free(d);
	free(u);
	if(rnd && rnd!=&::rnd) delete rnd;
	free(str);
	v = 0,v2 = 0,c = 0,c2 = 0,s = 0,d = 0,u = 0,rnd = 0,str = 0,len = 0;
}

void Word::setSeed(unsigned int n) {
	if(rnd==&::rnd) rnd = new Random(n);
	else rnd->setSeed(n);
}

char *Word::generate(int minlen,int maxlen) {
	size_t l = minlen+(minlen>=maxlen? 0 : rnd->uint32(maxlen-minlen));
	size_t i = 0,n = 0,r = rnd->uint32(2),a = 0;
	if(l>len) {
		if(str) str = (char *)realloc(str,l+1);
		else str = (char *)malloc(l+1);
		len = l;
	}
	char ch;
	while(i<l) {
		if(r) {
			str[i++] = ch = c[rnd->uint32(cl)];
			if(i<l && rnd->uint32(2)) {
				if(a && strchr(d,ch)) {
					if((ch=='c' || ch=='s') && rnd->uint32(2)) ch = 'h';
					str[i++] = ch;
				} else if(!strchr(s,ch)) {
					while(ch==(str[i]=c2[n=rnd->uint32(c2l)]) && strstr(u,str+i-1));
					ch = str[i++];
				}
				if(ch=='k' && i>1 && str[i-2]=='k') str[i-2] = 'c';
			}
			r = 0,a = 0;
		} else {
			str[i++] = ch = v[n=rnd->uint32(vl)];
			if(i<l && strchr(d,ch) && rnd->uint32(2)) {
				while(ch==v2[n=rnd->uint32(v2l)]);
				str[i++] = ch = v2[n];
			}
			r = 1,a = rnd->uint32(2);
		}
	}
	str[0] -= 32;
	str[i] = '\0';
	return str;
}

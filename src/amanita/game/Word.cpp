
#include "../_config.h"
#include <stdlib.h>
#include <string.h>
#include <amanita/Random.h>
#include <amanita/String.h>
#include <amanita/game/Word.h>


static const char *vowels = "aaaaaeeeeeiiiiooouuuyy";
static const char *vowels2 = "aaaaaeeeeeiiiiooouuu";
static const char *consonants = "bbbccdddffggghjjkkkllmmmnnnpppqrrrsssstttvvvwxz";
static const char *consonants2 = "bbbdddffgggkkkllmmmnnnppprrrsssstttvv";
static const char *singles = "chqwxz";
static const char *doubles = "bdfglmnprstaei";
static const char *unaccepted = "bk,bm,bp,bv,dk,fv,gf,gk,kg,pb,pk,pm,pv,sb,tb,td";

static const char *extract_letters(const char *def,const char *nm,const char *ret,int &l) {
	if(def && *def) {
		int i,n;
		const char *p1 = strstr(def,nm),*p2;
		char c,*s;
		if(p1) {
			for(n=0,p1+=strlen(nm),p2=p1; *p2 && *p2!='\n'; ++p2)
				if(a::isalpha(*p2)) {
					for(i=0; a::isdigit(p2[1]); ++p2) i = i*10+(p2[1]-'0');
					n += i? i : 1;
				}
			if(n>0) {
				for(p2=p1,l=n,s=(char *)malloc(n+1),s[n]='\0',n=0; *p2 && *p2!='\n'; ++p2)
					if(a::isalpha(*p2)) {
						for(c=*p2,i=0; a::isdigit(p2[1]); ++p2) i = i*10+(p2[1]-'0');
						for(i=i? i : 1; i>0; --i,++n) s[n] = c;
					}
//debug_output("extract_letters: [%d] %s\n",l,s);
				return s;
			}
		}
	}
	l = strlen(ret);
	return ret;
}

static const char *extract_string(const char *def,const char *nm,const char *ret,int &l) {
	if(def && *def) {
		const char *p1 = strstr(def,nm),*p2;
		char *s;
		if(p1) {
			p1 += strlen(nm);
			while(!a::isalpha(*p1)) ++p1;
			for(p2=p1; *p2 && *p2!='\n'; ++p2);
			l = (int)(p2-p1),s = (char *)malloc(l+1),s[l] = '\0';
			strncpy(s,p1,l);
//debug_output("extract_string: [%d] %s\n",l,s);
			return s;
		}
	}
	l = strlen(ret);
	return ret;
}


namespace a {

Word::Word(const char *def) {
	v = extract_letters(def,"vowels",vowels,vl);
	v2 = extract_letters(def,"vowels2",vowels2,v2l);
	c = extract_letters(def,"consonants",consonants,cl);
	c2 = extract_letters(def,"consonants2",consonants2,c2l);
	s = extract_string(def,"singles",singles,sl);
	d = extract_string(def,"doubles",doubles,dl);
	u = extract_string(def,"unaccepted",unaccepted,ul);
	rnd = &a::rnd,str = 0,len = 0;
}

Word::~Word() {
	if(v!=vowels) free((char *)v);
	if(v2!=vowels2) free((char *)v2);
	if(c!=consonants) free((char *)c);
	if(c2!=consonants2) free((char *)c2);
	if(s!=singles) free((char *)s);
	if(d!=doubles) free((char *)d);
	if(u!=unaccepted) free((char *)u);
	if(rnd && rnd!=&a::rnd) delete rnd;
	free(str);
	v = 0,v2 = 0,c = 0,c2 = 0,s = 0,d = 0,u = 0,rnd = 0,str = 0,len = 0;
}

void Word::setSeed(unsigned int n) {
	if(rnd==&a::rnd) rnd = new Random(n);
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

}; /* namespace a */



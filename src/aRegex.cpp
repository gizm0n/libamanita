
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <libamanita/aRegex.h>


#define REGEX_RNDBR			'('
#define REGEX_SQRBR			'['

enum REGEX_FLAG {
	REGEX_CHAR					= 0x00000010,		// [\x01-\xff]
	REGEX_WORD					= 0x00000100,		// \w [a-zA-Z0-9_]
	REGEX_DIGIT					= 0x00000200,		// \d [0-9]
	REGEX_SPACE					= 0x00000400,		// \s [ \t]
	REGEX_ANY					= 0x00000800,		// . [^\n]
	REGEX_SPCH_MASK			= 0x00000f00,		//

	REGEX_SKIP					= 0x00001000,		// ?
	REGEX_LOOP					= 0x00002000,		// +
	REGEX_MINMAX				= 0x00004000,		// {n,m}
	REGEX_LAZY					= 0x00008000,		// ??

	REGEX_WORD_B				= 0x00010000,		// \<
	REGEX_WORD_E				= 0x00020000,		// \>
	REGEX_WORD_BE				= 0x00030000,		// \b
	REGEX_LINE_BML				= 0x00040000,		// ^ (REGEX_MULTILINE)
	REGEX_LINE_EML				= 0x00080000,		// $ (REGEX_MULTILINE)
	REGEX_LINE_B				= 0x00100000,		// ^
	REGEX_LINE_E				= 0x00280000,		// $
	REGEX_STRING_B				= 0x00100000,		// \A
	REGEX_STRING_E				= 0x00200000,		// \Z
	REGEX_ANCHOR_MASK			= 0x003f0000,		//

	REGEX_FIRST					= 0x00400000,		// (abc)abcabc
	REGEX_LAST					= 0x00800000,		// abcabc(abc)

	REGEX_NOT					= 0x01000000,		// ^
	REGEX_OR						= 0x02000000,		// (abc)|abc

	REGEX_BACKREFS				= 0x10000000,		//
};



const char *blank = "";


struct recode {
	char c;
	uint32_t f;
	unsigned short min,max;
};

class REBlock {
	public:
		static const char whitespace[];
		static const char word[];

		aRegex *re;
		char brack;
		recode *code;
		uint32_t *chars;
		uint32_t depth,ref,pos,len,flags;
		unsigned short min,max;
		REBlock *prev,*next,*last,*first,*onmatch,*onfail;

		REBlock(aRegex *re,REBlock *prev,uint32_t pos,uint32_t f=0,char br=0);
		~REBlock();

		void setFlags(uint32_t m,unsigned short min,unsigned short max);
		void compile(const char *exp);
		bool test(const char *t,uint32_t p,uint32_t i) {
			char c = t[p];
			return test(p? t[p-1] : 0,c,c? t[p+1] : 0,i);
		}
		bool test(char c0,char c1,char c2,uint32_t i);
};


const char REBlock::whitespace[] = " \n\t\r\f\v";
const char REBlock::word[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";

REBlock::REBlock(aRegex *re,REBlock *prev,uint32_t pos,uint32_t f,char br)
		: re(re),brack(br),code(NULL),chars(NULL),depth(0),ref(0),pos(pos),len(0),flags(f),min(1),max(1),
			prev(prev),next(NULL),last(NULL),first(NULL),onmatch(NULL),onfail(NULL) {
	if(prev!=NULL) prev->next = this;
}

REBlock::~REBlock() {
	if(code!=NULL) { free(code);code = NULL; }
	if(chars!=NULL) { free(chars);chars = NULL; }
	if(next!=NULL) { delete next;next = NULL; }
}

void REBlock::setFlags(uint32_t m,unsigned short min,unsigned short max) {
	if(m&REGEX_LOOP) {
		if(min==0) m |= REGEX_SKIP;
		if(max==1) m ^= REGEX_LOOP;
	}
	this->flags |= (m&0xffffff00),this->min = min,this->max = max;
}

void REBlock::compile(const char *exp) {
	if(!len) return;
	char c,t;
	int b,n = 0;
	exp += pos;
	uint32_t i,j;
	recode q;
	if(brack!=REGEX_RNDBR && brack!=REGEX_SQRBR) brack = REGEX_RNDBR;
	if(brack==REGEX_RNDBR) code = (recode *)malloc((len+1)*sizeof(recode));
	else {
		chars = (uint32_t *)malloc(32);
		for(i=0; i<8; i++) chars[i] = '\0';
	}
//printf("REBlock::compile(%p,len=%d,\"",this,len);
	for(i=0,j=0; i<len; i++) {
		c = re->ic(exp[i]),q = (recode){ '\0',0,1,1 };
//putchar(c);
		switch(c) {
			case '.':if(brack==REGEX_RNDBR) q.f = REGEX_ANY;break;
			case '\\':
				switch(c=exp[++i]) {
					case '\\':q.c = '\\',q.f |= REGEX_CHAR;break;
					case 'r':q.c = '\r',q.f |= REGEX_CHAR;break;
					case 'n':q.c = '\n',q.f |= REGEX_CHAR;break;
					case 't':q.c = '\t',q.f |= REGEX_CHAR;break;
					case 'w':q.f = REGEX_WORD;break;
					case 'W':q.f = REGEX_WORD|REGEX_NOT;break;
					case 'd':q.f = REGEX_DIGIT;break;
					case 'D':q.f = REGEX_DIGIT|REGEX_NOT;break;
					case 's':q.f = REGEX_SPACE;break;
					case 'S':q.f = REGEX_SPACE|REGEX_NOT;break;
					case 'x':
					case 'X':
						if(aRegex::isHex(exp[i+1])) {
							q.c = aRegex::fromHex(exp[++i]),q.f |= REGEX_CHAR;
							if(aRegex::isHex(exp[i+1])) q.c = (q.c<<4)|aRegex::fromHex(exp[++i]);
						}
						break;
					default:
						if(brack==REGEX_SQRBR) q.c = c=='b'? '\b' : c,q.f |= REGEX_CHAR;
						else if(brack==REGEX_RNDBR) switch(c) {
							case 'A':
							case 'a':if(i==1) q.f = REGEX_STRING_B;break;
							case 'Z':if(i==len-1) q.f = REGEX_STRING_E|REGEX_NOT;break;
							case 'z':if(i==len-1) q.f = REGEX_STRING_E;break;
							case 'B':if(i==1 || i==len-1) q.f = REGEX_WORD_BE|REGEX_NOT;break;
							case 'b':if(i==1 || i==len-1) q.f = REGEX_WORD_BE;break;
							default:q.c = c,q.f |= REGEX_CHAR;
						}
				}
				break;
			case '^':
				if(i==0 && brack==REGEX_SQRBR) n = 1;
				else if(i==0 && brack==REGEX_RNDBR) q.f = (re->mod&REGEX_MULTILINE)? REGEX_LINE_BML : REGEX_LINE_B;
				break;
			case '$':
				if(i==len-1 && brack==REGEX_RNDBR) q.f = (re->mod&REGEX_MULTILINE)? REGEX_LINE_EML : REGEX_LINE_E;
				break;
			case '[':case ']':if(brack==REGEX_SQRBR) len = i;break;
			case '(':case ')':if(brack==REGEX_RNDBR) len = i;break;
			case '\0':len = i;break;
			default:
				if(brack==REGEX_SQRBR && i+2<len && exp[i+1]=='-') {
					t = re->ic(exp[i+2]);
					if((aRegex::isLower(c) && aRegex::isLower(t)) ||
						(aRegex::isUpper(c) && aRegex::isUpper(t)) ||
							(aRegex::isDigit(c) && aRegex::isDigit(t))) i += 2;
					else break;
//putchar('-');
//putchar(b);
					if(t<c) b = t,t = c,c = b;
					for(b=c; b<=t; b++) chars[b>>5] |= (1<<(b&31));
					q.c = '\0',q.f = 0;
				} else q.c = c,q.f = REGEX_CHAR;
		}
		if(brack==REGEX_RNDBR) {
printf("q.c=%c,q.f=%08" PRIx32 "\n",q.c,q.f);
			do {
				c = exp[++i];
				if(c=='?') {
					if(q.f&(REGEX_SKIP|REGEX_LOOP)) { if(!(q.f&REGEX_LAZY)) q.f |= REGEX_LAZY; }
					else q.f |= REGEX_SKIP;
				} else if(c=='+') q.f |= REGEX_LOOP;
				else if(c=='*') q.f |= REGEX_SKIP|REGEX_LOOP;
				else if(c=='{') {
printf("{\n");
					q.f |= REGEX_LOOP|REGEX_MINMAX,q.min = 0,q.max = 0;
					while(i<len-1 && aRegex::isDigit(c=exp[++i])) q.min = (q.min*10)+(c-'0');
					if(c==',') while(i<len-1 && aRegex::isDigit(c=exp[++i])) q.max = (q.max*10)+(c-'0');
					else q.max = q.min;
					if(c!='}') q.f = 0,q.min = 1,q.max = 1;
				} else c = '\0',--i;
			} while(c!='\0');
			if(q.f&REGEX_ANCHOR_MASK) q.f &= REGEX_ANCHOR_MASK;
			if(q.c!='\0' || q.f) code[j++] = q;
		} else if(brack==REGEX_SQRBR) {
			if(q.c || q.f) {
				if((b=q.c)) chars[b>>5] |= 1<<(b&31);
				else if(q.f&REGEX_NOT) {
					if(q.f&REGEX_WORD) for(b=0; b<=0xff; b++) {
						if(!aRegex::isWord(b)) chars[b>>5] |= 1<<(b&31);
					} else if(q.f&REGEX_DIGIT) for(b=0; b<=0xff; b++) {
						if(!aRegex::isDigit(b)) chars[b>>5] |= 1<<(b&31);
					} else if(q.f&REGEX_SPACE) for(b=0; b<=0xff; b++) {
						if(!aRegex::isSpace(b)) chars[b>>5] |= 1<<(b&31);
					}
				}
				else if(q.f&REGEX_WORD) for(j=0; (b=word[j]); j++) chars[b>>5] |= 1<<(b&31);
				else if(q.f&REGEX_DIGIT) for(b='0'; b<='9'; b++) chars[b>>5] |= 1<<(b&31);
				else if(q.f&REGEX_SPACE) for(j=0; (b=whitespace[j]); j++) chars[b>>5] |= 1<<(b&31);
			}
		}
	}
	if(brack==REGEX_RNDBR) len = j,code[len] = (recode){0,0,1,1};
	else if(brack==REGEX_SQRBR) {
		if(n) for(b=0; b<8; b++) chars[b] ^= 0xffffffff;
//printf("\",\"");
//		for(b=0; b<8; b++) printf("%08x",chars[b]);
		len = 8;
	}
//printf("\");\n");
}

bool REBlock::test(char c0,char c1,char c2,uint32_t i) {
	char n;
	REBlock *bl = this;
//printf("\nREBlock::test(%p,%c,%c,%c,%c,%c);\n",this,brack,c0,c1,c2,code!=NULL? code[i].c : ' ');
	if(brack==REGEX_SQRBR) {
		if(chars[c1>>5]&(1<<(c1&31))) return true;
		while(!(bl->flags&REGEX_SKIP) && (bl=bl->onfail)!=NULL && (bl=bl->prev)!=NULL);
		return bl!=NULL && bl->onfail!=NULL && bl->onfail->test(c0,c1,c2,0);
	} else if(len) {
		if(brack==REGEX_RNDBR) for(recode q; i<len; i++) {
			q = code[i],n = (q.f&REGEX_NOT)? 1 : 0;
			if(c1 && c1==q.c) return true;
			else if(q.f&REGEX_ANY) return ((re->mod&REGEX_DOTALL) || !aRegex::isBreak(c1));
			else if(q.f&REGEX_WORD) return (aRegex::isWord(c1)^n);
			else if(q.f&REGEX_DIGIT) return (aRegex::isDigit(c1)^n);
			else if(q.f&REGEX_SPACE) return (aRegex::isSpace(c1)^n);
			else if(q.f&REGEX_STRING_B) return (c0=='\0');
			else if(q.f&REGEX_STRING_E) return (c1=='\0' || ((q.f&REGEX_LINE_E) && aRegex::isBreak(c1) && c2=='\0'));
			else if(q.f&REGEX_LINE_BML) return (c0=='\0' || aRegex::isBreak(c0));
			else if(q.f&REGEX_LINE_EML) return (c1=='\0' || aRegex::isBreak(c1));
			else if(q.f&REGEX_WORD_B) {
				if(q.f&REGEX_WORD_E)
					return (((c0=='\0' || aRegex::isSpace(c0))^n) || ((c1=='\0' || aRegex::isSpace(c1))^n));
				return ((c0=='\0' || aRegex::isSpace(c0))^n);
			}
			else if(q.f&REGEX_WORD_E) return ((c1=='\0' || aRegex::isSpace(c1))^n);
			else if((!i && (flags&REGEX_SKIP)) || !(q.f&REGEX_SKIP) || i==len)
				return onfail!=NULL && onfail->test(c0,c1,c2,0);
		}
	}
	while((bl=bl->onmatch)!=NULL && !bl->len);
	return bl!=NULL && bl->test(c0,c1,c2,0);
}




class REMatch {
	public:
		aRegex *re;
		char *match,**brs;
		uint32_t pos,len,*brslen,nbrs;
		REMatch *next;

		REMatch(aRegex *re,REMatch *prev,const char *m,uint32_t p,uint32_t len);
		~REMatch();

		void backrefs(const char *t,uint32_t *r,uint32_t l);
		int backrefsLength(uint32_t *r);
};

REMatch::REMatch(aRegex *re,REMatch *prev,const char *m,uint32_t p,uint32_t len)
		: re(re),brs(NULL),pos(p),len(len),brslen(NULL),nbrs(0),next(NULL) {
	match = (char *)malloc(len+1);
	memcpy(match,m,len);
	match[len] = '\0';
	if(prev!=NULL) prev->next = this;
}

REMatch::~REMatch() {
	if(match!=NULL) { free(match);match = NULL,len = 0; }
	if(brs!=NULL) {
		for(uint32_t i=0; i<nbrs; i++) if(brs[i]!=NULL) free(brs[i]);
		free(brs);
		brs = NULL,nbrs = 0;
	}
	if(brslen!=NULL) { free(brslen);brslen = NULL; }
	next = NULL;
}

void REMatch::backrefs(const char *t,uint32_t *r,uint32_t n) {
	if(t==NULL || r==NULL || !n) return;
	uint32_t i = 0,j = 0,l = 0;
	if(brs!=NULL) { for(i=0; brs[i]; i++) free(brs[i]);free(brs); }
	if(brslen!=NULL) { free(brslen);brslen = NULL; }
	brs = (char **)malloc(n*sizeof(char *));
	brslen = (uint32_t *)malloc(n*4);
	nbrs = n;
	for(i=0; i<n; i++) {
		j = r[i],l = r[9+i];
		if((j=r[i])<(l=r[9+i])) {
			//if(j>l) j = l;
			brs[i] = (char *)malloc(l-j+1),brslen[i] = l-j;
			memcpy(brs[i],&t[j],l-j);
			//while(j<l) *b++ = *p++,j++;
			brs[i][l-j] = '\0';
		} else brs[i] = NULL,brslen[i] = 0;
	}
}

int REMatch::backrefsLength(uint32_t *r) {
	int l = 0;
	if(r[0]>0) l += r[0]*len;
	for(uint32_t i=0; i<nbrs; i++) if(r[i+1]>0 && brs[i]!=NULL && brslen[i]>0) {
//printf("REMatch::backrefsLength(i=%d,backref=%s,len=%d)\n",i,brs[i],brslen[i]);
		l += r[i+1]*brslen[i];
	}
	return l;
}



aRegex::aRegex() : mod(0),depth(1),found(0),refs(0),size(0),
		exp(NULL),mods(NULL),data(NULL),strings(NULL),text(NULL),blocks(NULL),hits(NULL),list(NULL) {
}

aRegex::~aRegex() {
	if(data!=NULL) { free(data);data = NULL; }
	clear();
}

void aRegex::clearBlocks() {
	if(exp!=NULL) { free(exp);exp = NULL; }
	if(mods!=NULL) { free(mods);mods = NULL; }
	if(blocks!=NULL) { delete blocks;blocks = NULL; }
	mod = 0,flags = REGEX_BACKREFS,refs = 0;
};

void aRegex::clearMatches() {
	if(strings!=NULL) { free(strings);strings = NULL; }
	if(found>0) {
		for(uint32_t i=0; i<found; i++) {
			if(list[i]!=NULL) delete list[i];
		}
		free(list);
		list = NULL,hits = NULL;
	}
	depth = 1,found = 0;
};


int aRegex::compile(const char *e,const char *m) {
printf("Compiling...\n");
	if(e==NULL) return 0; // If e==NULL use previous compiled code. Ignore if m is set.
	clear();
	exp = strdup(e);
	mod = 0;
	if(m!=NULL) {
		mods = strdup(m);
		while(*m) switch(*m++) {
			case 'g':mod |= REGEX_GLOBAL;break;
			case 'i':mod |= REGEX_CASELESS;break;
			case 'm':mod |= REGEX_MULTILINE;break;
			case 's':mod |= REGEX_DOTALL;break;
			case 'x':mod |= REGEX_EXTENDED;break;
		}
	}
	if(!*exp) return 0; // If e is empty, no need to compile.
	blocks = new REBlock(this,0,0,REGEX_FIRST,REGEX_RNDBR);
	REBlock *first = blocks,*last = first,*temp;
	uint32_t pos = 0,p = 0,n = 0,l = 0,d = 1;
	unsigned short min = 1,max = 1;
	char c = *exp,b=REGEX_RNDBR,sh = 0,ush = 0,oa = 0,ca = 0;
	while(true) {
//putchar(c);
		switch(c) {
			case '|':last->flags |= REGEX_OR,p = pos+1,l = pos;break;
			case '[':
				p = pos+1,n = 1,l = pos,b = REGEX_SQRBR;
				while((c=exp[++pos])!=']') /*{ putchar(c);*/if(c=='\\') /*putchar(exp[*/++pos/*])*/;/* }*/
				--pos;
				break;
			case ']':p = pos+1,n = 1,l = pos,ca = 1;break;
			case '(':p = pos+1,n = (d==1 && refs<9? REGEX_BACKREFS : 1),l = pos,b = REGEX_RNDBR,sh = 1,oa = 1;break;
			case ')':p = pos+1,n = 1,l = pos,ush = 1,ca = 2;break;
			/*case '{':
printf("{  last->pos=%d(\"%s\"),last->len=%d,p=%d,pos=%d(\"%s\") \n",
last? (int)last->pos : -1,last? &exp[last->pos] : "",last? (int)last->len : -1,p,pos,&exp[pos]);
				n = 1,l = pos,--pos,ca = 1;
				break;*/
			case '\0':l = pos;break;
			case '\\':/*putchar(c=exp[*/++pos/*])*/;
			default:
				/*if(exp[pos+1]=='{') p = pos,n = 3,l = pos;
				else */if(p || !first) n = 1,b = REGEX_RNDBR;
				break;
		}
		if(l && !last->len) last->len = l>last->pos? l-last->pos : 0,l = 0;
		if(ca) {
//putchar('~');
			l = n,n = 0,min = 1,max = 1;
			do {
				c = exp[++pos];
				if(c=='?') {
					if(n&(REGEX_SKIP|REGEX_LOOP)) { if(!(n&REGEX_LAZY)) n |= REGEX_LAZY; }
					else n |= REGEX_SKIP,min = 0;
				} else if(c=='*') n |= REGEX_SKIP|REGEX_LOOP,min = 0,max = 0;
				else if(c=='+') n |= REGEX_LOOP,max = 0;
				else if(c=='|') n |= REGEX_OR,c = 0;
				else if(c=='{') {
					n |= REGEX_LOOP|REGEX_MINMAX,min = 0,max = 0;
					while(isDigit(c=exp[++pos])) min = (min*10)+(c-'0');
					if(c==',') while(isDigit(c=exp[++pos])) max = (max*10)+(c-'0');
					else max = min;
					if(c!='}') n = 0,min = 1,max = 1;
				} else c = 0,--pos;
//if(c=='|' || !(n&1)) putchar(c);
			} while(c);
			last->setFlags(last->flags|n,min,max);
			p = pos+1,n = l,l = 0,ca = 0,c = '.';
		}
		if(n || (!c && p)) {
			if(!p) p = pos;
printf("Block: %s\n",&exp[p]);
			temp = new REBlock(this,last,p,n&0xfffffff0,b);
			if(n&REGEX_BACKREFS) temp->ref = ++refs;
			if(n&2) temp->first = temp->last = temp;
			if(ush) {
				last->flags |= REGEX_LAST;
				last->first = first,last->last = last,first->last = last;
				if(first->flags&REGEX_BACKREFS) last->flags |= REGEX_BACKREFS,last->ref = first->ref;
				while((first=first->prev))
					if(first->flags&REGEX_LAST) first = first->first;
					else if(first->flags&REGEX_FIRST) break;
				if(!first) first = blocks;
				d--;
			}
			last = temp;
			if(sh) first = last,first->first = first,first->flags |= REGEX_FIRST,d++;
			p = 0,n = 0,c = '.',b = REGEX_RNDBR,sh = 0,ush = 0,oa = 0;
		}
		if(!c) break;
		c = exp[++pos];
	}
	first = blocks;
	/*if(!first->next || !(last->flags&REGEX_LAST)) last->flags |= REGEX_LAST;
	else */last = new REBlock(this,last,pos,REGEX_LAST,REGEX_RNDBR);
	first->first = first,first->last = last,last->first = first,last->last = last;
//putchar('\n');
	first = NULL,last = blocks,p = 1,d = 2;
	while(last!=NULL) {
		if(!last->len && (!last->flags || last->flags==(REGEX_FIRST|REGEX_LAST))) {
			first->next = last->next;
			if(last->next) last->next->prev = first;
			last->next = NULL;
			delete last;
			last = first;
		}
		first = last,last = last->next;
	}
	last = blocks;
	while(last!=NULL) {
		last->compile(exp);
		if(last->flags&REGEX_OR) p++;
		if(last->flags&REGEX_FIRST) d++;
		if(d>depth) depth = d;
		if(last->flags&REGEX_LAST) d--;
		last = last->next;
	}
	first = blocks;
	REBlock *stack[depth+p];
	first = NULL,last = blocks,stack[d=0] = blocks;
	while(true) {
		if((last->flags&REGEX_FIRST) && (last->flags&REGEX_LAST))
			last->flags ^= REGEX_FIRST|REGEX_LAST;
		if(last->flags&REGEX_LAST) first = stack[--d];
		last->depth = d;
		if(last->flags&REGEX_FIRST) stack[++d] = first = last;
		if(last->first==NULL) last->first = first,last->last = first->last;
		if(last->next==NULL) break;
		last = last->next;
	}

	first = last,stack[d=0] = last = NULL,stack[depth+(p=0)] = temp = NULL,l = 0;
	while(first!=NULL) {
		if(first->flags&REGEX_LAST) stack[++d] = last = first;
		if(first->flags&REGEX_OR) {
			first->onfail = first->next;
			first->onmatch = first->next && first->next->last? first->next->last->onmatch : 0;
			stack[depth+ ++p] = temp = first;
		}
		if(first->flags&REGEX_FIRST) {
			last = stack[--d];
			while(p>0 && temp!=NULL && (temp->depth>first->depth || first==temp))
				temp = stack[depth+ --p];
		}
		if(!(first->flags&REGEX_OR)) {
			first->onmatch = first->next;
			if(first->flags&REGEX_SKIP) first->onfail = first->next;
			else first->onfail = (temp!=NULL? temp->next : (first->last!=NULL? first->last->onfail : NULL));
		}
		if(first==first->onmatch) first->onmatch = first->onmatch->next;
		if(first==first->onfail) first->onfail = NULL;
		if((first->flags&REGEX_FIRST) && (first->flags&REGEX_LAST))
			first->first = first->last = first;
		first = first->prev,l++;
	}
	return l;
}


//#define PRINT_OUTPUT
#ifdef PRINT_OUTPUT
#	define PUTCHAR(c) putchar(c)
#	define PRINTF(...) printf(__VA_ARGS__)
#	define PRINTSETSTACK(s1,s2,s3,s4) printSetStack(s1,s2,s3,s4)
#	define PRINTGETSTACK(s1,s2) printGetStack(s1,s2)
#else
#	define PUTCHAR(c)
#	define PRINTF(...)
#	define PRINTSETSTACK(s1,s2,s3,s4)
#	define PRINTGETSTACK(s1,s2)
#endif


struct blockstack {
	REBlock *bl;
	uint32_t p;
	int i,l,a,loops,s;
};

blockstack *resizeStack(blockstack *st,uint32_t &stsz,uint32_t size) {
	stsz = stsz<size && stsz*2>size? size : stsz*2;
	return (blockstack *)realloc(st,stsz*sizeof(blockstack));
}

#ifdef PRINT_OUTPUT
void printSetStack(blockstack &st,uint32_t stn,char c,int n) {
printf("\nSET STACK! - - - - - - - - - - - - - - - - - - - - - - - - - - - - -[%lu]%c %d\n\
bl=%p,p=%lu,i=%d,l=%d,a=%d,loops=%d,s=%d\n",stn,c,n,st.bl,st.p,st.i,st.l,st.a,st.loops,st.s);
}
void printGetStack(blockstack &st,uint32_t stn) {
printf("\nGET STACK! -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-[%lu]\n\
bl=%p,p=%lu,i=%d,l=%d,a=%d,loops=%d,s=%d\n",stn,st.bl,st.p,st.i,st.l,st.a,st.loops,st.s);
}
#endif


int aRegex::search(const char *t) {
printf("Searching...\n");
PRINTF("sizeof(blockstack)=%d\n",sizeof(blockstack));
	if(t!=NULL) setText(t);
	if(text==NULL || !*text || blocks==NULL) return 0;
	clearMatches();
	REBlock *bl,*bl0 = blocks,*bl1,*bln,*blr;
	REMatch *match = NULL;
	uint32_t i = 0,l = 0,a = 0,r = 0,s = 0,n = 0,hit = 0;
	recode q = { 0,0,0,0 };
	uint32_t test = 0,scan = 0,rscan,p = 0,loops = 1,backrefs[18],stsz = size<32? size : 32,stn = 0;
	bool lazy = false;
	blockstack *st = (blockstack *)malloc(stsz*sizeof(blockstack));
	char cc0 = '\0',cc1 = ic(*text),cc2 = cc1=='\0'? cc1 : ic(text[1]),c0 = '\0',c1 = '\0',c2 = '\0';
	while(bl0!=NULL && !bl0->len) bl0 = bl0->next;
	bl = bl0;
#ifdef PRINT_OUTPUT
print();
#endif
	clock_t time1 = clock();
	while(cc1!='\0') {
		if(!bl->test(cc0,cc1,cc2,0)) goto search_next_char;
		bl = blocks;
		if(flags&REGEX_BACKREFS) for(i=0; i<18; i++) backrefs[i] = 0;

		loops = 1,hit = 1;
		while(bl!=NULL) {
			if(!bl->len) goto blockmatch_end;

			p = scan,i = 0,l = 0,a = 0,r = 0,n = 0;
			bln = bl,lazy = false,hit = 0;
			c0 = (p>0? ic(text[p-1]) : '\0'),c1 = ic(text[p]),c2 = c1=='\0'? c1 : ic(text[p+1]);


blockmatch_start:
PRINTF("[%p]blockmatch_start: %lu[%c%c%c]\n",bl,test,cc0,cc1,cc2);



PRINTF("[%p]Match(brack='%c',lazy=%d,p=%lu,\"",bl,bl->brack,lazy,p);
			if(bl->brack==REGEX_SQRBR) {
PRINTF("\",\"");
				while((bln=bln->onmatch)!=NULL && !bln->len);
				while(true) {
PUTCHAR(c1);
					if(!(bl->chars[c1>>5]&(1<<(c1&31))) || (bl->max && i==bl->max)) break; // Check for match
					c0 = c1,c1 = c2,c2 = ic(text[c2=='\0'? ++i+p : ++i+p+1]),a++; // Shift chars one step
					if(c1=='\0' || !(bl->flags&REGEX_LOOP)) break; // Break if eof or no looping
					else if(bln!=NULL && bln->test(c0,c1,c2,0)) { // Test if next block match char
						 // Same as "Check for match" above. No need to set print in stack or check for lazy because this block
						if(!(bl->chars[c1>>5]&(1<<(c1&31))) || (bl->max && i==bl->max)) break; //  won't search more anyway.
						if(stn==stsz) st = resizeStack(st,stsz,size);
						st[stn++] = (blockstack){bl,p+i,0,0,0,loops,1}; // Push print to stack
PRINTSETSTACK(st[stn-1],stn-1,c1,1);
						if(lazy || bl->flags&REGEX_LAZY) break; // If block is lazy search go to next block
					}
				}
				if((i && i>=bl->min) || (bl->flags&REGEX_SKIP)) p = p+i,s = 1,hit = 1;
			} else if(bl->brack==REGEX_RNDBR) {
#ifdef PRINT_OUTPUT
for(s=0; s<bl->len; s++) putchar(bl->code[s].c);
#endif
PRINTF("\",c012=\"%c%c%c\",\"",c0,c1,c2);
				for(q.f=0,s=0; c1!='\0';) {
					if(!q.f) {
						if(l==bl->len) break;
						q = bl->code[l++],s = 0,a = 0,r = 0,n = (q.f&REGEX_NOT)? 1 : 0;
PUTCHAR('~');
PUTCHAR(q.c);
						if(l==bl->len) while((bln=bln->onmatch) && !bln->len);
					}
PUTCHAR(c1);
					if(q.f&REGEX_CHAR) s = (c1==q.c)? 1 : 2;
					else if(q.f&REGEX_ANY) {
						s = (((mod&REGEX_DOTALL) || !isBreak(c1))/* &&
							(!(q.f&REGEX_SKIP) || bln==NULL || !bln->test(c0,c1,c2,bln==bl? l : 0))*/)? 1 : 2;
					}
					else if(q.f&REGEX_WORD) s = (isWord(c1)^n)? 1 : 2;
					else if(q.f&REGEX_DIGIT) s = (isDigit(c1)^n)? 1 : 2;
					else if(q.f&REGEX_SPACE) s = (isSpace(c1)^n)? 1 : 2;
					else if(q.f&REGEX_STRING_B) { if(c0=='\0') q.f = 0;else s = 2; }
					else if(q.f&REGEX_STRING_E) { if(c1=='\0' || ((q.f&REGEX_LINE_EML) && isBreak(c1) && c2=='\0')) q.f = 0;else s = 2; }
					else if(q.f&REGEX_LINE_BML) { if(c0=='\0' || isBreak(c0)) {putchar('*');q.f = 0;}else s = 2; }
					else if(q.f&REGEX_LINE_EML) { if(c1=='\0' || isBreak(c1)) q.f = 0;else s = 2; }
					else if(q.f&REGEX_WORD_B) {
						if(q.f&REGEX_WORD_E) { // REGEX_WORD_BE
							if(((c0=='\0' || isSpace(c0))^n) || ((c1=='\0' || isSpace(c1))^n)) q.f = 0;
							else s = 2;
						} else if((c0=='\0' || isSpace(c0))^n) q.f = 0;
						else s = 2;
					}
					else if(q.f&REGEX_WORD_E) { if((c1=='\0' || isSpace(c1))^n) q.f = 0;else s = 2; }
					else if(q.f&REGEX_SKIP) q.f = 0;
					else { if(!a) l=0;break; }
					if(s) {
PUTCHAR(s==1? ((q.f&REGEX_LOOP)? '*' : '+') : s==2? '-' : '?');
						if(s==2 && (q.f&REGEX_LOOP) && l<bl->len) q.f = 0;
						else {
							if(s==2 && !(q.f&REGEX_SKIP)) { if(r>0) a -= i-r,i = r;if(a<=0) l = 0;break; }
							s = 0,c0 = c1,c1 = c2,c2 = ic(text[c2=='\0'? ++i+p : ++i+p+1]),a++;
							if(c1=='\0') {
PRINTF("EOF");
							/*break;*/ }
						}
					}
					if(q.f) {
						if(!(q.f&REGEX_LOOP)) q.f = 0;
						else if(bln && bln->test(c0,c1,c2,bln==bl? l : 0)) {
							if(stn==stsz) st = resizeStack(st,stsz,size);
							if(lazy || (q.f&REGEX_LAZY)) st[stn++] = (blockstack){bl,p,i,l,a,loops,2},q.f = 0;
							else if(l==bl->len) st[stn++] = (blockstack){bln,p+i,0,0,0,loops,3},r = i;
							else if(q.f&REGEX_SKIP) st[stn++] = (blockstack){bl,p,i,l,a,loops,2},r = i;
PRINTSETSTACK(st[stn-1],stn-1,c1,2);
						}
					}
				}
				if(l==bl->len) p = p+i,s = 1,hit = 1;
			}
PRINTF("\",i=%lu,l=%lu,stn=%lu,len=%lu); - %s\n",i,l,stn,bl->len,hit? "Hit!" : "Nope");


blockmatch_end:
PRINTF("[%p]blockmatch_end: %lu[%c%c%c]\n",bl,test,cc0,cc1,cc2);


			if(hit) {
				blr = bl,rscan = scan;
PRINTF("scan=%lu p=%lu\n",scan,p);
				if(bl->len && p) scan = p;
				if(bl->brack==REGEX_RNDBR && (bl->flags&REGEX_LOOP)) {
					bl1 = bl;
					while((bl1=bl1->onmatch)!=NULL && !bl1->len);
					if(text[scan]!='\0' && (bl1==NULL || !bl1->test(text,scan,0)) && (!bl->max || loops<bl->max)) {
						loops++,bl = bl->first,hit = 2;
					} else if(loops<bl->min) hit = 0;
					else bl = bl->onmatch,loops = 1;
//printf("loop(bl=0x%08x,bl1=0x%08x,ls=%d,repeats=%d,loops=0x%08x);\n",bl,bl1,ls,repeats[ls],loops[ls]);
				} else bl = bl->onmatch;
				if(hit && blr!=NULL) {
					if(s==1 && blr->len && bl!=NULL) {
						if(stn==stsz) st = resizeStack(st,stsz,size);
						st[stn++] = (blockstack){bl,scan,0,l,0,loops,4};
PRINTSETSTACK(st[stn-1],stn-1,c1,3);
					}
					if(hit!=2 && (blr->flags&REGEX_BACKREFS)) {
						if((!(blr->flags&(REGEX_FIRST|REGEX_LAST)) &&
								(!(blr->last->flags&REGEX_LOOP) || /*repeats[ls]*/loops==2)) ||
							(((blr->flags&REGEX_FIRST) ||
								(blr->prev && (blr->prev->flags&REGEX_OR))) &&
									(!(blr->last->flags&REGEX_LOOP) || /*repeats[ls]*/loops==1))) {
										backrefs[blr->ref-1] = rscan;
PRINTF("SetBackRefFirst(rscan=%lu,loops=%lu);\n",rscan,loops);
									}
						if(!(blr->flags&(REGEX_FIRST|REGEX_LAST)) ||
							(blr->flags&REGEX_LAST) ||
								((blr->flags&REGEX_OR) && (blr->flags&REGEX_FIRST))) {
PRINTF("SetBackRefLast(scan=%lu,loops=%lu);\n",scan,loops);
									backrefs[9+blr->ref-1] = scan;
								}
					}
				}
			}
			if(!hit) {
				bl1 = bl->onfail,loops = 1,s = 0;
				while(bl1!=NULL && !bl1->len) bl1 = bl1->onmatch;

get_from_stack:
PRINTF("[%p]get_from_stack: %lu[%c%c%c] %lu\n",bl,test,cc0,cc1,cc2,stn);

				if(bl1==NULL && stn>0) {
					if((!(bl->flags&REGEX_SKIP) || bl==bl->first->last) && (bl->first->last->flags&REGEX_SKIP)) {
PRINTF("[%p]Clear Stack. [%p] %lu\n",bl,bl1,stn);
						while(stn>0 && st[stn-1].bl>bl->first) stn--;
						if(stn>0) s = 1;
						else goto search_next;
					}
					blockstack &st1 = st[stn-1];
PRINTGETSTACK(st1,stn-1);
					scan = p = st1.p,i = st1.i,l = st1.l,a = st1.a,loops = st1.loops,lazy = true,hit = 0;
					c0 = (p+i>0? ic(text[p-1+i]) : '\0'),c1 = ic(text[p+i]),c2 = c1=='\0'? c1 : ic(text[p+1+i]);
					if(s==1) st1.bl = NULL;
					else if(st1.s==1 || st1.s==3) scan -= i,stn--;
					else if(st1.s==4 && st1.bl!=NULL) {
						bl1 = st1.bl->onfail;
						while(bl1!=NULL && !bl1->len) bl1 = bl1->onmatch;
						if(bl1==NULL) { if(st1.bl->flags&REGEX_SKIP) st1.bl = NULL;else {stn--;goto get_from_stack;} }
						else st1.bl = bl1;
					} else stn--;
					bl = st1.bl,bln = bl;
PRINTF("[%p]Stack. stn=%lu,s=%lu\n",bl,stn,s);
					if(bl==NULL) { hit = 1;goto collect_match; }
					if(st1.s==1 || !bl->len) { s = 0,hit = 1;goto blockmatch_end; }
					else goto blockmatch_start;
				} else bl = bl1;
			}
//printf("bl=0x%08x scan=%d d=%d scanstack=%d;\n",bl,scan,d,scanstack[d]);
		}
//printf("test(test=%d,scan=%d)\n",test,scan);


collect_match:
PRINTF("[%p]collect_match: %lu[%c%c%c] hit=%lu,scan=%lu\n",bl,test,cc0,cc1,cc2,hit,scan);


		if(hit && scan>test) {
			if(bl!=NULL && text[scan]=='\0')
				while(bl!=NULL && (!bl->len || (bl->flags&REGEX_SKIP))) bl = bl->onmatch;
			if(bl==NULL) {
				match = new REMatch(this,match,&text[test],test,scan-test),found++;
				if(flags&REGEX_BACKREFS) match->backrefs(text,backrefs,refs);
				if(hits==NULL) hits = match;
PRINTF("Match: p=%lu,i=%lu,scan=%lu,test=%lu,match=\"%s\",found=%lu\n",p,i,scan,test,match->match,found);
				if(!(mod&REGEX_GLOBAL)) break;
				test = scan-1;
				cc1 = ic(text[test]),cc2 = cc1=='\0'? cc1 : ic(text[test+1]);
			}
		}


search_next:
		bl = bl0,stn = 0;

search_next_char:

		cc0 = cc1,cc1 = cc2,cc2 = ic(text[cc1=='\0'? ++test : ++test+1]),scan = test;
PRINTF("[%p]search_next: %lu[%c%c%c]\n",bl,test,cc0,cc1,cc2);
	}
	clock_t time2 = clock();
	free(st);
	if(found>0) {
		strings = (char **)malloc(found*sizeof(char *));
		list = (REMatch **)malloc(found*sizeof(REMatch *));
		match = hits;
		for(i=0; i<found; i++)
			strings[i] = match->match,list[i] = match,match = match->next;
	}
printf("\nSearch done in %" PRIu64 " (%" PRIu64 ",%" PRIu64 ") ticks, matches found: %" PRIu32 " stsz=%" PRIu32 "\n",(uint64_t)(time2-time1),(uint64_t)time1,(uint64_t)time2,found,stsz);
	return found;
}


const char *aRegex::replace(const char *t,const char *r) {
	if(search(t)>0 && r!=NULL) {
printf("\nReplace: %s\n",r);
		uint32_t sz = size,i,n,br[10],l;
		for(i=0; i<=9; i++) br[i] = 0;
		const char *s1,*s3 = r;
		char *s2,c,*del = data;
		REMatch *m = hits;
		for(l=strlen(r); *s3; s3++) if(*s3=='\\' || *s3=='$') {
			if(*++s3=='\0') break;
			if(isDigit(*s3)) br[*s3-'0']++,l -= 2;
			else if(*s3=='&') br[0]++,l -= 2;
			else l--;
		}
		while(m!=NULL) {
			n = l+m->backrefsLength(br)-m->len;
printf("Backrefs Length: l=%" PRIu32 ", n=%" PRIu32 ", m->len=%" PRIu32 "\n",l,n,m->len);
			sz += n,m = m->next;
		}
printf("Size: size=%" PRIu32 " subst=%" PRIu32 "\n",size,sz);
		for(m=hits,data=(char *)malloc(sz+1),s1=text,s2=data,i=0; i<=size;)
			if(m!=NULL) {
printf("Match: %s (len=%" PRIu32 ")\n",m->match,m->len);
				memcpy(s2,s1,m->pos-i);
				s1 += m->pos-i,s2 += m->pos-i,i = m->pos;

				for(s3=r,l=0; (c=*s3); s3++,l++) if(c=='\\' || c=='$') {
					if(*++s3=='\0') { *s2++ = c;break; }
					if(*s3=='0' || *s3=='&')  { memcpy(s2,m->match,m->len);s2 += m->len; }
					else if(m->brslen[n=*s3-'1']>0) { memcpy(s2,m->brs[n],m->brslen[n]);s2 += m->brslen[n]; }
				} else *s2++ = c;
				s1 += m->len,i += m->len,m = m->next;
			} else {
				memcpy(s2,s1,size+1-i);
				break;
			}
		if(del!=NULL) free(del);
		text = data,size = sz;
	}
	return text;
}

int aRegex::match(const char *t,const char *e,const char *m) {
	return compile(e,m)? search(t) : 0;
}

const char *aRegex::replace(const char *t,const char *e,const char *r,const char *m) {
	return compile(e,m)? replace(t,r) : NULL;
}


void aRegex::print() {
	if(blocks==NULL) return;
	REBlock *bl = blocks;
	uint32_t i,j,c,m,n;
	recode q;
	printf("\n\nText:\n%s\n\nExpression: %s\nModifiers: %s\n",text,exp,mods);
	printf("\nBlock: First: Last:  Match: Fail:  Flags:         L D BR Code:\n");
	while(bl!=NULL) {
		printf("%06" PRIxPTR " %06" PRIxPTR " %06" PRIxPTR " %06" PRIxPTR " %06" PRIxPTR " %08" PRIx32 " %c%hu-%hu %2" PRIu32 " %" PRIu32 " ",
				(intptr_t)bl,(intptr_t)bl->first,(intptr_t)bl->last,(intptr_t)bl->onmatch,(intptr_t)bl->onfail,
					bl->flags,bl->brack,bl->min,bl->max,bl->len,bl->depth);
		m = bl->flags;
		if(m&REGEX_BACKREFS) printf("\\%" PRIu32 " ",bl->ref);
		else printf("   ");
		for(i=0; i<bl->depth; i++) printf("   ");
		if(bl->brack==REGEX_SQRBR) {
			putchar('[');
			for(i=0,n=0; i<=0xff && n<80; i++) if(bl->chars[i>>5]&(1<<(i&31))) {
				if(isDigit(i) || isAlpha(i)) {
					c = isDigit(i)? '9' : isUpper(i)? 'Z' : 'z';
					for(j=i; j<=c && (bl->chars[(j+1)>>5]&(1<<((j+1)&31))); j++);
					if(j-i>1) { putchar(i);putchar('-');putchar(j);i = j,n += 3; }
					else { putchar(i);n++; }
				} else if(isPrint(i)) { putchar(i);n++; }
			}
			putchar(']');
		} else if(bl->brack==REGEX_RNDBR) {
			if((m&REGEX_FIRST) || !(m&(REGEX_FIRST|REGEX_LAST))) putchar('(');
			for(i=0; i<bl->len; i++) {
				q = bl->code[i];
				if(q.f&REGEX_ANY) putchar('.');
				else if(q.f&REGEX_SPCH_MASK) {
					putchar('\\');
					if(q.f&REGEX_WORD) putchar((q.f&REGEX_NOT)? 'W' : 'w');
					else if(q.f&REGEX_DIGIT) putchar((q.f&REGEX_NOT)? 'D' : 'd');
					else if(q.f&REGEX_SPACE) putchar((q.f&REGEX_NOT)? 'S' : 's');
				} else if(q.f&REGEX_ANCHOR_MASK) {
					if((q.f&REGEX_LINE_BML) || (!(mod&REGEX_MULTILINE) && (q.f&REGEX_LINE_B)==REGEX_LINE_B)) putchar('^');
					else if((q.f&REGEX_LINE_EML) || (!(mod&REGEX_MULTILINE) && (q.f&REGEX_LINE_E)==REGEX_LINE_E)) putchar('$');
					else {
						if((q.f&REGEX_WORD_BE)==REGEX_WORD_BE) {putchar('\\');putchar((q.f&REGEX_NOT)? 'B' : 'b');}
						else if(q.f&REGEX_WORD_B) {putchar('\\');putchar('<');}
						else if(q.f&REGEX_WORD_E) {putchar('\\');putchar('>');}
						else if(q.f&REGEX_STRING_B) {putchar('\\');putchar('A');}
						else if(q.f&REGEX_STRING_E) {putchar('\\');putchar('Z');}
					}
				} else {
					if(strchr(".\\*+?()[]{}",q.c)!=NULL) putchar('\\');
					putchar(q.c);
				}
				if(q.f&REGEX_MINMAX) {
					printf("{%d",q.min);
					if(q.max!=1 && q.max!=q.min) { putchar(',');if(q.max>1) printf("%d",q.max); }
					putchar('}');
				} else if((q.f&REGEX_SKIP) && (q.f&REGEX_LOOP)) putchar('*');
				else if(q.f&REGEX_SKIP) putchar('?');
				else if(q.f&REGEX_LOOP) putchar('+');
				if(q.f&REGEX_LAZY) putchar('?');
				//printf("%08x",q.f);
			}
			if((m&REGEX_LAST) || !(bl->flags&(REGEX_FIRST|REGEX_LAST))) putchar(')');
		}
		if(m&REGEX_MINMAX) {
			printf("{%d",bl->min);
			if(bl->max!=1 && bl->max!=bl->min) { putchar(',');if(bl->max>1) printf("%d",bl->max); }
			putchar('}');
		} else if((m&REGEX_SKIP) && (m&REGEX_LOOP)) putchar('*');
		else if(m&REGEX_SKIP) putchar('?');
		else if(m&REGEX_LOOP) putchar('+');
		if(m&REGEX_LAZY) putchar('?');
		if(m&REGEX_NOT) putchar('^');
		if(m&REGEX_OR) putchar('|');
		//printf("%08x",m);
		putchar('\n');
		bl = bl->next;
	}
	if(found) {
		printf("\nNumber of matches: %" PRIu32 "\nNumber of backrefs: %" PRIu32 "\n",found,refs);
		printf("\n%-32s\tBackreferences:\n","Found Matches:");
		for(i=0; i<found; i++) {
			printf("%2" PRIu32 ". %-28s\t[",i,strings[i]);
			for(uint32_t j=0; j<refs; j++) printf(" %-8s",j<list[i]->nbrs && list[i]->brs[j]!=NULL? list[i]->brs[j] : "-");
			printf(" ]\n");
		}
	}
	printf("\n\n");
}

void aRegex::setText(const char *t) { text = t,size = strlen(text); }

const char **aRegex::getMatches() {
	return found? (const char **)(strings? strings : &hits->match) : &blank;
}

const char *aRegex::getMatch(int n) {
	return found? (const char *)(strings? strings[n] : hits->match) : blank;
}

const char *aRegex::getBackRef(int n,int r) {
	return n>=0 && (uint32_t)n<found && r>=0 && (uint32_t)r<refs && list[n]->brs[r]?
         (const char *)list[n]->brs[r] : blank;
}


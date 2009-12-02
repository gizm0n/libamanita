
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <SDL/SDL_endian.h>
#include <libamanita/aMath.h>
#include <libamanita/aString.h>
#include <libamanita/aRandom.h>
#include <libamanita/aInteger.h>




aInteger::~aInteger() {
	free(num);
	sign = Plus,num = 0,len = cap = 0;
}

void aInteger::resize(int n) {
	if(n==0) return;
// printf("resize(len=%d,cap=%d,n=%ld)\n",len,cap,n);fflush(stdout);
	len += n;
	if(n>0) {
		if(len>cap) cap = len,num = (uint32_t *)realloc(num,cap*4);
// printf("resize(len=%d,cap=%d,n=%ld)\n",len,cap,n);fflush(stdout);
		memset(&num[len-n],0,n*4);
	}
// printf("resize\n");fflush(stdout);
}

void aInteger::set32(int32_t n) {
	if(n<0) sign = Minus,n = -n;
	else sign = Plus;
	if(!num) num = (uint32_t *)malloc(4),len = cap = 1;
	*num = n;
}

void aInteger::setu32(uint32_t n) {
	sign = Plus;
	if(!num) num = (uint32_t *)malloc(4),len = cap = 1;
	*num = n;
}

void aInteger::set64(int64_t n) {
	if(n<0ll) sign = Minus,n = -n;
	else sign = Plus;
	len = n>0xffffffffll? 2 : 1;
	if(!num) num = (uint32_t *)malloc(len*4),cap = len;
	else if(len>cap) {
		cap = len;
		num = (uint32_t *)realloc(num,cap*4);
	}
	*num = n&0xffffffff;
	if(len==2) num[1] = (n>>32)&0xffffffff;
}

void aInteger::setu64(uint64_t n) {
	len = n>0xffffffffull? 2 : 1;
	if(!num) num = (uint32_t *)malloc(len*4),cap = len;
	else if(len>cap) {
		cap = len;
		num = (uint32_t *)realloc(num,cap*4);
	}
	sign = Plus;
	*num = n&0xffffffff;
	if(len==2) num[1] = (n>>32)&0xffffffff;
}

void aInteger::set(const char *n) {
	if(*n=='-') sign = Minus,n++;
	else sign = Plus;
	size_t l = strlen(n),p;
	uint32_t m,c;
	int i = l/8+((l%8)!=0),j;
	if(!num) num = (uint32_t *)malloc(i*4),len = cap = i;
	else resize(i-len);
// printf("aInteger(n=%s,l=%d,\"",n,l);
	for(i=l-1,m=0,p=0; i>=0; i-=8) {
		for(j=0; j<8; j+=2) {
			c = 0;
			if(i-j-1>=0) /*{*/c |= aString::fromHex(n[i-j-1])<<4;//putchar(n[i-j-1]);}
			if(i-j>=0) /*{*/c |= aString::fromHex(n[i-j]);//putchar(n[i-j]);}
			m = (m<<8)|c;
		}
		num[p++] = SDL_SwapBE32(m),m = 0,j = 8;
	}
// printf("\")\n");
}

void aInteger::set(const aInteger &n) {
	if(!num) num = (uint32_t *)malloc(n.len*4),len = cap = n.len;
	else if(n.len>cap) {
		cap = n.len;
		num = (uint32_t *)realloc(num,cap*4);
	}
	sign = n.sign,len = n.len;
	memcpy(num,n.num,len*4);
}

void aInteger::inc(uint32_t n) {
	for(size_t i=0; i<len; i++) {
		if(num[i]+n>num[i]) { num[i] += n;break; }
		if(i==len-1) resize(1);
		num[i] += n,n = 1;
	}
// printf("aInteger::inc(");print();printf(")\r");
}

void aInteger::dec(uint32_t n) {
	for(size_t i=0; i<len; i++) {
		if(n<=num[i]) { num[i] -= n;break; }
		if(len==1) sign = sign==Plus? Minus : Plus,*num = n-*num;
		else num[i] -= n,n = 1;
	}
}

void aInteger::add(const aInteger &n) {
// printf("add(");print();printf(" + ");n.print();printf(")\n");fflush(stdout);
// printf("add(len=%d,n.len=%d,sign=%c,n.sign=%c)\n",len,n.len,sign==Plus? '+' : '-',n.sign==Plus? '+' : '-');
	if(n.len>len) resize(n.len-len);
	size_t i = 0,t = 0;
	for(; i<len && (i<n.len || t); i++) {
// printf("add(i=%d,t=%I64u)\n",i,t);
		if(i<n.len) {
			if(n.num[i] && num[i]+t+n.num[i]<=num[i]) num[i] += t+n.num[i],t = 1;
			else num[i] += t+n.num[i],t = 0;
		} else num[i] += t,t = 0;
	}
// printf("add(t=%I64u)\n",t);
	if(t) { resize(1);num[len-1] = t; }
// printf("add( = ");print();printf(")\n");fflush(stdout);
}

void aInteger::sub(const aInteger &n) {
// printf("sub(");print();printf(" - ");n.print();printf(")\n");fflush(stdout);
// printf("sub(len=%d,n.len=%d,sign=%c,n.sign=%c,this=",len,n.len,sign==Plus? '+' : '-',n.sign==Plus? '+' : '-');
// print();printf(",n=");n.print();printf(")\n");
	if(n.isZero()) return;
	// if(len<n.len || (len==n.len && num[len-1]<n.num[len-1])) sign = sign==Plus? Minus : Plus;
	for(size_t i=0,t=0; i<len && (i<n.len || t); i++) {
// printf("sub(i=%d,t=%d,num[i]=%lx,n.num[i]=%lx)\n",i,t,num[i],i<n.len? n.num[i] : 0);
		if(t) { if(num[i]) t = 0;num[i]--; }
		if(i<n.len) { if(num[i]<n.num[i]) t++;num[i] -= n.num[i]; }
	}
	while(len>1 && num[len-1]==0) len--;
	//if(sign==Minus && len==1 && !*num) sign = Plus;
// printf("sub( = ");print();printf(")\n");fflush(stdout);
}

void aInteger::mul(const aInteger &n) {
// printf("mul(");print();printf(" * ");n.print();printf(")\n");fflush(stdout);
	if(sign==Minus && n.sign==Minus) sign = Plus;
	else if(sign==Plus && n.sign==Minus) sign = Minus;
	if(len==1 && n.len==1 && *num<=0xffff && *n.num<=0xffff) *num *= *n.num;
	else {
		aInteger p;
		p.resize(len+n.len);
		p.sign = sign;
		size_t i,j;
		uint64_t w;
		uint32_t c;
		for(i=0; i<n.len; p.num[i+len]=c,i++) for(j=0,c=0; j<len; j++)
			w = num[j],w *= n.num[i],w += p.num[i+j],w += c,p.num[i+j] = w&0xffffffff,c = w>>32;
		while(p.len>1 && p.num[p.len-1]==0) p.len--;
		set(p);
		/*aInteger t(*this),p,q(n); // The bitshift-multiply has been replaced by a matrix-multiply.
		size_t l,s;
		uint32_t d;
		do {
			if(*q.num&1) p += t,s = 1;
			else { // To minimize number of shifts, precalculate number of zeros.
				for(l=0ul; !q.num[l] && l<q.len; l++);
				for(d=q.num[l],s=l*32ul; !(d&1); s++,d>>=1);
			}
			q.rshift(s);
			t.lshift(s);
		} while(!q.isZero());
		*this = p;*/
	}
// printf("mul( = ");print();printf(")\n");fflush(stdout);
}

void aInteger::div(const aInteger &n,aInteger *q,aInteger *r) {
// printf("div(");print();printf(" / ");n.print();printf(")\n");fflush(stdout);
	if(sign==Minus && n.sign==Minus) sign = Plus;
	size_t i = 0,s = 0;
	if(n.isZero()) i = 1/i;
	if(len==1 && n.len==1) {
		if(q) q->setu32(*num / *n.num);
		if(r) r->setu32(*num % *n.num);
	} else {
		int m = cmp(n);
		if(m==0) {
			if(q) q->setu32(1);
			if(r) r->setu32(0);
		} else if(m<0) {
			if(q) q->setu32(0);
			if(r) r->set(*this);
		} else {
			aInteger a(*this),b(n),c,d(b);
// printf("div1(a=");a.print();printf(",b=");b.print();printf(",c=");c.print();printf(")\n");
			while(b<a) { b.lshift(1);s++; }
// printf("div2(a=");a.print();printf(",b=");b.print();printf(",c=");c.print();printf(")\n");
			if(b>a) { b.rshift(1);s--; }
// printf("div3(a=");a.print();printf(",b=");b.print();printf(",c=");c.print();printf(")\n");
			for(i=0ul; i<=s; i++) {
				m = b.cmp(a);
// printf("div4(cmp=%d,a=",m);a.print();printf(",b=");b.print();printf(",c=");c.print();printf(")\n");
				if(m<=0) { a -= b;b.rshift(1);c.lshift(1);*c.num |= 1; }
				else { b.rshift(1);c.lshift(1); }
			}
// printf("div( = ");c.print();printf(" mod=");a.print();printf(")\n");fflush(stdout);
			if(q) q->set(c);
			if(r) r->set(a);
		}
	}
// printf("div\n");fflush(stdout);
}

void aInteger::lshift(size_t n) {
// printf("lshift(n=%d,num=%p)\n",n,num);
	if(n==0) return;
	size_t l = n/32,m = n%32;
	if(l || (m && num[len-1]>=(1u<<(32-m)))) resize(l+(m && num[len-1]>=(1u<<(32-m))));
	for(long i=len-l-1; i>=0; i--) {
// printf("lshift(l=%d,i=%ld)\n",l,i);
		num[i+l] = (num[i]<<m)|(m && i>0? num[i-1]>>(32-m) : 0);
	}
	if(l) memset(num,0,l*4);
}

void aInteger::rshift(size_t n) {
// printf("rshift(n=%d,num=%p)\n",n,num);
	if(n==0) return;
	size_t l = n/32,m = n%32;
	if(m) { // For shifting 1-31 bit part of 32 bit integers (n%32>0).
		if(l>len || (l==len && num[len-1]<(0xffffffff>>m))) sign = Plus,*num = 0,len = 1;
		else {
			for(size_t i=l; i<len; i++) num[i-l] = (i<len-1? num[i+1]<<(32-m) : 0)|(num[i]>>m);
			len -= l;
			while(len>1 && !num[len-1]) len--;
		}
	} else { // For shifting whole 32 bit integers (n%32==0).
		if(l>len) sign = Plus,*num = 0,len = 1;
		else {
			for(size_t i=l; i<len; i++) num[i-l] = num[i];
			len -= l;
		}
	}
}

int aInteger::cmp(int32_t n) {
	if(sign==Minus) {
		if(n>=0 || len>1) return -1;
		n = -n;
		if(*num!=(uint32_t)n) return *num>(uint32_t)n? -1 : 1;
	} else {
		if(n<0 || len>1) return 1;
		if(*num!=(uint32_t)n) return *num>(uint32_t)n? 1 : -1;
	}
	return 0;
}

int aInteger::cmp(uint32_t n) {
	if(len>1) return 1;
	if(*num!=n) return *num>n? 1 : -1;
	return 0;
}

int aInteger::cmp(int64_t n) {
	if(sign==Minus) {
		if(n>=0 || len>2) return -1;
		n = -n;
		if(num[1]!=(uint32_t)(n>>32)) return num[1]>(uint32_t)(n>>32)? -1 : 1;
		if(*num!=(n&0xffffffff)) return *num>(n&0xffffffff)? -1 : 1;
	} else {
		if(n<0 || len>2) return 1;
		if(len<2) return -1;
		if(num[1]!=(uint32_t)(n>>32)) return num[1]>(uint32_t)(n>>32)? 1 : -1;
		if(*num!=(n&0xffffffff)) return *num>(n&0xffffffff)? 1 : -1;
	}
	return 0;
}

int aInteger::cmp(uint64_t n) {
	if(len>2) return 1;
	if(len<2) return -1;
	if(num[1]!=(n>>32)) return num[1]>(n>>32)? 1 : -1;
	if(*num!=(n&0xffffffff)) return *num>(n&0xffffffff)? 1 : -1;
	return 0;
}

int aInteger::cmp(const aInteger &n) {
// printf("cmp(");print();printf(",n=");n.print();printf(")\n");
	if(sign==Minus) {
		if(n.sign==Plus) return -1;
		if(len!=n.len) return len>n.len? -1 : 1;
	} else {
		if(n.sign==Minus) return 1;
		if(len!=n.len) return len>n.len? 1 : -1;
	}
	for(long i=len-1; i>=0l; i--) if(num[i]!=n.num[i]) return num[i]>n.num[i]? 1 : -1;
	return 0;
}

aInteger &aInteger::operator+=(const aInteger &n) {
	if((sign==Minus && n.sign==Plus) || (sign==Plus && n.sign==Minus)) sub(n);
	else add(n);
	return *this;
}

aInteger &aInteger::operator-=(const aInteger &n) {
	if((sign==Minus && n.sign==Plus) || (sign==Plus && n.sign==Minus)) add(n);
	else {
		long c = cmp(n);
// printf("-= c=%ld\n",c);
		if(c==0) sign = Plus,*num = 0ul,len = 1;
		else if(c<0l) {
			aInteger i(*this);
			*this = n;
			sub(i);
			sign = sign==Plus? Minus : Plus;
		} else sub(n);
	}
	return *this;
}

aInteger &aInteger::sqrt() {
	if(isNegative()) sign = NaN;
	else if(len>1 || *num>=2) {
// printf("aInteger::sqrt(n=");print();printf(")\n");
		size_t l2 = 0;
		aInteger u(*this),v,u2,v2,uv2,n1;
		for(u.rshift(1); !u.isZero(); u.rshift(1),l2++);
		l2 >>= 1;
		u.setu32(1);
		u.lshift(l2);
		u2.set(u);
		u2.lshift(l2);
// printf("aInteger::sqrt(l2=%d,u=",l2);u.print();printf(",u2=");u2.print();printf(")\n");
		while(l2--) {
			v.set(1);
			v.lshift(l2);
			v2.set(v);
			v2.lshift(l2);
			uv2.set(u);
			uv2.lshift(l2+1ul);
			n1.set(u2);
			n1 += uv2;
			n1 += v2;
			if(n1<=*this) u += v,u2.set(n1);

// printf("aInteger::sqrt(l2=%d,n=",l2);print();printf(",u=");u.print();printf(",u2=");u2.print();printf(",v=");v.print();
// printf(",v2=");v2.print();printf(",uv2=");uv2.print();printf(",n1=");n1.print();printf(")\n");
		}
		set(u);
	}
	return *this;
}

aInteger &aInteger::gcd(const aInteger &n) {
	aInteger t,b(n);
	while(!b.isZero()) {
		t.set(b);
		div(b,0,&b);
		this->set(t);
	}
	return *this;
}

/*void aInteger::monpro(const aInteger &n,long r) {
	aInteger t(*this),u;
	t.mul(n);

}*/

/*
long long aMath::modpow(long long n,long long e,long long m) {
	long long a = 1,b = n%m;
	while(e) {
		if(e&1) a = (a*b)%m;
		b = (b*b)%m,e >>= 1;
	}
	return a;
}
*/
aInteger &aInteger::modpow(const aInteger &e,const aInteger &m) {
	//if(e.len<=2 || !(*num&1)) {
		aInteger i(e),a(1),b(*this);
		uint32_t e1 = 0;
		int in,en;
// printf("aInteger::modpow(a=");a.print();printf(",b=");b.print();printf(",e=");e.print();printf(",m=");m.print();printf(")\n");
		b.div(m,0,&b);
// printf("b=");b.print();printf("\n");
		for(in=i.len-1; in>=0; in--) {
			for(en=i.len>1? 31 : i.bits(),e1=*i.num; en>=0; en--,e1>>=1) {
				if(e1&1) {
					a.mul(b);
					a.div(m,0,&a);
				}
				b.mul(b);
				b.div(m,0,&b);
// printf("modpow(a=");a.print();printf(",b=");b.print();printf(",e=");i.print();printf(",e1=%lu)\n",e1);
			}
			i.rshift(32);
		}
		set(a);
	/*} else {
		aInteger a(*this),b(1);
		long j = e.bits()-1,i,r = bits();
		a.lshift(r);
		a.div(n,0,&a);
		b.lshift(r);
		b.div(n,0,&b);
		for(i=j; i>=0; i--) {
			b.monpro(b,r);
			if(e.bit(i)) b.monpro(a,r);
		}
		b.rshift(r);
		*this = b;
	}*/
	return *this;
// printf("modpow(a=");a.print();printf(")\n");
}

aInteger &aInteger::random(size_t n) {
	if(n==0) *num = 0,len = 1;
	else {
		size_t i = (n/32)+((n%32)!=0);
		if(i>len) resize(i-len);
		else len = i;
		for(i=0; i<len-1; i++) num[i] = ::rnd.uint32();
		n %= 32;
		if(n==0) n = 32;
		num[len-1] = (n==32? ::rnd.uint32() : ::rnd.uintN(n)) | (0x80000000>>(32-n));
	}
	return *this;
}

size_t aInteger::log2() const {
	size_t i = bits();
	return i>0? i-1 : 0;
}

size_t aInteger::bits() const {
	if(isZero()) return 0;
	size_t i = 31;
	uint32_t n = num[len-1];
	if(n<=0xff) i -= 24;
	else if(n<=0xffff) i -= 16;
	else if(n<=0xffffff) i -= 8;
	while(!(n>>i)) i--;
	if(len>1) i += (len-1)*32;
	return i+1;
}

void aInteger::setBit(size_t n,int i) {
	size_t l = n/32;
	uint32_t b = 1<<(n%32);
	if(l>=len) resize(l-len);
	num[l] |= b;
	if(!i) {
		num[l] ^= b;
		if(l==len-1) while(len>1 && !num[len-1]) len--;
	}
}

/*
*/
bool aInteger::isPrime() {
	//if(len==1) return aMath::isprime((long long)*num);
	if((len==1 && *num<2) || ((len>1 || *num>3) && !(*num&1))) return false;
	if(len==1 && *num<4) return true;
// printf("aInteger::isPrime(");print();printf(")\n");
	aInteger n1(*this),a1;
// printf("1\n");
	n1 -= 1;
	for(int a=0; a<4; a++) {
// printf("a=%d\n",a);
		a1.setu32(aMath::prime[a]);
// printf("a1=");a1.print();printf("\n");
		a1.modpow(n1,*this);
// printf("a1=");a1.print();printf("\n");
		if(a1.len>1 || *a1.num!=1) return false;
	}
	return true;
}

bool aInteger::isSafePrime() {
	if(!isPrime()) return false;
	aInteger n(*this);
	n.rshift(1);
	return n.isPrime();
}

/*aInteger::operator long long() const {
	long long n = *num|((long long)num[1]);
	return sign==Minus? -n : n;
}*/


void aInteger::print(FILE *fp) const {
	if(sign==Minus) fputc('-',fp);
	if(len==2) {
		uint64_t n = (((uint64_t)num[1])<<32)|*num;
		printf("%" PRIu64,n);
	} else if(len==1) printf("%" PRIu32,*num);
	else for(long i=len-1; i>=0l; i--) fprintf(fp,i==(long)len-1? "%" PRIX32 : "%08" PRIX32,num[i]);
	//fprintf(fp,"[%lu,%p:%p]",len*32,this,num);
	fprintf(fp,"[%zu]",len);
	fflush(fp);
}



#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL_endian.h>
#include <libamanita/Math.h>
#include <libamanita/String.h>
#include <libamanita/Random.h>
#include <libamanita/Integer.h>



Integer::Integer() {
	sign = Plus,num = (unsigned long *)malloc(sizeof(long)),len = cap = 1;
	*num = 0ul;
}

Integer::Integer(int n) {
	if(n<0l) sign = Minus,n = -n;
	else sign = Plus;
	num = (unsigned long *)malloc(sizeof(long)),len = cap =1;
	*num = n;
}

Integer::Integer(long n) {
	if(n<0l) sign = Minus,n = -n;
	else sign = Plus;
	num = (unsigned long *)malloc(sizeof(long)),len = cap = 1;
	*num = n;
}

Integer::Integer(unsigned long n) {
	sign = Plus,num = (unsigned long *)malloc(sizeof(long)),len = cap = 1;
	*num = n;
}

Integer::Integer(long long n) {
	if(n<0ll) sign = Minus,n = -n;
	else sign = Plus;
	len = cap = n>0xffffffffll? 2 : 1;
	num = (unsigned long *)malloc(len*sizeof(long));
	*num = n&0xfffffffful;
	if(len==2) num[1] = (n>>32)&0xfffffffful;
}

Integer::Integer(unsigned long long n) {
	len = cap = n>0xffffffffull? 2 : 1;
	sign = Plus,num = (unsigned long *)malloc(len*sizeof(long));
	*num = n&0xfffffffful;
	if(len==2) num[1] = (n>>32)&0xfffffffful;
}

Integer::Integer(const Integer &n) {
	sign = n.sign,num = (unsigned long *)malloc(n.len*sizeof(long)),len = cap = n.len;
	memcpy(num,n.num,len*sizeof(long));
}

Integer::~Integer() {
	free(num);
	sign = Plus,num = 0,len = cap = 0;
}

void Integer::resize(long n) {
	if(n==0) return;
// printf("resize(len=%d,cap=%d,n=%ld)\n",len,cap,n);fflush(stdout);
	len += n;
	if(n>0) {
		if(len>cap) cap = len,num = (unsigned long *)realloc(num,cap*sizeof(long));
// printf("resize(len=%d,cap=%d,n=%ld)\n",len,cap,n);fflush(stdout);
		memset(&num[len-n],0,n*sizeof(long));
	}
// printf("resize\n");fflush(stdout);
}

Integer &Integer::operator=(int n) {
	if(n<0l) sign = Minus,n = -n;
	else sign = Plus;
	*num = n,len = 1;
	return *this;
}

Integer &Integer::operator=(long n) {
	if(n<0l) sign = Minus,n = -n;
	else sign = Plus;
	*num = n,len = 1;
	return *this;
}

Integer &Integer::operator=(unsigned long n) {
	sign = Plus,*num = n,len = 1;
	return *this;
}

Integer &Integer::operator=(long long n) {
	if(n<0ll) sign = Minus,n = -n;
	else sign = Plus;
	len = n>0xffffffffll? 2 : 1;
	if(len>cap) {
		cap = len;
		num = (unsigned long *)realloc(num,cap*sizeof(long));
	}
	*num = n&0xfffffffful;
	if(len==2) num[1] = (n>>32)&0xfffffffful;
	return *this;
}

Integer &Integer::operator=(unsigned long long n) {
	len = n>0xffffffffull? 2 : 1;
	if(len>cap) {
		cap = len;
		num = (unsigned long *)realloc(num,cap*sizeof(long));
	}
	sign = Plus;
	*num = n&0xfffffffful;
	if(len==2) num[1] = (n>>32)&0xfffffffful;
	return *this;
}

Integer &Integer::operator=(const char *n) {
	if(*n=='-') sign = Minus,n++;
	else sign = Plus;
	size_t l = strlen(n),p;
	unsigned long m,c;
	long i = l/8+((l%8)!=0),j;
	resize(i-len);
// printf("Integer(n=%s,l=%d,\"",n,l);
	for(i=l-1,m=0,p=0; i>=0; i-=8) {
		for(j=0; j<8; j+=2) {
			c = 0;
			if(i-j-1>=0) /*{*/c |= String::fromHex(n[i-j-1])<<4;//putchar(n[i-j-1]);}
			if(i-j>=0) /*{*/c |= String::fromHex(n[i-j]);//putchar(n[i-j]);}
			m = (m<<8)|c;
		}
		num[p++] = SDL_SwapBE32(m),m = 0,j = 8;
	}
// printf("\")\n");
	return *this;
}

Integer &Integer::operator=(const Integer &n) {
	if(n.len>cap) {
		cap = n.len;
		num = (unsigned long *)realloc(num,cap*sizeof(long));
	}
	sign = n.sign,len = n.len;
	memcpy(num,n.num,len*sizeof(long));
	return *this;
}

Integer &Integer::operator+=(const Integer &n) {
	if((sign==Minus && n.sign==Plus) || (sign==Plus && n.sign==Minus)) sub(n);
	else add(n);
	return *this;
}

Integer &Integer::operator-=(const Integer &n) {
	if((sign==Minus && n.sign==Plus) || (sign==Plus && n.sign==Minus)) add(n);
	else {
		long c = cmp(n);
// printf("-= c=%ld\n",c);
		if(c==0) sign = Plus,*num = 0ul,len = 1;
		else if(c<0l) {
			Integer i(*this);
			*this = n;
			sub(i);
			sign = sign==Plus? Minus : Plus;
		} else sub(n);
	}
	return *this;
}

void Integer::inc(unsigned long n) {
	for(size_t i=0ul; i<len; i++) {
		if(num[i]+n>num[i]) { num[i] += n;break; }
		if(i==len-1) resize(1);
		num[i] += n,n = 1ul;
	}
// printf("Integer::inc(");print();printf(")\r");
}

void Integer::dec(unsigned long n) {
	for(size_t i=0ul; i<len; i++) {
		if(n<=num[i]) { num[i] -= n;break; }
		if(len==1) sign = sign==Plus? Minus : Plus,*num = n-*num;
		else num[i] -= n,n = 1ul;
	}
}

void Integer::add(const Integer &n) {
// printf("add(");print();printf(" + ");n.print();printf(")\n");fflush(stdout);
// printf("add(len=%d,n.len=%d,sign=%c,n.sign=%c)\n",len,n.len,sign==Plus? '+' : '-',n.sign==Plus? '+' : '-');
	if(n.len>len) resize(n.len-len);
	size_t i = 0ul,t = 0ul;
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

void Integer::sub(const Integer &n) {
// printf("sub(");print();printf(" - ");n.print();printf(")\n");fflush(stdout);
// printf("sub(len=%d,n.len=%d,sign=%c,n.sign=%c,this=",len,n.len,sign==Plus? '+' : '-',n.sign==Plus? '+' : '-');
// print();printf(",n=");n.print();printf(")\n");
	if(n.isZero()) return;
	// if(len<n.len || (len==n.len && num[len-1]<n.num[len-1])) sign = sign==Plus? Minus : Plus;
	for(size_t i=0ul,t=0ul; i<len && (i<n.len || t); i++) {
// printf("sub(i=%d,t=%d,num[i]=%lx,n.num[i]=%lx)\n",i,t,num[i],i<n.len? n.num[i] : 0);
		if(t) { if(num[i]) t = 0;num[i]--; }
		if(i<n.len) { if(num[i]<n.num[i]) t++;num[i] -= n.num[i]; }
	}
	while(len>1 && num[len-1]==0) len--;
	//if(sign==Minus && len==1 && !*num) sign = Plus;
// printf("sub( = ");print();printf(")\n");fflush(stdout);
}

void Integer::mul(const Integer &n) {
// printf("mul(");print();printf(" * ");n.print();printf(")\n");fflush(stdout);
	if(sign==Minus && n.sign==Minus) sign = Plus;
	else if(sign==Plus && n.sign==Minus) sign = Minus;
	if(len==1 && n.len==1 && *num<=0xffff && *n.num<=0xffff) *num *= *n.num;
	else {
		Integer p;
		p.resize(len+n.len);
		p.sign = sign;
		size_t i,j;
		unsigned long long w;
		unsigned long c;
		for(i=0; i<n.len; p.num[i+len]=c,i++) for(j=0,c=0; j<len; j++)
			w = num[j],w *= n.num[i],w += p.num[i+j],w += c,p.num[i+j] = w&0xffffffff,c = w>>32;
		while(p.len>1 && p.num[p.len-1]==0) p.len--;
		*this = p;
		/*Integer t(*this),p,q(n); // The bitshift-multiply has been replaced by a matrix-multiply.
		size_t l,s;
		unsigned long d;
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

void Integer::div(const Integer &n,Integer *q,Integer *r) {
// printf("div(");print();printf(" / ");n.print();printf(")\n");fflush(stdout);
	if(sign==Minus && n.sign==Minus) sign = Plus;
	size_t i = 0,s = 0;
	if(n.isZero()) i = 1/i;
	if(len==1 && n.len==1) {
		if(q) *q = (unsigned long)(*num / *n.num);
		if(r) *r = (unsigned long)(*num % *n.num);
	} else {
		int m = cmp(n);
		if(m==0) {
			if(q) *q = 1;
			if(r) *r = 0;
		} else if(m<0) {
			if(q) *q = 0;
			if(r) *r = *this;
		} else {
			Integer a(*this),b(n),c,d(b);
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
			if(q) *q = c;
			if(r) *r = a;
		}
	}
// printf("div\n");fflush(stdout);
}

void Integer::lshift(size_t n) {
// printf("lshift(n=%d,num=%p)\n",n,num);
	if(n==0) return;
	size_t l = n/32,m = n%32;
	if(l || (m && num[len-1]>=(1ul<<(32-m)))) resize(l+(m && num[len-1]>=(1ul<<(32-m))));
	for(long i=len-l-1; i>=0; i--) {
// printf("lshift(l=%d,i=%ld)\n",l,i);
		num[i+l] = (num[i]<<m)|(m && i>0? num[i-1]>>(32-m) : 0ul);
	}
	if(l) memset(num,0,l*sizeof(long));
}

void Integer::rshift(size_t n) {
// printf("rshift(n=%d,num=%p)\n",n,num);
	if(n==0) return;
	size_t l = n/32,m = n%32;
	if(m) { // For shifting 1-31 bit part of 32 bit integers (n%32>0).
		if(l>len || (l==len && num[len-1]<(0xfffffffful>>m))) sign = Plus,*num = 0ul,len = 1;
		else {
			for(size_t i=l; i<len; i++) num[i-l] = (i<len-1? num[i+1]<<(32-m) : 0ul)|(num[i]>>m);
			len -= l;
			while(len>1 && !num[len-1]) len--;
		}
	} else { // For shifting whole 32 bit integers (n%32==0).
		if(l>len) sign = Plus,*num = 0ul,len = 1;
		else {
			for(size_t i=l; i<len; i++) num[i-l] = num[i];
			len -= l;
		}
	}
}

int Integer::cmp(long n) {
	if(sign==Minus) {
		if(n>=0l || len>1) return -1;
		n = -n;
		if(*num!=(unsigned long)n) return *num>(unsigned long)n? -1 : 1;
	} else {
		if(n<0l || len>1) return 1;
		if(*num!=(unsigned long)n) return *num>(unsigned long)n? 1 : -1;
	}
	return 0;
}

int Integer::cmp(unsigned long n) {
	if(len>1) return 1;
	if(*num!=n) return *num>n? 1 : -1;
	return 0;
}

int Integer::cmp(long long n) {
	if(sign==Minus) {
		if(n>=0ll || len>2) return -1;
		n = -n;
		if(num[1]!=(unsigned long)(n>>32)) return num[1]>(unsigned long)(n>>32)? -1 : 1;
		if(*num!=(n&0xfffffffful)) return *num>(n&0xfffffffful)? -1 : 1;
	} else {
		if(n<0ll || len>2) return 1;
		if(len<2) return -1;
		if(num[1]!=(unsigned long)(n>>32)) return num[1]>(unsigned long)(n>>32)? 1 : -1;
		if(*num!=(n&0xfffffffful)) return *num>(n&0xfffffffful)? 1 : -1;
	}
	return 0;
}

int Integer::cmp(unsigned long long n) {
	if(len>2) return 1;
	if(len<2) return -1;
	if(num[1]!=(n>>32)) return num[1]>(n>>32)? 1 : -1;
	if(*num!=(n&0xfffffffful)) return *num>(n&0xfffffffful)? 1 : -1;
	return 0;
}

int Integer::cmp(const Integer &n) {
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

Integer &Integer::sqrt() {
	if(isNegative()) sign = NaN;
	else if(len>1 || *num>=2ul) {
// printf("Integer::sqrt(n=");print();printf(")\n");
		size_t l2 = 0ul;
		Integer u(*this),v,u2,v2,uv2,n1;
		for(u.rshift(1); !u.isZero(); u.rshift(1),l2++);
		l2 >>= 1ul;
		u = 1ul;
		u.lshift(l2);
		u2 = u;
		u2.lshift(l2);
// printf("Integer::sqrt(l2=%d,u=",l2);u.print();printf(",u2=");u2.print();printf(")\n");
		while(l2--) {
			v = 1ul;
			v.lshift(l2);
			v2 = v;
			v2.lshift(l2);
			uv2 = u;
			uv2.lshift(l2+1ul);
			n1 = u2;
			n1 += uv2;
			n1 += v2;
			if(n1<=*this) u += v,u2 = n1;

// printf("Integer::sqrt(l2=%d,n=",l2);print();printf(",u=");u.print();printf(",u2=");u2.print();printf(",v=");v.print();
// printf(",v2=");v2.print();printf(",uv2=");uv2.print();printf(",n1=");n1.print();printf(")\n");
		}
		*this = u;
	}
	return *this;
}

Integer &Integer::gcd(const Integer &n) {
	Integer t,b(n);
	while(!b.isZero()) {
		t = b;
		div(b,0,&b);
		*this = t;
	}
	return *this;
}

/*void Integer::monpro(const Integer &n,long r) {
	Integer t(*this),u;
	t.mul(n);

}*/

/*
long long Math::modpow(long long n,long long e,long long m) {
	long long a = 1,b = n%m;
	while(e) {
		if(e&1) a = (a*b)%m;
		b = (b*b)%m,e >>= 1;
	}
	return a;
}
*/
Integer &Integer::modpow(const Integer &e,const Integer &m) {
	//if(e.len<=2 || !(*num&1)) {
		Integer i(e),a(1),b(*this);
		unsigned long e1 = 0;
		int in,en;
// printf("Integer::modpow(a=");a.print();printf(",b=");b.print();printf(",e=");e.print();printf(",m=");m.print();printf(")\n");
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
		*this = a;
	/*} else {
		Integer a(*this),b(1);
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

Integer &Integer::random(size_t n) {
	if(n==0) *num = 0,len = 1;
	else {
		size_t i = (n/32)+((n%32)!=0);
		if(i>len) resize(i-len);
		else len = i;
		for(i=0; i<len-1; i++) num[i] = ::rnd.getUint32();
		num[len-1] = ::rnd.getUintN((n%32)==0? 32 : n%32);
	}
	return *this;
}

size_t Integer::log2() const {
	size_t i = bits();
	return i>0? i-1 : 0;
}

size_t Integer::bits() const {
	if(isZero()) return 0;
	size_t i = 31;
	unsigned long n = num[len-1];
	if(n<=0xff) i -= 24;
	else if(n<=0xffff) i -= 16;
	else if(n<=0xffffff) i -= 8;
	while(!(n>>i)) i--;
	if(len>1) i += (len-1)*32;
	return i+1;
}

void Integer::setBit(size_t n,int i) {
	size_t l = n/32;
	unsigned long b = 1<<(n%32);
	if(l>=len) resize(l-len);
	num[l] |= b;
	if(!i) {
		num[l] ^= b;
		if(l==len-1) while(len>1 && !num[len-1]) len--;
	}
}

/*
*/
bool Integer::isPrime() {
	//if(len==1) return Math::isprime((long long)*num);
	if((len==1 && *num<2) || ((len>1 || *num>3) && !(*num&1))) return false;
	if(len==1 && *num<4) return true;
// printf("Integer::isPrime(");print();printf(")\n");
	Integer n1(*this),a1;
// printf("1\n");
	n1 -= 1;
	for(int a=0; a<4; a++) {
// printf("a=%d\n",a);
		a1 = Math::prime[a];
// printf("a1=");a1.print();printf("\n");
		a1.modpow(n1,*this);
// printf("a1=");a1.print();printf("\n");
		if(a1.len>1 || *a1.num!=1) return false;
	}
	return true;
}

bool Integer::isSafePrime() {
	if(!isPrime()) return false;
	Integer n = *this;
	n >>= 1;
	return n.isPrime();
}

Integer::operator long long() const {
	long long n = *num|((long long)num[1]);
	return sign==Minus? -n : n;
}


void Integer::print(FILE *fp) const {
	if(sign==Minus) fputc('-',fp);
	if(len==2) {
		unsigned long long n = (unsigned long long)num[1]<<32;
		n |= *num;
#if defined(__linux__)
		printf("%llu",n);
#else
		printf("%I64u",n);
#endif
	} else if(len==1) printf("%lu",*num);
	else for(long i=len-1; i>=0l; i--) fprintf(fp,i==(long)len-1? "%lX" : "%08lX",num[i]);
	//fprintf(fp,"[%lu,%p:%p]",len*32,this,num);
	fprintf(fp,"[%u]",len);
	fflush(fp);
}


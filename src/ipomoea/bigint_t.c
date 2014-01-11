
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <ipomoea/bigint_t.h>
#include <ipomoea/string_t.h>


/*
typedef struct bigint_t {
	uint8_t sign;						//!< Sign tells what type of number this is.
	uint32_t *num;						//!< Num contains all data of this.
	size_t len;							//!< Len is the length of the number this is set to in number of 32 bit integers, it is always >= 1.
	size_t cap;							//!< Cap is the capacity of num, it is always >= 1, and len is always <= cap.
} bigint_t;
*/


bigint_t *bi_new() {
	bigint_t *bi = (bigint_t *)malloc(sizeof(bigint_t));
	bi->sign = Plus,bi->num = 0,bi->len = bi->cap = 0;
}

void bi_delete(bigint_t *bi) {
	if(bi) {
		if(bi->num) free(bi->num);
		free(bi);
	}
}

bigint_t *bi_resize(bigint_t *bi,int n) {
	if(!bi) bi = bi_new();
	if(n>0) {
//printf("bi_resize(len=%d,cap=%d,n=%d)\n",(int)bi->len,(int)bi->cap,n);fflush(stdout);
		bi->len += n;
		if(n>0) {
			if(bi->len>bi->cap) {
				bi->cap = bi->len;
				if(bi->num) bi->num = (uint32_t *)realloc(bi->num,bi->cap*4);
				else bi->num = (uint32_t *)malloc(bi->cap*4);
			}
//printf("bi_resize(len=%d,cap=%d,n=%d)\n",(int)bi->len,(int)bi->cap,n);fflush(stdout);
			memset(&bi->num[bi->len-n],0,n*4);
		}
	}
//printf("bi_resize\n");fflush(stdout);
	return bi;
}

bigint_t *bi_seti32(bigint_t *bi,int32_t n) {
	if(!bi) bi = bi_new();
	if(n<0) bi->sign = Minus,n = -n;
	else bi->sign = Plus;
	bi->len = 1;
	if(!bi->num) bi->num = (uint32_t *)malloc(4),bi->cap = 1;
	*bi->num = n;
	return bi;
}

bigint_t *bi_setu32(bigint_t *bi,uint32_t n) {
	if(!bi) bi = bi_new();
	bi->sign = Plus;
	bi->len = 1;
	if(!bi->num) bi->num = (uint32_t *)malloc(4),bi->cap = 1;
	*bi->num = n;
	return bi;
}

bigint_t *bi_seti64(bigint_t *bi,int64_t n) {
	if(!bi) bi = bi_new();
	if(n<0) bi->sign = Minus,n = -n;
	else bi->sign = Plus;
	bi->len = n>0xffffffffll? 2 : 1;
	if(!bi->num) bi->num = (uint32_t *)malloc(bi->len*4),bi->cap = bi->len;
	else if(bi->len>bi->cap) {
		bi->cap = bi->len;
		bi->num = (uint32_t *)realloc(bi->num,bi->cap*4);
	}
	*bi->num = n&0xffffffff;
	if(bi->len==2) bi->num[1] = (n>>32)&0xffffffff;
	return bi;
}

bigint_t *bi_setu64(bigint_t *bi,uint64_t n) {
	if(!bi) bi = bi_new();
	bi->sign = Plus;
	bi->len = n>0xffffffffull? 2 : 1;
	if(!bi->num) bi->num = (uint32_t *)malloc(bi->len*4),bi->cap = bi->len;
	else if(bi->len>bi->cap) {
		bi->cap = bi->len;
		bi->num = (uint32_t *)realloc(bi->num,bi->cap*4);
	}
	*bi->num = n&0xffffffff;
	if(bi->len==2) bi->num[1] = (n>>32)&0xffffffff;
	return bi;
}

bigint_t *bi_setstr(bigint_t *bi,const char *n) {
	if(!bi) bi = bi_new();
	if(*n=='-') bi->sign = Minus,n++;
	else bi->sign = Plus;
	{
		size_t l = strlen(n),p;
		uint32_t m,c;
		int i = l/8+((l%8)!=0),j;
		if(!bi->num) bi->num = (uint32_t *)malloc(i*4),bi->len = bi->cap = i;
		else bi_resize(bi,i-bi->len);
//printf("Integer(n=%s,l=%d,\"",n,(int)l);
		for(i=l-1,m=0,p=0; i>=0; i-=8) {
			for(j=0; j<8; j+=2) {
				c = 0;
				if(i-j-1>=0) /*{*/c |= xtoi(n[i-j-1])<<4;//putchar(n[i-j-1]);}
				if(i-j>=0) /*{*/c |= xtoi(n[i-j]);//putchar(n[i-j]);}
#if __BYTE_ORDER == __BIG_ENDIAN
				m = (m<<8)|c;
#else
				m = (c<<24)|(m>>8);
#endif
			}
			bi->num[p++] = m,m = 0,j = 8;
		}
//printf("\")\n");
	}
	return bi;
}

bigint_t *bi_set(bigint_t *bi,const bigint_t *n) {
	if(!bi) bi = bi_new();
	bi->sign = n->sign,bi->len = n->len;
	if(!bi->num) bi->num = (uint32_t *)malloc(n->len*4),bi->cap = n->len;
	else if(n->len>bi->cap) {
		bi->cap = n->len;
		bi->num = (uint32_t *)realloc(bi->num,bi->cap*4);
	}
	memcpy(bi->num,n->num,bi->len*4);
	return bi;
}

void bi_inc(bigint_t *bi,uint32_t n) {
	size_t i;
	for(i=0; i<bi->len; i++) {
		if(bi->num[i]+n>bi->num[i]) { bi->num[i] += n;break; }
		if(i==bi->len-1) bi_resize(bi,1);
		bi->num[i] += n,n = 1;
	}
//printf("Integer::inc(");bi_print(bi);printf(")\r");
}

void bi_dec(bigint_t *bi,uint32_t n) {
	size_t i;
	for(i=0; i<bi->len; i++) {
		if(n<=bi->num[i]) { bi->num[i] -= n;break; }
		if(bi->len==1) bi->sign = bi->sign==Plus? Minus : Plus,*bi->num = n-*bi->num;
		else bi->num[i] -= n,n = 1;
	}
}

static void __add(bigint_t *bi,const bigint_t *n) {
//printf("add(");bi_print(bi);printf(" + ");bi_print(n);printf(")\n");fflush(stdout);
//printf("add(len=%d,n.len=%d,sign=%c,n.sign=%c)\n",(int)bi->len,(int)n->len,bi->sign==Plus? '+' : '-',n->sign==Plus? '+' : '-');
	if(n->len>bi->len) bi_resize(bi,n->len-bi->len);
	size_t i = 0,t = 0;
	for(; i<bi->len && (i<n->len || t); i++) {
//printf("add(i=%d,t=%d)\n",(int)i,(int)t);
		if(i<n->len) {
			if(n->num[i] && bi->num[i]+t+n->num[i]<=bi->num[i]) bi->num[i] += t+n->num[i],t = 1;
			else bi->num[i] += t+n->num[i],t = 0;
		} else bi->num[i] += t,t = 0;
	}
//printf("add(t=%d)\n",(int)t);
	if(t) { bi_resize(bi,1);bi->num[bi->len-1] = t; }
//printf("add( = ");bi_print(bi);printf(")\n");fflush(stdout);
}

static void __sub(bigint_t *bi,const bigint_t *n) {
	size_t i,t;
//printf("sub(");bi_print(bi);printf(" - ");bi_print(n);printf(")\n");fflush(stdout);
//printf("sub(len=%d,n->len=%d,sign=%c,n->sign=%c,this=",(int)bi->len,(int)n->len,bi->sign==Plus? '+' : '-',n->sign==Plus? '+' : '-');
//bi_print(bi);printf(",n=");bi_print(n);printf(")\n");
	if(bi_is_zero(n)) return;
	for(i=0,t=0; i<bi->len && (i<n->len || t); i++) {
//printf("sub(i=%d,t=%d,num[i]=%x,n->num[i]=%x)\n",(int)i,(int)t,(int)bi->num[i],i<n->len? (int)n->num[i] : 0);
		if(t) { if(bi->num[i]) t = 0;bi->num[i]--; }
		if(i<n->len) { if(bi->num[i]<n->num[i]) t++;bi->num[i] -= n->num[i]; }
	}
	while(bi->len>1 && bi->num[bi->len-1]==0) bi->len--;
//printf("sub( = ");bi_print(bi);printf(")\n");fflush(stdout);
}

void bi_add(bigint_t *bi,const bigint_t *n) {
	if((bi->sign==Minus && n->sign==Plus) || (bi->sign==Plus && n->sign==Minus)) __sub(bi,n);
	else __add(bi,n);
}

void bi_sub(bigint_t *bi,const bigint_t *n) {
	if((bi->sign==Minus && n->sign==Plus) || (bi->sign==Plus && n->sign==Minus)) __add(bi,n);
	else {
		long c = bi_cmp(bi,n);
// printf("-= c=%ld\n",c);
		if(c==0) bi->sign = Plus,*bi->num = 0,bi->len = 1;
		else if(c<0) {
			bigint_t *i = bi_new();
			bi_set(i,bi);
			bi_set(bi,n);
			__sub(bi,i);
			bi->sign = bi->sign==Plus? Minus : Plus;
			bi_delete(i);
		} else __sub(bi,n);
	}
}

void bi_mul(bigint_t *bi,const bigint_t *n) {
//printf("mul(");bi_print(bi);printf(" * ");bi_print(n);printf(")\n");fflush(stdout);
	if(bi->sign==Minus && n->sign==Minus) bi->sign = Plus;
	else if(bi->sign==Plus && n->sign==Minus) bi->sign = Minus;
	if(bi->len==1 && n->len==1 && *bi->num<=0xffff && *n->num<=0xffff) *bi->num *= *n->num;
	else {
		size_t i,j;
		uint64_t w;
		uint32_t c;
		bigint_t *p = bi_new();
		bi_resize(p,bi->len+n->len);
		p->sign = bi->sign;
		for(i=0; i<n->len; p->num[i+bi->len]=c,i++) for(j=0,c=0; j<bi->len; j++)
			w = bi->num[j],w *= n->num[i],w += p->num[i+j],w += c,p->num[i+j] = w&0xffffffff,c = w>>32;
		while(p->len>1 && p->num[p->len-1]==0) p->len--;
		bi_set(bi,p);
		bi_delete(p);
	}
//printf("mul( = ");bi_print(bi);printf(")\n");fflush(stdout);
}

void bi_div(bigint_t *bi,const bigint_t *n,bigint_t *q,bigint_t *r) {
// printf("div(");bi_print(bi);printf(" / ");bi_print(n);printf(")\n");fflush(stdout);
	if(bi->sign==Minus && n->sign==Minus) bi->sign = Plus;
	size_t i = 0,s = 0;
	if(bi_is_zero(n)) i = 1/i;
	if(bi->len==1 && n->len==1) {
		if(q) bi_setu32(q,*bi->num / *n->num);
		if(r) bi_setu32(r,*bi->num % *n->num);
	} else {
		int m = bi_cmp(bi,n);
		if(m==0) {
			if(q) bi_setu32(q,1);
			if(r) bi_setu32(r,0);
		} else if(m<0) {
			if(q) bi_setu32(q,0);
			if(r) bi_set(r,bi);
		} else {
			bigint_t *a = bi_new(),*b = bi_new(),*c = bi_new();
			bi_set(a,bi);
			bi_set(b,n);
			bi_seti32(c,0);
// printf("div1(a=");bi_print(a);printf(",b=");bi_print(b);printf(",c=");bi_print(c);printf(")\n");
			while(bi_cmp(b,a)<0) { bi_lshift(b,1);s++; }
// printf("div2(a=");bi_print(a);printf(",b=");bi_print(b);printf(",c=");bi_print(c);printf(")\n");
			if(bi_cmp(b,a)>0) { bi_rshift(b,1);s--; }
// printf("div3(a=");bi_print(a);printf(",b=");bi_print(b);printf(",c=");bi_print(c);printf(")\n");
			for(i=0; i<=s; i++) {
				m = bi_cmp(b,a);
// printf("div4(cmp=%d,a=",m);bi_print(a);printf(",b=");bi_print(b);printf(",c=");bi_print(c);printf(")\n");
				if(m<=0) { bi_sub(a,b);bi_rshift(b,1);bi_lshift(c,1);*c->num |= 1; }
				else { bi_rshift(b,1);bi_lshift(c,1); }
			}
// printf("div( = ");bi_print(c);printf(" mod=");bi_print(a);printf(")\n");fflush(stdout);
			if(q) bi_set(q,c);
			if(r) bi_set(r,a);
			bi_delete(a);
			bi_delete(b);
			bi_delete(c);
		}
	}
// printf("div\n");fflush(stdout);
}


void bi_lshift(bigint_t *bi,size_t n) {
// printf("lshift(n=%d,num=%p)\n",n,num);
	if(n==0) return;
	long i;
	size_t l = n/32,m = n%32;
	if(l || (m && bi->num[bi->len-1]>=(1u<<(32-m)))) bi_resize(bi,l+(m && bi->num[bi->len-1]>=(1u<<(32-m))));
	for(i=bi->len-l-1; i>=0; i--) {
// printf("lshift(l=%d,i=%ld)\n",l,i);
		bi->num[i+l] = (bi->num[i]<<m)|(m && i>0? bi->num[i-1]>>(32-m) : 0);
	}
	if(l) memset(bi->num,0,l*4);
}

void bi_rshift(bigint_t *bi,size_t n) {
// printf("rshift(n=%d,num=%p)\n",n,num);
	if(n==0) return;
	size_t i,l = n/32,m = n%32;
	if(m) { // For shifting 1-31 bit part of 32 bit integers (n%32>0).
		if(l>bi->len || (l==bi->len && bi->num[bi->len-1]<(0xffffffff>>m))) bi->sign = Plus,*bi->num = 0,bi->len = 1;
		else {
			for(i=l; i<bi->len; i++) bi->num[i-l] = (i<bi->len-1? bi->num[i+1]<<(32-m) : 0)|(bi->num[i]>>m);
			bi->len -= l;
			while(bi->len>1 && !bi->num[bi->len-1]) bi->len--;
		}
	} else { // For shifting whole 32 bit integers (n%32==0).
		if(l>bi->len) bi->sign = Plus,*bi->num = 0,bi->len = 1;
		else {
			for(i=l; i<bi->len; i++) bi->num[i-l] = bi->num[i];
			bi->len -= l;
		}
	}
}

int bi_cmpi32(const bigint_t *bi,int32_t n) {
	if(bi->sign==Minus) {
		if(n>=0 || bi->len>1) return -1;
		n = -n;
		if(*bi->num!=(uint32_t)n) return *bi->num>(uint32_t)n? -1 : 1;
	} else {
		if(n<0 || bi->len>1) return 1;
		if(*bi->num!=(uint32_t)n) return *bi->num>(uint32_t)n? 1 : -1;
	}
	return 0;
}

int bi_cmpu32(const bigint_t *bi,uint32_t n) {
	if(bi->len>1) return 1;
	if(*bi->num!=n) return *bi->num>n? 1 : -1;
	return 0;
}

int bi_cmpi64(const bigint_t *bi,int64_t n) {
	if(bi->sign==Minus) {
		if(n>=0 || bi->len>2) return -1;
		n = -n;
		if(bi->num[1]!=(uint32_t)(n>>32)) return bi->num[1]>(uint32_t)(n>>32)? -1 : 1;
		if(*bi->num!=(n&0xffffffff)) return *bi->num>(n&0xffffffff)? -1 : 1;
	} else {
		if(n<0 || bi->len>2) return 1;
		if(bi->len<2) return -1;
		if(bi->num[1]!=(uint32_t)(n>>32)) return bi->num[1]>(uint32_t)(n>>32)? 1 : -1;
		if(*bi->num!=(n&0xffffffff)) return *bi->num>(n&0xffffffff)? 1 : -1;
	}
	return 0;
}

int bi_cmpu64(const bigint_t *bi,uint64_t n) {
	if(bi->len>2) return 1;
	if(bi->len<2) return -1;
	if(bi->num[1]!=(n>>32)) return bi->num[1]>(n>>32)? 1 : -1;
	if(*bi->num!=(n&0xffffffff)) return *bi->num>(n&0xffffffff)? 1 : -1;
	return 0;
}

int bi_cmp(const bigint_t *bi,const bigint_t *n) {
	long i;
// printf("cmp(");bi_print(bi);printf(",n=");bi_print(n);printf(")\n");
	if(bi->sign==Minus) {
		if(n->sign==Plus) return -1;
		if(bi->len!=n->len) return bi->len>n->len? -1 : 1;
	} else {
		if(n->sign==Minus) return 1;
		if(bi->len!=n->len) return bi->len>n->len? 1 : -1;
	}
	for(i=bi->len-1; i>=0; i--) if(bi->num[i]!=n->num[i]) return bi->num[i]>n->num[i]? 1 : -1;
	return 0;
}

void bi_abs(bigint_t *bi) {
	if(bi->sign==Minus) bi->sign = Plus;
}

void bi_sqrt(bigint_t *bi) {
	if(bi_is_negative(bi)) bi->sign = NaN;
	else if(bi->len>1 || *bi->num>=2) {
// printf("Integer::sqrt(n=");bi_print(bi);printf(")\n");
		size_t l2 = 0;
		bigint_t *u = bi_new(),*v = bi_new(),*u2 = bi_new(),*v2 = bi_new(),*uv2 = bi_new(),*n1 = bi_new();
		bi_set(u,bi);
		bi_rshift(u,1);	
		while(!bi_is_zero(u)) { bi_rshift(u,1);l2++; }
		l2 >>= 1;
		bi_setu32(u,1);
		bi_lshift(u,l2);
		bi_set(u2,u);
		bi_lshift(u2,l2);
// printf("Integer::sqrt(l2=%d,u=",l2);bi_print(u);printf(",u2=");bi_print(u2);printf(")\n");
		while(l2--) {
			bi_setu32(v,1);
			bi_lshift(v,l2);
			bi_set(v2,v);
			bi_lshift(v2,l2);
			bi_set(uv2,u);
			bi_lshift(uv2,l2+1ul);
			bi_set(n1,u2);
			bi_add(n1,uv2);
			bi_add(n1,v2);
			if(bi_cmp(n1,bi)<=0) {
				bi_add(u,v);
				bi_set(u2,n1);
			}

// printf("Integer::sqrt(l2=%d,n=",l2);bi_print(bi);printf(",u=");bi_print(u);printf(",u2=");bi_print(u2);printf(",v=");bi_print(v);
// printf(",v2=");bi_print(v2);printf(",uv2=");bi_print(uv2);printf(",n1=");bi_print(n1);printf(")\n");
		}
		bi_set(bi,u);
		bi_delete(u);
		bi_delete(v);
		bi_delete(u2);
		bi_delete(v2);
		bi_delete(uv2);
		bi_delete(n1);
	}
}

//bigint_t *bi_gcd(bigint_t *bi,const bigint_t *n);
//bigint_t *bi_modpow(bigint_t *bi,const bigint_t *e,const bigint_t *m);
//bigint_t *bi_factor(bigint_t *bi);
//bigint_t *bi_random(size_t n);
//size_t bi_log2(bigint_t *bi);
//size_t bi_bits(bigint_t *bi);


//int bi_bit(bigint_t *bi,size_t n) { return n>=0 && n/32<len? ((num[n/32]>>(n%32))&1) : 0; }

//void bi_setbit(bigint_t *bi,size_t n,int i);

int bi_is_zero(const bigint_t *bi) { return bi->len==1 && !*bi->num; }
int bi_is_negative(const bigint_t *bi) { return bi->sign==Minus; }
int bi_is_nan(const bigint_t *bi) { return bi->sign==NaN; }
int bi_is_infinite(const bigint_t *bi) { return bi->sign==Infinite; }
//int bi_is_prime(const bigint_t *bi);
//int bi_is_safe_prime(const bigint_t *bi);
int bi_is_even(const bigint_t *bi) { return (*bi->num&1)==0; }
int bi_is_odd(const bigint_t *bi) { return (*bi->num&1)==1; }

void bi_print(const bigint_t *bi) {
	if(bi->sign==Minus) fputc('-',stdout);
	if(bi->len==2) {
		uint64_t n = (((uint64_t)bi->num[1])<<32)|*bi->num;
		fprintf(stdout,"%" PRIu64,n);
	} else if(bi->len==1) fprintf(stdout,"%" PRIu32,*bi->num);
	else {
		long i;
		for(i=bi->len-1; i>=0; i--) fprintf(stdout,i==(long)bi->len-1? "%" PRIX32 : "%08" PRIX32,bi->num[i]);
	}
	//fprintf(fp,"[%lu,%p:%p]",len*32,this,num);
	//fprintf(fp,"[%lu]",(unsigned long)len);
	fflush(stdout);
}


/*
Integer &Integer::gcd(const Integer &n) {
	Integer t,b(n);
	while(!b.isZero()) {
		t.set(b);
		div(b,0,&b);
		this->set(t);
	}
	return *this;
}
*/

/*
void Integer::monpro(const Integer &n,long r) {
	Integer t(*this),u;
	t.mul(n);

}
*/

/*
long long a::modpow(long long n,long long e,long long m) {
	long long a = 1,b = n%m;
	while(e) {
		if(e&1) a = (a*b)%m;
		b = (b*b)%m,e >>= 1;
	}
	return a;
}
*/

/*
Integer &Integer::modpow(const Integer &e,const Integer &m) {
		Integer i(e),a(1),b(*this);
		uint32_t e1 = 0;
		int in,en;
// printf("Integer::modpow(a=");bi_print(a);printf(",b=");bi_print(b);printf(",e=");bi_print(e);printf(",m=");bi_print(m);printf(")\n");
		b.div(m,0,&b);
// printf("b=");bi_print(b);printf("\n");
		for(in=i.len-1; in>=0; in--) {
			for(en=i.len>1? 31 : i.bits(),e1=*i.num; en>=0; en--,e1>>=1) {
				if(e1&1) {
					a.mul(b);
					a.div(m,0,&a);
				}
				b.mul(b);
				b.div(m,0,&b);
// printf("modpow(a=");bi_print(a);printf(",b=");bi_print(b);printf(",e=");bi_print(i);printf(",e1=%lu)\n",e1);
			}
			i.rshift(32);
		}
		set(a);
	return *this;
// printf("modpow(a=");bi_print(a);printf(")\n");
}
*/

/*
Integer &Integer::random(size_t n) {
	if(n==0) *num = 0,len = 1;
	else {
		size_t i = (n/32)+((n%32)!=0);
		if(i>len) bi_resize(bi,i-len);
		else len = i;
		for(i=0; i<len-1; i++) num[i] = a::rnd.uint32();
		n %= 32;
		if(n==0) n = 32;
		num[len-1] = (n==32? a::rnd.uint32() : a::rnd.uintN(n)) | (0x80000000>>(32-n));
	}
	return *this;
}
*/

/*
size_t Integer::log2() const {
	size_t i = bits();
	return i>0? i-1 : 0;
}
*/

/*
size_t Integer::bits() const {
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
*/

/*
void Integer::setBit(size_t n,int i) {
	size_t l = n/32;
	uint32_t b = 1<<(n%32);
	if(l>=len) bi_resize(bi,l-len);
	num[l] |= b;
	if(!i) {
		num[l] ^= b;
		if(l==len-1) while(len>1 && !num[len-1]) len--;
	}
}
*/

/*
bool Integer::isPrime() {
	//if(len==1) return a::isprime((long long)*num);
	if((len==1 && *num<2) || ((len>1 || *num>3) && !(*num&1))) return false;
	if(len==1 && *num<4) return true;
// printf("Integer::isPrime(");bi_print(bi);printf(")\n");
	Integer n1(*this),a1;
// printf("1\n");
	n1 -= 1;
	for(int a=0; a<4; a++) {
// printf("a=%d\n",a);
		a1.setu32(a::prime[a]);
// printf("a1=");bi_print(a1);printf("\n");
		a1.modpow(n1,*this);
// printf("a1=");bi_print(a1);printf("\n");
		if(a1.len>1 || *a1.num!=1) return false;
	}
	return true;
}

bool Integer::isSafePrime() {
	if(!isPrime()) return false;
	Integer n(*this);
	n->rshift(1);
	return n->isPrime();
}
*/





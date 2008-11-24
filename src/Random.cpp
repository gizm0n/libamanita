
#include "config.h"
#include <time.h>
#include <libamanita/Math.h>
#include <libamanita/Random.h>
#include <libamanita/Integer.h>



const double n2p32 = 1.0/4294967295.0;
Random rnd = Random();


Random::Random(seed_t n) { setSeed(n); }
Random::Random() { setSeed(time(0)); }

unsigned long Random::generate() { return (num=1812433253ul*((num^(num>>30ul))+(index++))); }

void Random::setSeed(seed_t n) {
	seed = n,num = n,index = 1;
	for(int i=0; i<256; i++) {
		table[i] = generate();
// printf("table[%3d] = %8lx\n",i,table[i]);
	}
	index = 0;
}

void Random::setSeed(seed_t *n,int l) {
	seed = 0,num = 0,index = 1;
	for(int i=0,j=0; i<256; i++,j++) {
		if(j==l) j = 0;
		num ^= n[j];
		table[i] = generate();
// printf("n[%3d] = %8lx\ttable[%3d] = %8lx\n",j,n[j],i,table[i]);
	}
	index = 0;
}

void Random::setSeed(Integer &n) {
	setSeed((seed_t *)((unsigned long *)n),(int)n.length());
}

unsigned long Random::getUint32() { return (num=(table[(index++)&0xff]^=(table[num&0xff]+num))); }
unsigned long long Random::getUint64() {
	unsigned long long n = (unsigned long long)getUint32();
	n = (n<<32)|(unsigned long long)getUint32();
	return n;
}
double Random::getDouble() { return (double)getUint32()*n2p32; }

unsigned long Random::getUintN(int n) {
	if(n<=0 || n>32) return 0ul;
	int j;
	unsigned long r = getUint32(),i,m = 0xffffffff>>(32-n);
	for(j=32,i=0x80000000; i && !(r&i); j--,i>>=1);
// printf("Random::getUintN(r=%lx,i=%lx,m=%lx,j=%d)\n",r,i,m,j);
	if(j>=n) return r>>(j-n);
	r <<= n-j,m >>= n-(n-j);
	return r|(getUint32()&m);
}

int Random::roll(int d,int n) {
	if(d<=1) return d*n;
	int v = 0;
	while(n--) v += getUint32(d)+1;
	return v;
}

int Random::oe(int d,int n) {
	if(d<=1) return d*n;
	int v = 0,i;
	while(n--)
		// One version of this algorithm discards each value of [d] and adds 2d[d]
		// if((i=roll(d))==d) n += 2;

		// The prefered version though, instead of [d], adds [d-1]+1d[d].
		// This saves number of rolls and generate a more even result.
		if((i=roll(d))==d) v += d-1,n++;
		else v += i;
	return v;
}

int Random::oeD4(int n) {
	unsigned long v = 0,i= 0,c = 10;
	while(n-->0) {
		if(c==10) i = getUint32(),c = 1;
		else i >>= 3,c++;
		// One version of this algorithm discards each value of 4 and adds 2d4
		// if((i&3)==3) n += 2;

		// The prefered version though, instead of 4, adds 3+1d4.
		// This saves number of rolls and generate a more even result.
		if((i&3)==3) v += 3,n++;
		else v += (i&3)+1;
	}
	while(n-->0) {
		if(c==10) i = getUint32(),c = 1;
		else i >>= 3,c++;
		if((i&3)==3) n += 2;
		else v += (i&3)+1;
	}
	return v;
}

int Random::oeD8(int n) {
	int v = 0,i = 0,c = 8;
	while(n--) {
		if(c==8) i = getUint32(),c = 1;
		else i >>= 4,c++;
		// One version of this algorithm discards each value of 8 and adds 2d8
		// if((i&7)==7) n += 2;

		// The prefered version though, instead of 8, adds 7+1d8.
		// This saves number of rolls and generate a more even result.
		if((i&7)==7) v += 7,n++;
		else v += (i&7)+1;
	}
	return v;
}

unsigned long Random::rollTable(unsigned long *t,size_t l,unsigned long s) {
	if(!t || !l) return 0ul;
	unsigned long i = 0ul;
	if(!s) s = Math::sum(t,l);
	s = getUint32(s);
	while(s>=t[i]) s -= t[i++];
	return i;
}




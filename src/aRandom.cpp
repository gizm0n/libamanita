
#include "config.h"
#include <time.h>
#include <libamanita/aMath.h>
#include <libamanita/aRandom.h>


#define GENERATE_NUMBER (num=(1812433253*((num^(num>>30))+(index++))))
#define RANDOM_NUMBER (num=(table[(index++)&0xff]^=(table[num&0xff]+num)))

const double n2p32 = 1.0/4294967295.0;
aRandom rnd = aRandom();


aRandom::aRandom() { setSeed(time(0)); }
aRandom::aRandom(random_t n) { setSeed(n); }


void aRandom::setSeed(random_t n) {
	seed = n,num = n,index = 1;
	for(int i=0; i<256; i++) {
		table[i] = GENERATE_NUMBER;
// printf("table[%3d] = %8lx\n",i,table[i]);
	}
	index = 0;
}

void aRandom::setSeed(random_t *n,size_t l) {
	seed = 0,num = 0,index = 1;
	for(size_t i=0,j=0; i<256; i++,j++) {
		if(j==l) j = 0;
		num ^= n[j];
		table[i] = GENERATE_NUMBER;
// printf("n[%3d] = %8lx\ttable[%3d] = %8lx\n",j,n[j],i,table[i]);
	}
	index = 0;
}


uint32_t aRandom::uint32() { return (uint32_t)RANDOM_NUMBER; }

uint64_t aRandom::uint64() {
#if _WORDSIZE == 64
	return (uint64_t)RANDOM_NUMBER;
#else
	uint64_t n = (uint64_t)RANDOM_NUMBER;
	n = (n<<32)|(uint64_t)RANDOM_NUMBER;
	return n;
#endif
}

/*uint32_t aRandom::uintN(int n) {
	if(n<=0 || n>32) return 0;
	int j;
	uint32_t r = uint32(),i,m = 0xffffffff>>(32-n);
	for(j=32,i=0x80000000; i && !(r&i); j--,i>>=1);
//printf("aRandom::getUintN(r=%lx,i=%lx,m=%lx,j=%d)\n",r,i,m,j);
	if(j>=n) return r>>(j-n);
	r <<= n-j,m >>= n-(n-j);
	return r|(uint32()&m);
}*/

double aRandom::real64() { return (double)uint32()*n2p32; }

int aRandom::roll(int d,int n) {
	if(d<=1) return d*n;
	int v = 0;
	while(n--) v += uint32(d)+1;
	return v;
}

int aRandom::oe(int d,int n) {
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

int aRandom::oeD4(int n) {
	uint32_t v = 0,i= 0,c = 10;
	while(n-->0) {
		if(c==10) i = uint32(),c = 1;
		else i >>= 3,c++;
		// One version of this algorithm discards each value of 4 and adds 2d4
		// if((i&3)==3) n += 2;

		// The prefered version though, instead of 4, adds 3+1d4.
		// This saves number of rolls and generate a more even result.
		if((i&3)==3) v += 3,n++;
		else v += (i&3)+1;
	}
	while(n-->0) {
		if(c==10) i = uint32(),c = 1;
		else i >>= 3,c++;
		if((i&3)==3) n += 2;
		else v += (i&3)+1;
	}
	return v;
}

int aRandom::oeD8(int n) {
	uint32_t v = 0,i = 0,c = 8;
	while(n--) {
		if(c==8) i = uint32(),c = 1;
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

uint32_t aRandom::rollTable(uint32_t *t,uint32_t n,uint32_t l) {
	if(!t || !*t) return 0;
	if(!n) {
		if(!l) for(; t[l]>0; l++);
		n = aMath::sum(t,l);
	}
	n = uint32(n);
	uint32_t i = 0;
	while(n>=t[i]) n -= t[i++];
	return i;
}


void aRandom::write(FILE *fp) {
	if(!fp // To keep compiler happy.
		|| !fwrite(&seed,sizeof(random_t),1,fp)
		|| !fwrite(&num,sizeof(random_t),1,fp)
		|| !fwrite(&index,sizeof(random_t),1,fp)
		|| !fwrite(table,256*sizeof(random_t),1,fp)) return;
}


void aRandom::read(FILE *fp) {
	if(!fp // To keep compiler happy.
		|| !fread(&seed,sizeof(random_t),1,fp)
		|| !fread(&num,sizeof(random_t),1,fp)
		|| !fread(&index,sizeof(random_t),1,fp)
		|| !fread(table,256*sizeof(random_t),1,fp)) return;
}



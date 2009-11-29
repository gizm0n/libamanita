#ifndef _LIBAMANITA_ARANDOM_H
#define _LIBAMANITA_ARANDOM_H

#include <stdio.h>
#include <stdint.h>

typedef uint32_t seed_t;

class aRandom {
private:
	seed_t seed;
	uint32_t num,index,table[256];

	uint32_t generate();

public:
	aRandom();
	aRandom(seed_t n);

	void setSeed(seed_t n);
	void setSeed(seed_t *n,size_t l);
	seed_t getSeed() { return seed; }

	/** Returns a random unsigned 32 bit integer. */
	uint32_t uint32();
	/** Returns a random unsigned 32 bit integer in the range 0-n. */
	uint32_t uint32(uint32_t n) { return uint32()%n; }
	/** Returns a random signed 32 bit integer. */
	int32_t int32() { return (int32_t)uint32(); }
	/** Returns a random signed 32 bit integer in the range 0-n. */
	int32_t int32(int32_t n) { return (int32_t)(n<0? -(uint32()%n) : uint32()%n); }
	/** Returns a random signed 32 bit integer in the range min-max. */
	int32_t int32(int32_t min,int32_t max) { return min+(uint32()%(max-min)); }
	/** Returns a random unsigned 64 bit integer. */
	uint64_t uint64();
	/** Returns a random unsigned 64 bit integer in the range 0-n. */
	uint64_t uint64(uint64_t n) { return uint64()%n; }
	/** Returns a random signed 64 bit integer. */
	int64_t int64() { return (int64_t)uint64(); }
	/** Returns a random signed 64 bit integer in the range 0-n. */
	int64_t int64(int64_t n) { return n<0? -(uint64()%n) : uint64()%n; }
	/** Returns a random signed 64 bit integer in the range min-max. */
	int64_t getSint64(int64_t min,int64_t max) { return min+(uint64()%(max-min)); }
	/** Returns a random double in the range 0.0-1.0. */
	double real64();
	/** Returns a random double in the range 0.0-n. */
	double real64(double n) { return real64()*n; }

	/** Returns a random unsigned 32 bit integer that is exactly n bits, no more, no less. */
	uint32_t uintN(int n);

	char alpha() { int i = uint32()%52;return i<26? 'A'+i : 'a'-26+i; }
	char alphanum() { int i = uint32()%62;return i<10? '0'+i : (i<36? 'A'-10+i : 'a'-36+i); }

	int roll(int d,int n);
	int roll(int d) { return uint32(d)+1; }
	int rollD2() { return uint32(2)+1; }
	int rollD2(int n) { return roll(2,n); }
	int rollD4() { return uint32(4)+1; }
	int rollD4(int n) { return roll(4,n); }
	int rollD6() { return uint32(6)+1; }
	int rollD6(int n) { return roll(6,n); }
	int rollD8() { return uint32(8)+1; }
	int rollD8(int n) { return roll(8,n); }
	int rollD10() { return uint32(10)+1; }
	int rollD10(int n) { return roll(10,n); }
	int rollD12() { return uint32(12)+1; }
	int rollD12(int n) { return roll(12,n); }
	int rollD20() { return uint32(20)+1; }
	int rollD20(int n) { return roll(20,n); }
	int rollD100() { return uint32(100)+1; }
	int rollD100(int n) { return roll(100,n); }
	int oe(int d,int n);									// Open ended dice roll
	int oeD4(int n);											// Open ended D4 rolls
	int oeD6(int n) { return oe(6,n); }				// Open ended D6 rolls
	int oeD8(int n);											// Open ended D8 rolls

	uint32_t rollTable(uint32_t *t,size_t l,uint32_t s=0);
};

extern aRandom rnd;

#endif /* _LIBAMANITA_ARANDOM_H */


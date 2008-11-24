#ifndef _LIBAMANITA_RANDOM_H
#define _LIBAMANITA_RANDOM_H

#include <stdio.h>

class Integer;

typedef unsigned long seed_t;

class Random {
private:
	seed_t seed;
	unsigned long num,index,table[256];

	unsigned long generate();

public:
	Random();
	Random(seed_t n);

	void setSeed(seed_t n);
	void setSeed(seed_t *n,int l);
	void setSeed(Integer &n);
	seed_t getSeed() { return seed; }

	/** Returns a random unsigned long. */
	unsigned long getUint32();
	/** Returns a random unsigned long in the range 0-n. */
	unsigned long getUint32(unsigned long n) { return getUint32()%n; }
	/** Returns a random signed long. */
	long getSint32() { return (long)getUint32(); }
	/** Returns a random signed long in the range 0-n. */
	long getSint32(long n) { return n<0? -(getUint32()%n) : getUint32()%n; }
	/** Returns a random signed long in the range min-max. */
	long getSint32(long min,long max) { return min+(getUint32()%(max-min)); }
	/** Returns a random unsigned long long. */
	unsigned long long getUint64();
	/** Returns a random unsigned long long in the range 0-n. */
	unsigned long long getUint64(unsigned long long n) { return getUint64()%n; }
	/** Returns a random signed long long. */
	long long getSint64() { return (long long)getUint64(); }
	/** Returns a random signed long long in the range 0-n. */
	long long getSint64(long long n) { return n<0? -(getUint64()%n) : getUint64()%n; }
	/** Returns a random signed long long in the range min-max. */
	long long getSint64(long long min,long long max) { return min+(getUint64()%(max-min)); }
	/** Returns a random double in the range 0.0-1.0. */
	double getDouble();
	/** Returns a random double in the range 0.0-n. */
	double getDouble(double n) { return getDouble()*n; }

	/** Returns a random unsigned long that is exactly n bits, no more, no less. */
	unsigned long getUintN(int n);

	char getAlpha() { int i = getUint32()%52;return i<26? 'A'+i : 'a'-26+i; }
	char getAlphaNumeric() { int i = getUint32()%62;return i<10? '0'+i : (i<36? 'A'-10+i : 'a'-36+i); }

	int roll(int d,int n);
	int roll(int d) { return getUint32(d)+1; }
	int rollD2() { return getUint32(2)+1; }
	int rollD2(int n) { return roll(2,n); }
	int rollD4() { return getUint32(4)+1; }
	int rollD4(int n) { return roll(4,n); }
	int rollD6() { return getUint32(6)+1; }
	int rollD6(int n) { return roll(6,n); }
	int rollD8() { return getUint32(8)+1; }
	int rollD8(int n) { return roll(8,n); }
	int rollD10() { return getUint32(10)+1; }
	int rollD10(int n) { return roll(10,n); }
	int rollD12() { return getUint32(12)+1; }
	int rollD12(int n) { return roll(12,n); }
	int rollD20() { return getUint32(20)+1; }
	int rollD20(int n) { return roll(20,n); }
	int rollD100() { return getUint32(100)+1; }
	int rollD100(int n) { return roll(100,n); }
	int oe(int d,int n);									// Open ended dice roll
	int oeD4(int n);											// Open ended D4 rolls
	int oeD6(int n) { return oe(6,n); }				// Open ended D6 rolls
	int oeD8(int n);											// Open ended D8 rolls

	unsigned long rollTable(unsigned long *t,size_t l,unsigned long s=0ul);
};

extern Random rnd;

#endif /* _LIBAMANITA_RANDOM_H */


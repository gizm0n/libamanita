#ifndef _LIBAMANITA_ARANDOM_H
#define _LIBAMANITA_ARANDOM_H

#include <stdio.h>
#include <stdint.h>

/** The random_t type contains a random number.
 */
#if _WORDSIZE == 64
typedef uint64_t random_t;
#else
typedef uint32_t random_t;
#endif

class aRandom {
private:
	random_t seed;
	random_t num;
	random_t index;
	random_t table[256];

public:
	aRandom();
	aRandom(random_t n);

	void setSeed(random_t n);
	void setSeed(random_t *n,size_t l);
	random_t getSeed() { return seed; }

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

	/** Returns a random unsigned 32 bit integer that is exactly n bits, no more, no less. 
	 */
	uint32_t uintN(int n);

	/** Returns a random value within the latin alphabet [a-zA-Z]. */
	char alpha() { int i = uint32()%52;return i<26? 'A'+i : 'a'-26+i; }

	/** Returns a random value within the latin alphabet or a digit [a-zA-Z0-9]. */
	char alphanum() { int i = uint32()%62;return i<10? '0'+i : (i<36? 'A'-10+i : 'a'-36+i); }

	/** The roll*-methods simulates dice rolls, and so generates n number of rolls of a d-sided dice.
	 * The result is per dice-roll 1-d, compared to uint32(uint32_t n) that generated a random number with the value 0-(n-1)
	 */
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

	/** Open ended dice roll.
	 * 
	 * where when the result is d is not added to the sum but instead adds two more rolls.
	 */
	int oe(int d,int n);										/**< Open ended roll of n number of d-sided dice. */
	int oeD4(int n);											/**< Open ended D4 rolls. */
	int oeD6(int n) { return oe(6,n); }					/**< Open ended D6 rolls. */
	int oeD8(int n);											/**< Open ended D8 rolls. */


	/** Rolls a number 0-s and returns the index number in the table t in the range 0-l.
	 * 
	 * The table t's values should (per unit) only name the size of itself. Look
	 * at this example for a better understanding:
	 * 
	 * uint32_t table[] = { 2,4,7,2,4,9,0 };
	 * uint32_t n = rollTable(table);
	 * 
	 * This will be read as a table with index and range, with l calculated to 6 and s to 28:
	 *    -# 0-1
	 *    -# 2-5
	 *    -# 6-12
	 *    -# 13-14
	 *    -# 15-18
	 *    -# 19-27
	 * 
	 * @param t The table, it should contain a list of positive integers and ending with 0. The sum of the values in the list should not be larger than 0xFFFFFFFF.
	 * @param s If s==0 it's calculated to be the sum of all values of t.
	 */
	uint32_t rollTable(uint32_t *t,uint32_t l=0,uint32_t s=0);

	void write(FILE *fp);
	void read(FILE *fp);
};

extern aRandom rnd;

#endif /* _LIBAMANITA_ARANDOM_H */


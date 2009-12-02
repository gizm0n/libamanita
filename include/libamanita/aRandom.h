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

	/** @name Random integer numbers
	 * This group of methods generate various types of random integer numbers.
	 */
	//@{
	/** @return A random unsigned 32 bit integer. */
	uint32_t uint32();

	/** Generate a random number in the range 0-(n-1).
	 * @param n
	 * @return A random unsigned 32 bit integer. */
	uint32_t uint32(uint32_t n) { return (n==0? 0 : uint32()%n); }

	/** Generates a number that is exactly n bits of randomness.
	 * @param n
	 * @return A random unsigned 32 bit integer. */
	uint32_t uintN(int n) { return (n<=0 || n>32? 0 : (uint32()&(0xffffffff>>(32-n)))); }

	/** @return A random signed 32 bit integer. */
	int32_t int32() { return (int32_t)uint32(); }

	/** Generate a random number in the range 0-(n-1).
	 * @param n
	 * @return A random signed 32 bit integer. */
	int32_t int32(int32_t n) { return (int32_t)(n==0? 0 : (n<0? -(uint32()%n) : uint32()%n)); }

	/** Generate a random number in the range min-max.
	 * @param min The minimum value that is generated.
	 * @param max The maximum value that is generated.
	 * @return A random signed 32 bit integer. */
	int32_t int32(int32_t min,int32_t max) { return (max==min? min : (max<min? int32(max,min) : min+(uint32()%(max-min)))); }

	/** @return A random unsigned 64 bit integer. */
	uint64_t uint64();

	/** Generate a random number in the range 0-(n-1).
	 * @param n
	 * @return A random unsigned 64 bit integer. */
	uint64_t uint64(uint64_t n) { return (n==0? 0 : uint64()%n); }

	/** @return A random signed 64 bit integer. */
	int64_t int64() { return (int64_t)uint64(); }

	/** Generate a random number in the range 0-(n-1).
	 * @param n
	 * @return A random signed 64 bit integer. */
	int64_t int64(int64_t n) { return (int64_t)(n==0? 0 : (n<0? -(uint64()%n) : uint64()%n)); }

	/** Generate a random number in the range min-max.
	 * @param min The minimum value that is generated.
	 * @param max The maximum value that is generated.
	 * @return A random signed 64 bit integer. */
	int64_t int64(int64_t min,int64_t max) { return (max==min? min : (max<min? int64(max,min) : min+(uint64()%(max-min)))); }
	//@}

	/** @name Random real numbers
	 * This group of methods generate values of the type double. */
	//@{
	/** @return A random double in the range 0.0-1.0. */
	double real64();

	/** @return A random double in the range 0.0-n. */
	double real64(double n) { return real64()*n; }
	//@}

	/** @name Random chars
	 * This group of methods generate random chars. */
	//@{
	/** @return A random char within the latin alphabet [a-zA-Z]. */
	char alpha() { int i = uint32()%52;return i<26? 'A'+i : 'a'-26+i; }

	/** @return A random char within the latin alphabet or a digit [a-zA-Z0-9]. */
	char alphanum() { int i = uint32()%62;return i<10? '0'+i : (i<36? 'A'-10+i : 'a'-36+i); }
	//@}

	/** @name Roll methods
	 * The roll-methods simulates dice rolls, and so generates a number of rolls of a D-sided dice.
	 * 
	 * The result is per dice-roll 1-d, compared to uint32(uint32_t n) that generated a
	 * random number with the value 0-(n-1)
	 */
	//@{
	/** Rolls n number of d-sided dice.
	 * @param d Sides of the dice.
	 * @param n Number of dice rolled.
	 * @return The sum of the dice rolled.
	 */
	int roll(int d,int n);

	/** Rolls a d-sided dice.
	 * @param d Sides of the dice.
	 * @return The result of the dice rolled.
	 */
	int roll(int d) { return uint32(d)+1; }
	int rollD2() { return (uint32()&1)+1; }			/**< Rolls a D2 die. */
	int rollD2(int n) { return roll(2,n); }			/**< Rolls n D2 dice. */
	int rollD4() { return (uint32()&3)+1; }			/**< Rolls a D4 die. */
	int rollD4(int n) { return roll(4,n); }			/**< Rolls n D4 dice. */
	int rollD6() { return uint32(6)+1; }				/**< Rolls a D6 die. */
	int rollD6(int n) { return roll(6,n); }			/**< Rolls n D6 dice. */
	int rollD8() { return (uint32()&7)+1; }			/**< Rolls a D8 die. */
	int rollD8(int n) { return roll(8,n); }			/**< Rolls n D8 dice. */
	int rollD10() { return uint32(10)+1; }				/**< Rolls a D10 die. */
	int rollD10(int n) { return roll(10,n); }			/**< Rolls n D10 dice. */
	int rollD12() { return uint32(12)+1; }				/**< Rolls a D12 die. */
	int rollD12(int n) { return roll(12,n); }			/**< Rolls n D12 dice. */
	int rollD20() { return uint32(20)+1; }				/**< Rolls a D20 die. */
	int rollD20(int n) { return roll(20,n); }			/**< Rolls n D20 dice. */
	int rollD100() { return uint32(100)+1; }			/**< Rolls a D100 die. */
	int rollD100(int n) { return roll(100,n); }		/**< Rolls n D100 dice. */

	/** Rolls a number 0-(n-1) and returns the index number in the table t in the range 0-(l-1).
	 * 
	 * The table t's values should (per unit) only contain the size of itself.
	 * Look at this example for a better understanding:
	 * @code
	 * uint32_t table[] = { 2,4,7,2,4,9,0 };
	 * uint32_t i = rnd.rollTable(table);
	 * @endcode
	 * This will be read as a table with index and range, with l calculated to 6 and n to 28:
	 *    -0. 0-1
	 *    -1. 2-5
	 *    -2. 6-12
	 *    -3. 13-14
	 *    -4. 15-18
	 *    -5. 19-27
	 * 
	 * If n>0, then the table can contain values of 0, and that only means the empty table cells
	 * are skipped and won't ever be returned. For an example:
	 * @code
	 * uint32_t table[] = { 8,3,0,4,1,7,0,2,0 };
	 * uint32_t i = rnd.rollTable(table,25);
	 * @endcode
	 * In this example, i can be a value of 0, 1, 3, 4, 5, or 7.
	 * @param t The table. It should contain a list of positive integers and ending with 0.
	 * The sum of the values in the list should not be larger than 0xFFFFFFFF.
	 * @param n The sum of values in the table. If set to 0, n is calculated to be the sum of all values of t.
	 * @param l Length of the table. If n is set to a value >0, then l can be omitted, because
	 * it's only used to calculate n. If set to 0 and n is also 0, then the algorithm counts
	 * values in t until it reaches a 0 and set l to this.
	 * @return The index of the rolled values in the table, ranging from 0-(l-1).
	 */
	uint32_t rollTable(uint32_t *t,uint32_t l=0,uint32_t n=0);
	//@}

	/** Open ended dice roll.
	 * 
	 * When the result is d is not added to the sum but instead adds two more rolls.
	 */
	//@{
	/** Rolls n number of d-sided dice open ended.
	 * @param d Sides of the dice.
	 * @param n Number of dice rolled.
	 * @return The sum of the dice rolled.
	 */
	int oe(int d,int n);
	int oeD4(int n);											/**< Open ended D4 rolls. */
	int oeD6(int n) { return oe(6,n); }					/**< Open ended D6 rolls. */
	int oeD8(int n);											/**< Open ended D8 rolls. */
	//@}


	void write(FILE *fp);
	void read(FILE *fp);
};

extern aRandom rnd;

#endif /* _LIBAMANITA_ARANDOM_H */


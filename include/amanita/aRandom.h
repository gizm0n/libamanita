#ifndef _AMANITA_RANDOM_H
#define _AMANITA_RANDOM_H

/**
 * @file amanita/aRandom.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 


#include <stdio.h>
#include <stdint.h>


/** Defines an integer number used internally by the engine, and to
 * set the seed. */
typedef unsigned long random_t;


/** A class that generates pseudorandom numbers.
 * 
 * The algorithm is inspired by mersenne twister, but differs in some ways. It is designed
 * to be fast, before generating perfect randomness, though the entropy values measured are
 * relatively good.
 * 
 * From the seed is generated a sequence of 256 integer numbers with a decent randomness and
 * place in the table. For generation of new random numbers are used two more variables; "num"
 * and "index". "Index" is an unsigned integer of the random_t-type that counts forward +1 for
 * every generated number. This ensures we avoid getting stuck in loops. "Num" is also an
 * unsigned integer of type random_t, it is always the value of the previously generated number.
 * When generating a new number we take two numbers in the table, add num to the second
 * number and xor them together. The result is placed in the address of the first number.
 * The code for this algorithm looks like this:
 * @code
table[index&0xff] ^= table[num&0xff] + num;
num = table[index&0xff];
index = index +1;
 * @endcode
 * It isn't more complicated than this, and yet it generates quite a good result compared to other
 * random number generators. A comparsion can be made here:
 * http://www.cacert.at/cgi-bin/rngresults
 * 
 * Actually, given a sufficently great enough amount of random data it seems to top the list,
 * but even a smaller amount of data places aRandom at a good spot.
 * 
 * How to use the class is extremely simple. There is a global instance of aRandom that may
 * be used anywhere where amanita/aRandom.h is included. It is named "rnd". You may also
 * instantiate aRandom as an object to keep track of the number sequence, and to make certain
 * it's not affected somewhere else in the program, because this could easily happen with the
 * global rnd.
 * 
 * Instantiating aRandom has it advantages in games for example, wher you perhaps want to ensure
 * that a certain sequence of random events always happens within a specific game, to avoid that
 * the player saves and loads and saves and loads to gain advantages. By storing aRandom with the
 * save file ensures exactly the same random sequence through the entire game, if following the
 * same paths. Of course, doing something else may trigger some other event or sequence of events,
 * but that's perhaps the way it should be? When situations like this are not called for, rnd
 * works very well.
 * 
 * To ge a random number, simply call any of the given methods, depending on what type of value
 * you require. The most commonly used method is perhaps uint32(), that generates an unsigned
 * 32 bits integer. There are some methods specific for games, board games or role-playing games.
 * 
 * Here is one example of how to get a random int:
 * @code
int i = rnd.int32();
 * @endcode
 * 
 * Here's an example on how to store and restore an instance of aRandom:
 * @code
aRandom r; // Sets seed to time(0).
FILE *f = fopen("random.dat","wb");
r.write(f);
fclose(f);

... // Do romething with r.

f = fopen("random.dat","rb");
r.read(f); // Reads r to be exactly the same as when saved.
fclose(f);
 * @endcode
 * @ingroup amanita
 */
class aRandom {
private:
	random_t seed;					//!< The initial seed from which the random sequence is generated.
	random_t num;					//!< The last random number in the sequence.
	random_t index;				//!< The index in the table.
	random_t table[256];			//!< The table used for random number generation.

public:
	/** @name Constructors
	 * The constructors for aRandom.
	 * @{ */
	/** Constructor method. Sets seed to time(0) in time.h. */
	aRandom();

	/** Constructor method. Sets seed to n.
	 * @param n Sets seed to n. */
	aRandom(random_t n);
	/** @} */

	/** @name Random engine
	 * Methods to handle the enginge internally.
	 * @{ */
	/** Set seed and generate a new sequence of numbers.
	 * @param n The seed. */
	void setSeed(random_t n);

	/** Set seed and generate a new sequence of numbers.
	 * 
	 * This method uses the entire list n as the seed.
	 * @param n A list of seed numbers.
	 * @param l Length of n. */
	void setSeed(random_t n[],size_t l);

	/** Get the seed used for random sequence.
	 * @return The seed. */
	random_t getSeed() { return seed; }

	/** Get the last number generated. It doesn't generate a new number,
	 * only repeats the last generated number.
	 * @return The last generated number. */
	random_t repeat() { return num; }
	/** @} */

	/** @name Random integer numbers
	 * This group of methods generate various types of random integer numbers.
	 * @{ */
	/** Generate a random number in the range 0-0xffffffff.
	 * @return A random unsigned 32 bit integer. */
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
	/** @} */

	/** @name Random real numbers
	 * This group of methods generate values of the type double.
	 * @{ */
	/** @return A random double in the range 0.0-1.0. */
	double real64();

	/** @return A random double in the range 0.0-n. */
	double real64(double n) { return real64()*n; }
	/** @} */

	/** @name Random chars
	 * This group of methods generate random chars.
	 * @{ */
	/** @return A random char within the latin alphabet [a-zA-Z]. */
	char alpha() { int i = uint32()%52;return i<26? 'A'+i : 'a'-26+i; }

	/** @return A random char within the latin alphabet or a digit [a-zA-Z0-9]. */
	char alphanum() { int i = uint32()%62;return i<10? '0'+i : (i<36? 'A'-10+i : 'a'-36+i); }
	/** @} */

	/** @name Roll methods
	 * The roll-methods simulates dice rolls, and so generates a number of rolls of a D-sided dice.
	 * 
	 * The result is per dice-roll 1-d, compared to uint32(uint32_t n) that generated a
	 * random number with the value 0-(n-1)
	 * @{ */
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
	int rollD2() { return (uint32()&1)+1; }			//!< Rolls a D2 die.
	int rollD2(int n) { return roll(2,n); }			//!< Rolls n number of D2 dice.
	int rollD4() { return (uint32()&3)+1; }			//!< Rolls a D4 die.
	int rollD4(int n) { return roll(4,n); }			//!< Rolls n number of D4 dice.
	int rollD6() { return uint32(6)+1; }				//!< Rolls a D6 die.
	int rollD6(int n) { return roll(6,n); }			//!< Rolls n number of D6 dice.
	int rollD8() { return (uint32()&7)+1; }			//!< Rolls a D8 die.
	int rollD8(int n) { return roll(8,n); }			//!< Rolls n number of D8 dice.
	int rollD10() { return uint32(10)+1; }				//!< Rolls a D10 die.
	int rollD10(int n) { return roll(10,n); }			//!< Rolls n number of D10 dice.
	int rollD12() { return uint32(12)+1; }				//!< Rolls a D12 die.
	int rollD12(int n) { return roll(12,n); }			//!< Rolls n number of D12 dice.
	int rollD20() { return uint32(20)+1; }				//!< Rolls a D20 die.
	int rollD20(int n) { return roll(20,n); }			//!< Rolls n number of D20 dice.
	int rollD100() { return uint32(100)+1; }			//!< Rolls a D100 die.
	int rollD100(int n) { return roll(100,n); }		//!< Rolls n number of D100 dice.

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
	uint32_t rollTable(int *t,int n=0,int l=0);
	/** @} */

	/** @name Open ended roll methods
	 * 
	 * When the result is d is not added to the sum but instead adds two more rolls.
	 * @{ */
	/** Rolls n number of d-sided dice open ended.
	 * @param d Sides of the dice.
	 * @param n Number of dice rolled.
	 * @return The sum of the dice rolled.
	 */
	int oe(int d,int n);
	int oeD4(int n);											//!< Open ended D4 rolls.
	int oeD6(int n) { return oe(6,n); }					//!< Open ended D6 rolls.
	int oeD8(int n);											//!< Open ended D8 rolls.
	/** @} */

	/** @name IO
	 * Methods for Input/Output.
	 * @{ */
	/** Write to file.
	 * 
	 * Write the internal data of the engine to file (it's 1036 byte or 2072 bytes depending on if it's a
	 * 32 bit processor or a 64 bit).
	 * @param fp File to write to. */
	void write(FILE *fp);

	/** Read from file.
	 * 
	 * Read stored data into the random engine to get an exact print of when it was stored.
	 * @param fp File to read from. */
	void read(FILE *fp);
	/** @} */
};

/** @var rnd The global instance of aRandom.
 * To use rnd, all you need to do is include amanita/aRandom.h and link to libamanita.*/
extern aRandom rnd;



#endif /* _AMANITA_RANDOM_H */


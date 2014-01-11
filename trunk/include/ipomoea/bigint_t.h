#ifndef _IPOMOEA_BIGINT_T_H
#define _IPOMOEA_BIGINT_T_H

/**
 * @file ipomoea/bigint_t.h  
 * @author Per Löwgren
 * @date Modified: 2013-12-29
 * @date Created: 2013-12-27
 */ 

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>


enum {
	Plus,									//!< Sign of this is set to Plus if it is a positive number.
	Minus,								//!< Sign of this is set to Minus if it is a negative number.
	NaN,									//!< Sign of this is set to Minus if it is not a number.
	Infinite,							//!< Sign of this is set to Minus if it is an infinite number.
};


typedef struct bigint_t {
	uint8_t sign;						//!< Sign tells what type of number this is.
	uint32_t *num;						//!< Num contains all data of this.
	size_t len;							//!< Len is the length of the number this is set to in number of 32 bit integers, it is always >= 1.
	size_t cap;							//!< Cap is the capacity of num, it is always >= 1, and len is always <= cap.
} bigint_t;


bigint_t *bi_new();											//!< Constructor.
void bi_delete(bigint_t *bi);								//!< Destructor.

/** Increase capacity with n.
 * @param n Number of 32 bit integers to increace capacity with. */
bigint_t *bi_resize(bigint_t *bi,int n);

bigint_t *bi_seti32(bigint_t *bi,int32_t n);			//!< Set this to n.
bigint_t *bi_setu32(bigint_t *bi,uint32_t n);		//!< Set this to n.
bigint_t *bi_seti64(bigint_t *bi,int64_t n);			//!< Set this to n.
bigint_t *bi_setu64(bigint_t *bi,uint64_t n);		//!< Set this to n.
bigint_t *bi_setstr(bigint_t *bi,const char *n);	//!< Set this to n.
bigint_t *bi_set(bigint_t *bi,const bigint_t *n);	//!< Set this to n.

void bi_inc(bigint_t *bi,uint32_t n);					//!< Increase this with n.
void bi_dec(bigint_t *bi,uint32_t n);					//!< Decrease this with n.
void bi_add(bigint_t *bi,const bigint_t *n);			//!< Add n to this.
void bi_sub(bigint_t *bi,const bigint_t *n);			//!< Subtract n from this.
void bi_mul(bigint_t *bi,const bigint_t *n);			//!< Multiply this with n.

/** Divide this with n.
 * The quota and remainder is stored in q and r.
 * @param n Value to divide with.
 * @param q Where to store quota in.
 * @param r Where to store remainder in. */
void bi_div(bigint_t *bi,const bigint_t *n,bigint_t *q,bigint_t *r);

void bi_lshift(bigint_t *bi,size_t n);					//!< Shift this n bits left.
void bi_rshift(bigint_t *bi,size_t n);					//!< Shift this n bits right.

int bi_cmpi32(const bigint_t *bi,int32_t n);			//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
int bi_cmpu32(const bigint_t *bi,uint32_t n);		//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
int bi_cmpi64(const bigint_t *bi,int64_t n);			//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
int bi_cmpu64(const bigint_t *bi,uint64_t n);		//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
int bi_cmp(const bigint_t *bi,const bigint_t *n);	//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.


/** Sets the Integer sign to Plus. */
void bi_abs(bigint_t *bi);

/** Calculates sqrt of the value of this, and sets this to that value. */
void bi_sqrt(bigint_t *bi);

/** Euclidean algorithm.
 * In number theory, the Euclidean algorithm (also called Euclid's algorithm) is an algorithm to determine
 * the greatest common divisor (GCD) of two elements of any Euclidean domain (for example, the integers).
 * Its major significance is that it does not require factoring the two integers, and it is also significant
 * in that it is one of the oldest algorithms known, dating back to the ancient Greeks.
 */
//bigint_t *bi_gcd(bigint_t *bi,const bigint_t *n);

/** Modular exponentiation. */
//bigint_t *bi_modpow(bigint_t *bi,const bigint_t *e,const bigint_t *m);

/** Calculates the largest prime factor and sets this to that value. */
//bigint_t *bi_factor(bigint_t *bi);

/** Creates a random integer of exactly n bits. */
//bigint_t *bi_random(size_t n);

/** Returns the log2 of this as an integer floor value.
 * @return log2 of this as an integer floor value. */
//size_t bi_log2(bigint_t *bi);

/** Returns number of bits in this.
 * @return Number of bits in this. */
//size_t bi_bits(bigint_t *bi);

/** Returns the bit at n, starting with 0.
 * @param n The index number ot the bit.
 * @return The bit value at n, either 0 or 1.*/
//int bi_bit(bigint_t *bi,size_t n);

/** Set bit at n, where n can be any value and this will resize to fit, to value i, that is either 0 or else 1.
 * @param n The index number of the bit to set.
 * @param i The value to set n to, either 1 or 0. */
//void bi_setbit(bigint_t *bi,size_t n,int i);

int bi_is_zero(const bigint_t *bi);			//!< Returns true if this is zero.
int bi_is_negative(const bigint_t *bi);		//!< Returns true if sign of this is set to Minus.
int bi_is_nan(const bigint_t *bi);				//!< Returns true if sign of this is set to NaN.
int bi_is_infinite(const bigint_t *bi);		//!< Returns true if sign of this is set to Infinite.
//int bi_is_prime(const bigint_t *bi);			//!< Returns true if this is a prime-number.
//int bi_is_safe_prime(const bigint_t *bi);	//!< Returns true if this is a safe prime-number.
int bi_is_even(const bigint_t *bi);			//!< Returns true if this is an even number.
int bi_is_odd(const bigint_t *bi);				//!< Returns true if this is an odd number.

/** Print to file.
 * This method prints the integer as digital numbers. */
void bi_print(const bigint_t *bi);



#ifdef __cplusplus
}
#endif
#endif /* _IPOMOEA_BIGINT_T_H */


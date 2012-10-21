#ifndef _AMANITA_INTEGER_H
#define _AMANITA_INTEGER_H

/**
 * @file amanita/Integer.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>


/** Amanita Namespace */
namespace a {


/** A class for calculating big integers.
 * 
 * @ingroup amanita */
class Integer {
private:
	enum {
		Plus,									//!< Sign of this is set to Plus if it is a positive number.
		Minus,								//!< Sign of this is set to Minus if it is a negative number.
		NaN,									//!< Sign of this is set to Minus if it is not a number.
		Infinite,							//!< Sign of this is set to Minus if it is an infinite number.
	};

	uint8_t sign;							//!< Sign tells what type of number this is.
	uint32_t *num;							//!< Num contains all data of this.
	size_t len;								//!< Len is the length of the number this is set to in number of 32 bit integers, it is always >= 1.
	size_t cap;								//!< Cap is the capacity of num, it is always >= 1, and len is always <= cap.

	/** Increase capacity with n.
	 * @param n Number of 32 bit integers to increace capacity with. */
	void resize(int n);

	/** @name Set
	 * @{ */
	void set32(int32_t n);				//!< Set this to n.
	void setu32(uint32_t n);			//!< Set this to n.
	void set64(int64_t n);				//!< Set this to n.
	void setu64(uint64_t n);			//!< Set this to n.
	void set(const char *n);			//!< Set this to n.
	void set(const Integer &n);		//!< Set this to n.
	/** @} */

	/** @name Primitives
	 * @{ */
	void inc(uint32_t n);				//!< Increase this with n.
	void dec(uint32_t n);				//!< Decrease this with n.
	void add(const Integer &n);		//!< Add n to this.
	void sub(const Integer &n);		//!< Subtract n from this.
	void mul(const Integer &n);		//!< Multiply this with n.

	/** Divide this with n.
	 * The quota and remainder is stored in q and r.
	 * @param n Value to divide with.
	 * @param q Where to store quota in.
	 * @param r Where to store remainder in. */
	void div(const Integer &n,Integer *q,Integer *r);
	void lshift(size_t n);				//!< Shift this n bits left.
	void rshift(size_t n);				//!< Shift this n bits right.
	/** @} */

	/** @name Compare
	 * @{ */
	int cmp(int32_t n);					//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
	int cmp(uint32_t n);					//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
	int cmp(int64_t n);					//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
	int cmp(uint64_t n);					//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
	int cmp(const Integer &n);		//!< Compare this with n. @return 0 if equal, <0 if less, and >0 if bigger.
	/** @} */

//	void monpro(const Integer &n,const Integer &r);

public:
	/** @name Constructors and Destructors
	 * @{ */
	Integer() : num(0) { set((uint32_t)0); }
	Integer(int32_t n) : num(0) { set32((int32_t)n); }
	Integer(uint32_t n) : num(0) { setu32((uint32_t)n); }
	Integer(int64_t n) : num(0) { set64((int64_t)n); }
	Integer(uint64_t n) : num(0) { setu64((uint64_t)n); }
	Integer(const char *n) : num(0) { set(n); }
	Integer(const Integer &n) : num(0) { set(n); }
	~Integer();
	/** @} */

	/** @name Operators
	 * @{ */
	Integer &operator=(int32_t n) { set32(n);return *this; }
	Integer &operator=(uint32_t n) { setu32(n);return *this; }
	Integer &operator=(int64_t n) { set64(n);return *this; }
	Integer &operator=(uint64_t n) { setu64(n);return *this; }
	Integer &operator=(const char *n) { set(n);return *this; }
	Integer &operator=(const Integer &n) { set(n);return *this; }
	Integer &operator++(int n) { if(sign==Minus) dec(1);else inc(1);return *this; }
	Integer &operator--(int n) { if(sign==Minus) inc(1);else dec(1);return *this; }
	Integer operator+(const Integer &n) { Integer i(*this);i += n;return i; }
	Integer &operator+=(int n) { if(n<0) *this -= -n;else if(sign==Minus) dec(n);else inc(n);return *this; }
	Integer &operator+=(long n) { if(n<0) *this -= -n;else if(sign==Minus) dec(n);else inc(n);return *this; }
	Integer &operator+=(unsigned long n) { if(sign==Minus) dec(n);else inc(n);return *this; }
	Integer &operator+=(const Integer &n);
	Integer operator-(const Integer &n) { Integer i(*this);i -= n;return i;}
	Integer &operator-=(int n) { if(n<0) *this += -n;else if(sign==Minus) inc(n);else dec(n);return *this; }
	Integer &operator-=(long n) { if(n<0) *this += -n;else if(sign==Minus) inc(n);else dec(n);return *this; }
	Integer &operator-=(unsigned long n) { if(sign==Minus) inc(n);else dec(n);return *this; }
	Integer &operator-=(const Integer &n);
	Integer operator*(const Integer &n) { Integer i(*this);i.mul(n);return i; }
	Integer &operator*=(const Integer &n) { mul(n);return *this; }
	Integer operator/(const Integer &n) { Integer i(*this);div(n,&i,0);return i; }
	Integer &operator/=(const Integer &n) { div(n,this,0);return *this; }
	Integer operator%(const Integer &n) { Integer i(*this);div(n,0,&i);return i; }
	Integer &operator%=(const Integer &n) { div(n,0,this);return *this; }
	Integer operator<<(size_t n) { Integer i(*this);i.lshift(n);return i; }
	Integer &operator<<=(size_t n) { lshift(n);return *this; }
	Integer operator>>(size_t n) { Integer i(*this);i.rshift(n);return i; }
	Integer &operator>>=(size_t n) { rshift(n);return *this; }

	bool operator==(int32_t n) { return cmp(n)==0; }
	bool operator==(uint32_t n) { return cmp(n)==0; }
	bool operator==(int64_t n) { return cmp(n)==0; }
	bool operator==(uint64_t n) { return cmp(n)==0; }
	bool operator==(const Integer &n) { return cmp(n)==0; }
	bool operator!=(int32_t n) { return cmp(n)!=0; }
	bool operator!=(uint32_t n) { return cmp(n)!=0; }
	bool operator!=(int64_t n) { return cmp(n)!=0; }
	bool operator!=(uint64_t n) { return cmp(n)!=0; }
	bool operator!=(const Integer &n) { return cmp(n)!=0; }
	bool operator<(int32_t n) { return cmp(n)<0; }
	bool operator<(uint32_t n) { return cmp(n)<0; }
	bool operator<(int64_t n) { return cmp(n)<0; }
	bool operator<(uint64_t n) { return cmp(n)<0; }
	bool operator<(const Integer &n) { return cmp(n)<0; }
	bool operator<=(int32_t n) { return cmp(n)<=0; }
	bool operator<=(uint32_t n) { return cmp(n)<=0; }
	bool operator<=(int64_t n) { return cmp(n)<=0; }
	bool operator<=(uint64_t n) { return cmp(n)<=0; }
	bool operator<=(const Integer &n) { return cmp(n)<=0; }
	bool operator>(int32_t n) { return cmp(n)>0; }
	bool operator>(uint32_t n) { return cmp(n)>0; }
	bool operator>(int64_t n) { return cmp(n)>0; }
	bool operator>(uint64_t n) { return cmp(n)>0; }
	bool operator>(const Integer &n) { return cmp(n)>0; }
	bool operator>=(int32_t n) { return cmp(n)>=0; }
	bool operator>=(uint32_t n) { return cmp(n)>=0; }
	bool operator>=(int64_t n) { return cmp(n)>=0; }
	bool operator>=(uint64_t n) { return cmp(n)>=0; }
	bool operator>=(const Integer &n) { return cmp(n)>=0; }
	/** @} */

	/** @name Algorithms
	 * @{ */
	/** Sets the Integer sign to Plus. */
	Integer &abs() { if(sign==Minus) sign = Plus;return *this; }

	/** Calculates sqrt of the value of this, and sets this to that value. */
	Integer &sqrt();

	/** Euclidean algorithm.
	 * In number theory, the Euclidean algorithm (also called Euclid's algorithm) is an algorithm to determine
	 * the greatest common divisor (GCD) of two elements of any Euclidean domain (for example, the integers).
	 * Its major significance is that it does not require factoring the two integers, and it is also significant
	 * in that it is one of the oldest algorithms known, dating back to the ancient Greeks.
	 */
	Integer &gcd(const Integer &n);

	/** Modular exponentiation. */
	Integer &modpow(const Integer &e,const Integer &m);

	/** Calculates the largest prime factor and sets this to that value. */
	Integer &factor();

	/** Creates a random integer of exactly n bits. */
	Integer &random(size_t n);

	/** Returns the log2 of this as an integer floor value.
	 * @return log2 of this as an integer floor value. */
	size_t log2() const;
	/** @} */

	/** @name Internal
	 * @{ */
	/** Returns number of bits in this.
	 * @return Number of bits in this. */
	size_t bits() const;

	/** Returns number of unsigned long (32 bit) in this.
	 * @return Number of unsigned long (32 bit) in this. */
	size_t length() const { return len; }

	/** Returns the bit at n, starting with 0.
	 * @param n The index number ot the bit.
	 * @return The bit value at n, either 0 or 1.*/
	int bit(size_t n) const { return n>=0 && n/32<len? ((num[n/32]>>(n%32))&1) : 0; }

	/** Set bit at n, where n can be any value and this will resize to fit, to value i, that is either 0 or else 1.
	 * @param n The index number of the bit to set.
	 * @param i The value to set n to, either 1 or 0. */
	void setBit(size_t n,int i);
	/** @} */

	/** @name Tests
	 * @{ */
	bool isZero() const { return len==1 && !*num; }			//!< Returns true if this is zero.
	bool isNegative() const { return sign==Minus; }			//!< Returns true if sign of this is set to Minus.
	bool isNaN() const { return sign==NaN; }					//!< Returns true if sign of this is set to NaN.
	bool isInfinite() const { return sign==Infinite; }		//!< Returns true if sign of this is set to Infinite.
	bool isPrime();													//!< Returns true if this is a prime-number.
	bool isSafePrime();												//!< Returns true if this is a safe prime-number.
	bool isEven() { return (*num&1)==0; }						//!< Returns true if this is an even number.
	bool isOdd() { return (*num&1)==1; }						//!< Returns true if this is an odd number.
	/** @} */

	//operator long() const { return sign==Minus? -(long)*num : (long)*num; }
	//operator unsigned long() const { return *num; }
	//operator unsigned long *() const { return num; }
	//operator long long() const;
	//operator unsigned long long() const { return *num|((unsigned long long)num[1]<<32); }

	/** @name Input and Output
	 * @{ */
	/** Print to file.
	 * This method prints the integer as digital numbers.
	 * @param fp File to write to, if ommited is set to stdout. */
	void print(FILE *fp=stdout) const;
	/** @} */
};

}; /* namespace a */


#endif /* _AMANITA_INTEGER_H */


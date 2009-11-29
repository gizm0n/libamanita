#ifndef _LIBAMANITA_AINTEGER_H
#define _LIBAMANITA_AINTEGER_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>


class aInteger {
private:
	enum {
		Plus,			/** < Sign of this is set to Plus if it is a positive number. */
		Minus,		/** < Sign of this is set to Minus if it is a negative number. */
		NaN,			/** < Sign of this is set to Minus if it is not a number. */
		Infinite,	/** < Sign of this is set to Minus if it is an infinite number. */
	};
	uint8_t sign;				/** < Sign tells what type of number this is. */
	uint32_t *num;				/** < Num contains all data of this. */
	size_t len;					/** < Len is the length of the number this is set to in number of 32 bit integers, it is always >= 1. */
	size_t cap;					/** < Cap is the capacity of num, it is always >= 1, and len is always <= cap. */

	void resize(int n);

	void set32(int32_t n);
	void setu32(uint32_t n);
	void set64(int64_t n);
	void setu64(uint64_t n);
	void set(const char *n);
	void set(const aInteger &n);
	void inc(uint32_t n);
	void dec(uint32_t n);
	void add(const aInteger &n);
	void sub(const aInteger &n);
	void mul(const aInteger &n);
	void div(const aInteger &n,aInteger *q,aInteger *r);
	void lshift(size_t n);
	void rshift(size_t n);

	int cmp(int32_t n);
	int cmp(uint32_t n);
	int cmp(int64_t n);
	int cmp(uint64_t n);
	int cmp(const aInteger &n);

	void monpro(const aInteger &n,const aInteger &r);

public:
	aInteger() : num(0) { set((uint32_t)0); }
	aInteger(int32_t n) : num(0) { set32((int32_t)n); }
	aInteger(uint32_t n) : num(0) { setu32((uint32_t)n); }
	aInteger(int64_t n) : num(0) { set64((int64_t)n); }
	aInteger(uint64_t n) : num(0) { setu64((uint64_t)n); }
	aInteger(const char *n) : num(0) { set(n); }
	aInteger(const aInteger &n) : num(0) { set(n); }
	~aInteger();

	aInteger &operator=(int32_t n) { set32(n);return *this; }
	aInteger &operator=(uint32_t n) { setu32(n);return *this; }
	aInteger &operator=(int64_t n) { set64(n);return *this; }
	aInteger &operator=(uint64_t n) { setu64(n);return *this; }
	aInteger &operator=(const char *n) { set(n);return *this; }
	aInteger &operator=(const aInteger &n) { set(n);return *this; }
	aInteger &operator++(int n) { if(sign==Minus) dec(1);else inc(1);return *this; }
	aInteger &operator--(int n) { if(sign==Minus) inc(1);else dec(1);return *this; }
	aInteger operator+(const aInteger &n) { aInteger i(*this);i += n;return i; }
	aInteger &operator+=(int n) { if(n<0) *this -= -n;else if(sign==Minus) dec(n);else inc(n);return *this; }
	aInteger &operator+=(long n) { if(n<0) *this -= -n;else if(sign==Minus) dec(n);else inc(n);return *this; }
	aInteger &operator+=(unsigned long n) { if(sign==Minus) dec(n);else inc(n);return *this; }
	aInteger &operator+=(const aInteger &n);
	aInteger operator-(const aInteger &n) { aInteger i(*this);i -= n;return i;}
	aInteger &operator-=(int n) { if(n<0) *this += -n;else if(sign==Minus) inc(n);else dec(n);return *this; }
	aInteger &operator-=(long n) { if(n<0) *this += -n;else if(sign==Minus) inc(n);else dec(n);return *this; }
	aInteger &operator-=(unsigned long n) { if(sign==Minus) inc(n);else dec(n);return *this; }
	aInteger &operator-=(const aInteger &n);
	aInteger operator*(const aInteger &n) { aInteger i(*this);i.mul(n);return i; }
	aInteger &operator*=(const aInteger &n) { mul(n);return *this; }
	aInteger operator/(const aInteger &n) { aInteger i(*this);div(n,&i,0);return i; }
	aInteger &operator/=(const aInteger &n) { div(n,this,0);return *this; }
	aInteger operator%(const aInteger &n) { aInteger i(*this);div(n,0,&i);return i; }
	aInteger &operator%=(const aInteger &n) { div(n,0,this);return *this; }
	aInteger operator<<(size_t n) { aInteger i(*this);i.lshift(n);return i; }
	aInteger &operator<<=(size_t n) { lshift(n);return *this; }
	aInteger operator>>(size_t n) { aInteger i(*this);i.rshift(n);return i; }
	aInteger &operator>>=(size_t n) { rshift(n);return *this; }

	bool operator==(int32_t n) { return cmp(n)==0; }
	bool operator==(uint32_t n) { return cmp(n)==0; }
	bool operator==(int64_t n) { return cmp(n)==0; }
	bool operator==(uint64_t n) { return cmp(n)==0; }
	bool operator==(const aInteger &n) { return cmp(n)==0; }
	bool operator!=(int32_t n) { return cmp(n)!=0; }
	bool operator!=(uint32_t n) { return cmp(n)!=0; }
	bool operator!=(int64_t n) { return cmp(n)!=0; }
	bool operator!=(uint64_t n) { return cmp(n)!=0; }
	bool operator!=(const aInteger &n) { return cmp(n)!=0; }
	bool operator<(int32_t n) { return cmp(n)<0; }
	bool operator<(uint32_t n) { return cmp(n)<0; }
	bool operator<(int64_t n) { return cmp(n)<0; }
	bool operator<(uint64_t n) { return cmp(n)<0; }
	bool operator<(const aInteger &n) { return cmp(n)<0; }
	bool operator<=(int32_t n) { return cmp(n)<=0; }
	bool operator<=(uint32_t n) { return cmp(n)<=0; }
	bool operator<=(int64_t n) { return cmp(n)<=0; }
	bool operator<=(uint64_t n) { return cmp(n)<=0; }
	bool operator<=(const aInteger &n) { return cmp(n)<=0; }
	bool operator>(int32_t n) { return cmp(n)>0; }
	bool operator>(uint32_t n) { return cmp(n)>0; }
	bool operator>(int64_t n) { return cmp(n)>0; }
	bool operator>(uint64_t n) { return cmp(n)>0; }
	bool operator>(const aInteger &n) { return cmp(n)>0; }
	bool operator>=(int32_t n) { return cmp(n)>=0; }
	bool operator>=(uint32_t n) { return cmp(n)>=0; }
	bool operator>=(int64_t n) { return cmp(n)>=0; }
	bool operator>=(uint64_t n) { return cmp(n)>=0; }
	bool operator>=(const aInteger &n) { return cmp(n)>=0; }

	/** Sets the aInteger sign to Plus. */
	aInteger &abs() { if(sign==Minus) sign = Plus;return *this; }

	/** Calculates sqrt of the value of this, and sets this to that value. */
	aInteger &sqrt();

	/** Euclidean algorithm.
	 * In number theory, the Euclidean algorithm (also called Euclid's algorithm) is an algorithm to determine
	 * the greatest common divisor (GCD) of two elements of any Euclidean domain (for example, the integers).
	 * Its major significance is that it does not require factoring the two integers, and it is also significant
	 * in that it is one of the oldest algorithms known, dating back to the ancient Greeks.
	 */
	aInteger &gcd(const aInteger &n);

	/** Modular exponentiation. */
	aInteger &modpow(const aInteger &e,const aInteger &m);

	/** Calculates the largest prime factor and sets this to that value. */
	aInteger &factor();

	/** Creates a random integer of exactly n bits. */
	aInteger &random(size_t n);

	/** Returns the log2 of this as an integer floor value. */
	size_t log2() const;

	/** Returns number of bits in this. */
	size_t bits() const;

	/** Returns number of unsigned long (32 bit) in this. */
	size_t length() const { return len; }

	/** Returns the bit at n, starting with 0. */
	int bit(size_t n) const { return n>=0 && n/32<len? ((num[n/32]>>(n%32))&1) : 0; }

	/** Set bit at n, where n can be any value and this will resize to fit, to value i, that is either 0 or else 1. */
	void setBit(size_t n,int i);

	/** Returns if this is zero. */
	bool isZero() const { return len==1 && !*num; }

	/** Returns true if sign of this is set to Minus. */
	bool isNegative() const { return sign==Minus; }

	/** Returns true if sign of this is set to NaN. */
	bool isNaN() const { return sign==NaN; }

	/** Returns true if sign of this is set to Infinite. */
	bool isInfinite() const { return sign==Infinite; }

	/** Returns true if this is a prime-number. */
	bool isPrime();

	/** Returns true if this is a safe prime-number. */
	bool isSafePrime();

	/** Returns true if this is an even number. */
	bool isEven() { return (*num&1)==0; }

	/** returns true if this is an odd number. */
	bool isOdd() { return (*num&1)==1; }

	//operator long() const { return sign==Minus? -(long)*num : (long)*num; }
	//operator unsigned long() const { return *num; }
	//operator unsigned long *() const { return num; }
	//operator long long() const;
	//operator unsigned long long() const { return *num|((unsigned long long)num[1]<<32); }

	void print(FILE *fp=stdout) const;
};


#endif /* _LIBAMANITA_AINTEGER_H */


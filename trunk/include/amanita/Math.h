#ifndef _AMANITA_MATH_H
#define _AMANITA_MATH_H

/**
 * @file amanita/Math.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 


#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/** Amanita Namespace */
namespace a {

/** A collection of mathematical functions.
 * @ingroup amanita
 */

/** @name Constants
 * @{ */
/** The PI constant 3.1415926535897932384626433832795 */
const double PI = 3.1415926535897932384626433832795;
/** The prime number serie from 2 to 499. */
extern const int prime[];
/** The fibonacci number serie from 0 to 1'836'311'903. */
extern const int fibonacci[];
/** @} */


/** @name Square root.
 * @{ */
/** Integer Square Root for unsigned long. */
extern unsigned short sqrt(unsigned long n);
/** Integer Square Root for unsigned long long. */
extern unsigned long sqrt(unsigned long long n);
/** Nth Root for double. */
inline double nrt(double i,double n) { return ::pow(i,1./n); }
/** @} */


/** @name Rounding
 * @{ */
/** Round double d to floor integer. */
inline long floor(double d) { return (long)d; }
/** Round double d to ceil integer. */
extern long ceil(double d);
/** Round double d to closest integer. */
extern long round(double d);
/** @} */


/** @name Minimum and Maximum
 * @{ */
/** Return the smaller of the values. */
inline int minimum(int i1,int i2) { return i1<i2? i1 : i2; }
/** Return the larger of the values. */
inline int maximum(int i1,int i2) { return i1>i2? i1 : i2; }
/** @} */


/** @name Logarithms
 * @{ */
/** Return integer common logarithm (base-10) for n. */
extern int log10(long n);
/** Return integer common logarithm (base-10) for n. */
extern int log10(long long n);
/** @} */


/** @name Geometry
 * @{ */
inline uint32_t pythagoras(int32_t x,int32_t y) { return a::sqrt((uint64_t)(x*x+y*y)); }
inline double pythagoras(double x,double y) { return ::sqrt(x*x+y*y); }
inline double pythagoras2(double kat,double tan) { return ::sqrt(tan*tan-kat*kat); }

extern double angle(double x,double y);

extern int32_t distance(int32_t x1,int32_t y1,int32_t x2,int32_t y2);
extern double distance(double x1,double y1,double x2,double y2);
extern double distance(double x1,double y1,double z1,double x2,double y2,double z2);

inline double circle_area(double radius) { return PI*radius*radius; }
inline double sphere_area(double radius) { return 4.*PI*radius*radius; }
inline double sphere_volume(double radius) { return 4.*PI*radius*radius*radius/3.; }
inline double sphere_radius(double volume) { return a::nrt((volume*3)/(4*PI),3.); }
/** @} */

/** @name Sum
 * @{ */
/** The sum of numbers between (and including) min and max. */
extern long sum(long min,long max);
/** The sum of arr.
 * @param arr An array of chars to be added together.
 * @param l Length of arr.
 * @return A long containing the sum of arr. */
extern long sum(char *arr,long l);
/** The sum of arr.
 * @param arr An array of unsigned chars to be added together.
 * @param l Length of arr.
 * @return An unsigned long containing the sum of arr. */
extern unsigned long sum(unsigned char *arr,long l);
/** The sum of arr.
 * @param arr An array of short to be added together.
 * @param l Length of arr.
 * @return A long containing the sum of arr. */
extern long sum(short *arr,long l);
/** The sum of arr.
 * @param arr An array of unsigned short to be added together.
 * @param l Length of arr.
 * @return An unsigned long containing the sum of arr. */
extern unsigned long sum(unsigned short *arr,long l);
/** The sum of arr.
 * @param arr An array of int to be added together.
 * @param l Length of arr.
 * @return A long containing the sum of arr. */
extern long sum(int *arr,long l);
/** The sum of arr.
 * @param arr An array of unsigned int to be added together.
 * @param l Length of arr.
 * @return An unsigned long containing the sum of arr. */
extern unsigned long sum(unsigned int *arr,long l);
/** The sum of arr.
 * @param arr An array of long to be added together.
 * @param l Length of arr.
 * @return A long containing the sum of arr. */
extern long sum(long *arr,long l);
/** The sum of arr.
 * @param arr An array of unsigned long to be added together.
 * @param l Length of arr.
 * @return An unsigned long containing the sum of arr. */
extern unsigned long sum(unsigned long *arr,long l);
/** The sum of arr.
 * @param arr An array of double to be added together.
 * @param l Length of arr.
 * @return A double containing the sum of arr. */
extern double sum(double *arr,long l);
/** @} */

/** Number of bits in n, or log2 of n, or the left most 1-bit in n.
 * For 0b00101101 it would return 6.
 * @param n A long long
 * @return Number of bits in n */
extern size_t bits(long long n);

/** @name Euclidean
 * @{ */
/** Euclidean algorithm.
 * In number theory, the Euclidean algorithm (also called Euclid's algorithm) is an algorithm to determine
 * the greatest common divisor (GCD) of two elements of any Euclidean domain (for example, the integers).
 * Its major significance is that it does not require factoring the two integers, and it is also significant
 * in that it is one of the oldest algorithms known, dating back to the ancient Greeks. */
extern long gcd(long a,long b);
extern long long gcd(long long a,long long b);

/** Extended Euclidean algorithm (Modular multiplicative inverse).
 * For finding the greatest common divisor (GCD) of integers a and b:
 * it also finds the integers x and y in Bézout's identity. */
extern long gcd(long a,long b,long &x,long &y);
extern long long gcd(long long a,long long b,long long &x,long long &y);
/** @} */

/** @name Exponentiation
 * @{ */
/** Modular exponentiation for long. */
extern long modpow(long b,long e,long m);
/** Modular exponentiation for long long. */
extern long long modpow(long long b,long long e,long long m);
/** @} */

/** @name Prime
 * @{ */
/** Returns the largest prime factor of unsigned long long n. */
extern long long factor(long long n);
/** Test if prime for unsigned long long. */
extern bool isprime(long long n,int a=0,int k=0);
/** Find the previous prime before n. */
extern long long prevprime(long long n);
/** Find the next prime after n. */
extern long long nextprime(long long n);
/** @} */

}; /* namespace a */


using namespace a;


#endif /* _AMANITA_MATH_H */


#ifndef _LIBAMANITA_MATH_H
#define _LIBAMANITA_MATH_H


class Math {
public:
	static const double PI; /** < The PI constant 3.1415926535897932384626433832795 */
	static const int prime[]; /** < The prime number serie from 2 to 499. */
	static const long fibonacci[]; /** < The fibonacci number serie from 0 to 1'836'311'903. */

	static long floor(double d); /** < Round double d to floor integer. */
	static long ceil(double d); /** < Round double d to ceil integer. */
	static long round(double d); /** < Round double d to closest integer. */

	static unsigned long pythagoras(long x,long y);
	static double pythagoras(double x,double y);
	static double pythagoras2(double kat,double tan);
	static double angle(double x,double y);
	static long distance(long x1,long y1,long x2,long y2);
	static double distance(double x1,double y1,double x2,double y2);
	static double distance(double x1,double y1,double z1,double x2,double y2,double z2);
	static double circleArea(double radius) { return PI*radius*radius; }
	static double sphereArea(double radius) { return 4.*PI*radius*radius; }
	static double sphereVolume(double radius) { return 4.*PI*radius*radius*radius/3.; }
	static double radius(double volume) { return nrt((volume*3)/(4*PI),3.); }

	static long sum(long min,long max); /** < The sum of numbers between (and including) min and max. */
	static long sum(char *arr,long l); /** < The sum of l number of doubles in arr. */
	static unsigned long sum(unsigned char *arr,long l); /** < The sum of l number of doubles in arr. */
	static long sum(short *arr,long l); /** < The sum of l number of doubles in arr. */
	static unsigned long sum(unsigned short *arr,long l); /** < The sum of l number of doubles in arr. */
	static long sum(int *arr,long l); /** < The sum of l number of doubles in arr. */
	static unsigned long sum(unsigned int *arr,long l); /** < The sum of l number of doubles in arr. */
	static long sum(long *arr,long l); /** < The sum of l number of doubles in arr. */
	static unsigned long sum(unsigned long *arr,long l); /** < The sum of l number of doubles in arr. */
	static double sum(double *arr,long l); /** < The sum of l number of doubles in arr. */

	static int bits(long long n);

	/** Euclidean algorithm.
	 * In number theory, the Euclidean algorithm (also called Euclid's algorithm) is an algorithm to determine
	 * the greatest common divisor (GCD) of two elements of any Euclidean domain (for example, the integers).
	 * Its major significance is that it does not require factoring the two integers, and it is also significant
	 * in that it is one of the oldest algorithms known, dating back to the ancient Greeks.
	 */
	static long gcd(long a,long b);
	static long long gcd(long long a,long long b);

	/** Extended Euclidean algorithm (Modular multiplicative inverse).
	 * For finding the greatest common divisor (GCD) of integers a and b:
	 * it also finds the integers x and y in Bézout's identity.
	 */
	static long gcd(long a,long b,long &x,long &y);
	static long long gcd(long long a,long long b,long long &x,long long &y);

	/** Modular exponentiation for long. */
	static long modpow(long b,long e,long m);

	/** Modular exponentiation for long long. */
	static long long modpow(long long b,long long e,long long m);

	/** Integer Square Root for unsigned long. */
	static unsigned short sqrt(unsigned long n);

	/** Integer Square Root for unsigned long long. */
	static unsigned long sqrt(unsigned long long n);

	/** Nth Root for double. */
	static double nrt(double i,double n);

	/** Returns the largest prime factor of unsigned long long n. */
	static long long factor(long long n);

	/** Test if prime for unsigned long long. */
	static bool isprime(long long n,int a=0,int k=0);

	static long long lastPrime(long long n);
	static long long nextPrime(long long n);
};


#endif /* _LIBAMANITA_MATH_H */


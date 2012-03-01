
/**
 * Test for the randomness/entropy and speed of the aRandom class.
 **/
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#include <amanita/aRandom.h>


int main(int argc,char *argv[]) {
	const char *file = "./entropy.dat";
	int i,n = 1000000;
	uint32_t r;
	timeval t1,t2,t3,t4;
	double d1,d2;
	FILE *fp = fopen(file,"wb");
	gettimeofday(&t1,0);
	for(i=0; i<n; i++) {
		r = rnd.uint32();
		fwrite(&r,sizeof(uint32_t),1,fp);
	}
	gettimeofday(&t2,0);
	fclose(fp);
	gettimeofday(&t3,0);
	for(i=0; i<n; i++) rnd.uint32();
	gettimeofday(&t4,0);
	d1 = (double)(t2.tv_sec-t1.tv_sec);
	d1 += ((double)(t2.tv_usec-t1.tv_usec))/1000000.0;
	d2 = (double)(t4.tv_sec-t3.tv_sec);
	d2 += ((double)(t4.tv_usec-t3.tv_usec))/1000000.0;
	printf(
		"\n"
		"Pseudo random number generation completed and stored in file \"%s\".\n"
		"Generated %d numbers.\n\n"
		"Time in seconds to write to file was %lf.\n\n"
		"Time in seconds to only generate numbers was %lf.\n"
		"\n",
		file,n,d1,d2
	);
	return 0;
}


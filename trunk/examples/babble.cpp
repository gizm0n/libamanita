
#include <stdio.h>
#include <string.h>
#include <amanita/Word.h>

int main(int argc, char *argv[]) {
	int i,l,n;
	const char *s;
	Word word(
		"vowels: a5 e5 i4 o3 u3 y2\n"
		"vowels2: a5 e5 i4 o3 u3\n"
		"consonants: b3c2d3f2g3hj2k3l2m3n3p3qr3s4t3v3wxz\n"
		"consonants2: b3d3f2g3k3l2m3n3p3r3s4t3v2\n"
		"singles: chqwxz\n"
		"doubles: bdfglmnprstaei\n"
		"unaccepted: bk,bm,bp,bv,dk,fv,gf,gk,kg,pb,pk,pm,pv,sb,tb,td"
	);
	for(i=0,n=0; i<100; ++i) {
		s = word.generate(3,8);
		l = strlen(s);
		printf("%s%c",s,n+l>=40? '\n' : ' ');
		if(n+l>=40) n = 0;
		else n += l;
	}
	putchar('\n');
	return 0;
}

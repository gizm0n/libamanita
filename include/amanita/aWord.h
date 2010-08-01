#ifndef _LIBAMANITA_WORD_H
#define _LIBAMANITA_WORD_H

/**
 * @file libamanita/aWord.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

class aRandom;

class aWord {
protected:
	char *v,*v2,*c,*c2,*s,*d,*u;
	int vl,cl,sl,dl,v2l,c2l,ul;
	aRandom *rnd;
	char *str;
	size_t len;

public:
	static const char *vowels,*vowels2,*consonants,*consonants2,*singles,*doubles,*unaccepted;

	aWord(const char *v=0,const char *v2=0,const char *c=0,const char *c2=0,const char *s=0,
			const char *d=0,const char *u=0);
	~aWord();

	void setSeed(unsigned int n);
	char *generate(int minlen,int maxlen);
};

#endif /* _LIBAMANITA_WORD_H */

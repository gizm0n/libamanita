#ifndef _AMANITA_WORD_H
#define _AMANITA_WORD_H

/**
 * @file amanita/aWord.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

class aRandom;

/** A class for randomly generating words.
 * 
 * This class generates words randomly, that can be used for names even producing pseudo
 * languages. The generated words can be of any chosen length, and tries to mimic natural
 * language so that they can be pronounced, with a normal variation of nouns and vocals.
 * 
 * Any parameters in the generation of words can be set so that specific combinations
 * of letters are produced, to allow for variations between different languages. For
 * example; one language may contain lots of nouns and unusual combinations like "kk",
 * while other languages may use many vocals and vocal combinations like "ae".
 * @ingroup amanita
 */
class aWord {
protected:
	char *v;
	char *v2;
	char *c;
	char *c2;
	char *s;
	char *d;
	char *u;
	int vl;
	int cl;
	int sl;
	int dl;
	int v2l;
	int c2l;
	int ul;
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

#endif /* _AMANITA_WORD_H */

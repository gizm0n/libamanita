#ifndef _AMANITA_AI_WORD_H
#define _AMANITA_AI_WORD_H

/**
 * @file amanita/ai/Word.h  
 * @author Per Löwgren
 * @date Modified: 2013-01-20
 * @date Created: 2008-09-07
 */ 

/** Amanita Namespace */
namespace a {

/** @cond */
class Random;
/** @endcond */

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
 * 
 * @ingroup amanita */
class Word {
protected:
	const char *v;
	const char *v2;
	const char *c;
	const char *c2;
	const char *s;
	const char *d;
	const char *u;
	int vl;
	int cl;
	int sl;
	int dl;
	int v2l;
	int c2l;
	int ul;
	Random *rnd;
	char *str;
	size_t len;

public:
	Word(const char *def=0);
	~Word();

	void setSeed(unsigned int n);
	char *generate(int minlen,int maxlen);
};

}; /* namespace a */


#endif /* _AMANITA_AI_WORD_H */

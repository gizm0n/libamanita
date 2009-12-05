#ifndef _LIBAMANITA_DICTIONARY_H
#define _LIBAMANITA_DICTIONARY_H

/**
 * @file libamanita/aDictionary.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <libamanita/aCollection.h>


/** A quick lookup class.
 * 
 * The aDictionary class generated an indexed list of words with attached numerical values.
 * @ingroup libamanita
 */
class aDictionary : public aCollection {
RttiObjectInstance(aDictionary)

public:
	struct word {
		const char *key;
		value_t value;
		size_t len;
	};

private:
	struct node {
		size_t offset;
		size_t range;
	};
	word *words;
	word **values;
	node k_index[256];

protected:
	void clear();
	void createIndex();
	static int compareKey(const void *w1, const void *w2);
	static int compareValue(const void *w1, const void *w2);

public:
	aDictionary() : aCollection(),words(0) {}
	aDictionary(const char **ws,const value_t *vs,size_t l=0,bool c=false);
	aDictionary(const word *ws,size_t l=0,bool c=false);
	~aDictionary();

	void createIndex(const char **ws,const value_t *vs,size_t l=0,bool c=false);
	void createIndex(const word *w,size_t l=0,bool c=false);
	value_t getValue(const char *w,size_t l=0);
	const char *getKey(value_t v);
};

#endif /* _LIBAMANITA_DICTIONARY_H */

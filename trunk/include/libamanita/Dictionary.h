#ifndef _LIBAMANITA_DICTIONARY_H
#define _LIBAMANITA_DICTIONARY_H

#include <libamanita/Collection.h>


class Dictionary : public Collection {
RttiObjectInstance(Dictionary)

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
	Dictionary() : Collection(),words(0) {}
	Dictionary(const char **ws,const value_t *vs,size_t l=0,bool c=false);
	Dictionary(const word *ws,size_t l=0,bool c=false);
	~Dictionary();

	void createIndex(const char **ws,const value_t *vs,size_t l=0,bool c=false);
	void createIndex(const word *w,size_t l=0,bool c=false);
	value_t getValue(const char *w,size_t l=0);
	const char *getKey(value_t v);
};

#endif /* _LIBAMANITA_DICTIONARY_H */


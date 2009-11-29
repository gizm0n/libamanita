#ifndef _LIBAMANITA_AITERATOR_H
#define _LIBAMANITA_AITERATOR_H


class aIterator {
public:
	virtual bool hasMoreElements() = 0;
	virtual void reset() = 0;
	virtual void *first() = 0;
	virtual void *next() = 0;
	virtual void *previous() = 0;
	virtual void *last() = 0;
	virtual void *remove() = 0;
};

#endif /* _LIBAMANITA_AITERATOR_H */


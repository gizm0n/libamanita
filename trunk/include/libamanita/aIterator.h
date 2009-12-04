#ifndef _LIBAMANITA_ITERATOR_H
#define _LIBAMANITA_ITERATOR_H

/**
 * @file libamanita/aIterator.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 


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

#endif /* _LIBAMANITA_ITERATOR_H */


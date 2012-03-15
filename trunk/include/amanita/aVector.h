#ifndef _AMANITA_VECTOR_H
#define _AMANITA_VECTOR_H

/**
 * @file amanita/aVector.h  
 * @author Per Löwgren
 * @date Modified: 2012-03-11
 * @date Created: 2004-03-22
 */ 

#include <stdint.h>
#include <amanita/aCollection.h>
#include <amanita/aObject.h>
#include <amanita/aString.h>

/** @cond */
class aHashtable;
/** @endcond */


/** A generic multitype vector.
 * 
 * This vector differs from the standard C++ vector in that it isn't build with templates,
 * and instead handles any of the build in C types, and the aString and aObject of libamanita.
 * 
 * There is an almost infinite collection of overloaded methods in this class. This is to ensure
 * that all types get accepted during compiletime. All integers, 8, 16, and 32 bits are stored as
 * such, and in 64 bit systems, also 64 bit integers are stored so.
 * Floats are converted to int like this *(value_t *)((void *)&f) to make sure that the same
 * data in the float is stored in the table.
 * 
 * All these overloaded methods are actually inline methods, most of them, so no need for panic,
 * the compiled file is not very large, and there is very little overhead to handle all these
 * different types in reality.
 * @ingroup amanita */
class aVector : public aCollection {
friend class aHashtable;

/** @cond */
aObject_Instance(aVector)
/** @endcond */

private:
	struct node {
		value_t value;
		type_t type;
	};
	size_t inc;
	node *list;

	size_t insert(long n,value_t v,type_t t);
	void set(long n,value_t v,type_t t);
	size_t remove(value_t v,type_t t);
	long find(value_t v,type_t t);

public:
	/** iterator: Iterator class for aVector.
	 * This class is used to iterate through the values of the vector. */
	class iterator {
	friend class aVector;

	private:
		aVector *vec;		//!< The vector this iterator is iterating through.
		long index;			//!< Index where the iterator is at.

		/** Constructor
		 * @param vec aVector to iterate. */
		iterator(aVector *vec) : vec(vec) { reset(); }

	public:
		/** Reset iterator to startpoint. */
		void reset() { index = vec && vec->sz? ITER_INDEX_START : ITER_INDEX_EMPTY; }

		/** Set iterator to first element.
		 * @param type Specify the first element of type. Optional.
		 * @return Value. */
		value_t first(type_t type=TYPE_EMPTY);

		/** Set iterator to next element.
		 * @param type Specify the next element of type. Optional.
		 * @return Value. */
		value_t next(type_t type=TYPE_EMPTY);

		/** Set iterator to previous element.
		 * @param type Specify the previous element of type. Optional.
		 * @return Value.*/
		value_t previous(type_t type=TYPE_EMPTY);

		/** Set iterator to last element.
		 * @param type Specify the last element of type. Optional.
		 * @return Value. */
		value_t last(type_t type=TYPE_EMPTY);

		/** Remove element at iterator's index.
		 * @return Value of the removed element. */
		value_t remove();

		/** Get the value at where the index is.
		 * @return Value. */
		value_t value() { return index>=0? vec->list[index].value : 0; }

		/** Get the type of the value at where the index is.
		 * @return Type of the value. */
		value_t type() { return index>=0? vec->list[index].type : TYPE_EMPTY; }

		/** Iterate forward.
		 * @see next()
		 * @return Reference to this iterator. */
		iterator &operator++() { next(TYPE_EMPTY);return *this; }

		/** Iterate backward.
		 * @see previous()
		 * @return Reference to this iterator. */
		iterator &operator--() { previous(TYPE_EMPTY);return *this; }

		/** Compare with other iterator if they point at same index in the same vector.
		 * @param iter An iterator.
		 * @return Return true if equal. */
		bool operator==(const iterator &iter) { return vec==iter.vec && index==iter.index; }

		/** Compare with other iterator if they don't point at same index in the same vector.
		 * @param iter An iterator.
		 * @return Return true if not equal. */
		bool operator!=(const iterator &iter) { return vec!=iter.vec || index!=iter.index; }

		/** Get the value at where iterator points as a pointer to void.
		 * @return Pointer to void. */
		operator void *() const { return index>=0? (void *)vec->list[index].value : 0; }
	};

	aVector();
	aVector(size_t s);
	virtual ~aVector();

	iterator iterate();

	/** @name Get Operators
	 * @{ */
	value_t operator[](char n) { return sz && n<(long)sz && (long)sz+n>=0? list[n<0? (long)sz+n : (long)n].value : 0; }
	value_t operator[](unsigned char n) { return sz && n<sz? list[n].value : 0; }
	value_t operator[](short n) {  return sz && n<(long)sz && (long)sz+n>=0? list[n<0? (long)sz+n : (long)n].value : 0; }
	value_t operator[](unsigned short n) { return sz && n<sz? list[n].value : 0; }
	value_t operator[](int n) {  return sz && n<(long)sz && (long)sz+n>=0? list[n<0? (long)sz+n : (long)n].value : 0; }
	value_t operator[](unsigned int n) { return sz && n<sz? list[n].value : 0; }
	value_t operator[](long n) {  return sz && n<(long)sz && (long)sz+n>=0? list[n<0? (long)sz+n : (long)n].value : 0; }
	value_t operator[](unsigned long n) { return sz && n<sz? list[n].value : 0; }
	/** @} */

	/** @name Insert operator
	 * @{ */
	aVector &operator<<(void *v) { insert(sz,(value_t)v,TYPE_VOID_P);return *this; }
	aVector &operator<<(long v) { insert(sz,(value_t)v,TYPE_INTPTR);return *this; }
	aVector &operator<<(unsigned long v) { insert(sz,(value_t)v,TYPE_INTPTR);return *this; }
#if _WORDSIZE == 64
	aVector &operator<<(float v) { double d = v;insert(sz,*(value_t *)((void *)&d),TYPE_DOUBLE);return *this; }
#else
	aVector &operator<<(float v) { insert(sz,*(value_t *)((void *)&v),TYPE_FLOAT);return *this; }
#endif
	aVector &operator<<(double v) { insert(sz,*(value_t *)((void *)&v),TYPE_DOUBLE);return *this; }
	aVector &operator<<(const char *v) { insert(sz,(value_t)v,TYPE_CHAR_P);return *this; }
	aVector &operator<<(const aString *v) { insert(sz,(value_t)v->str,TYPE_CHAR_P);return *this; }
	aVector &operator<<(const aString &v) { insert(sz,(value_t)v.str,TYPE_CHAR_P);return *this; }
	aVector &operator<<(const aObject *v) { insert(sz,(value_t)v,TYPE_OBJECT_P);return *this; }
	aVector &operator<<(const aObject &v) { insert(sz,(value_t)&v,TYPE_OBJECT_P);return *this; }
	aVector &operator<<(const aVector &a) { insert(sz,a);return *this; }
	/** @} */

	/** @name Remove operators
	 * @{ */
	aVector &operator>>(void *v) { remove((value_t)v,TYPE_VOID_P);return *this; }
	aVector &operator>>(long v) { remove((value_t)v,TYPE_INTPTR);return *this; }
	aVector &operator>>(unsigned long v) { remove((value_t)v,TYPE_INTPTR);return *this; }
#if _WORDSIZE == 64
	aVector &operator>>(float v) { double d = v;remove(*(value_t *)((void *)&d),TYPE_DOUBLE);return *this; }
#else
	aVector &operator>>(float v) { remove(*(value_t *)((void *)&v),TYPE_FLOAT);return *this; }
#endif
	aVector &operator>>(double v) { remove(*(value_t *)((void *)&v),TYPE_DOUBLE);return *this; }
	aVector &operator>>(const char *v) { remove(v);return *this; }
	aVector &operator>>(const aString *v) { remove(v->str);return *this; }
	aVector &operator>>(const aString &v) { remove(v.str);return *this; }
	aVector &operator>>(const aObject *v) { remove((value_t)v,TYPE_OBJECT_P);return *this; }
	aVector &operator>>(const aObject &v) { remove((value_t)&v,TYPE_OBJECT_P);return *this; }
	/** @} */

	/** @name Insert methods
	 * @{ */
	size_t insert(long n,void *v) { return insert(n,(value_t)v,TYPE_VOID_P); }
	size_t insert(long n,long v) { return insert(n,(value_t)v,TYPE_INTPTR); }
	size_t insert(long n,unsigned long v) { return insert(n,(value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	size_t insert(long n,float v) { double d = v;return insert(n,*(value_t *)((void *)&d),TYPE_DOUBLE); }
#else
	size_t insert(long n,float v) { return insert(n,*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	size_t insert(long n,double v) { return insert(n,*(value_t *)((void *)&v),TYPE_DOUBLE); }
	size_t insert(long n,const char *v) { return insert(n,(value_t)v,TYPE_CHAR_P); }
	size_t insert(long n,const aString *v) { return insert(n,(value_t)v->str,TYPE_CHAR_P); }
	size_t insert(long n,const aString &v) { return insert(n,(value_t)v.str,TYPE_CHAR_P); }
	size_t insert(long n,const aObject *v) { return insert(n,(value_t)v,TYPE_OBJECT_P); }
	size_t insert(long n,const aObject &v) { return insert(n,(value_t)&v,TYPE_OBJECT_P); }
	size_t insert(long n,const aVector &v);
	/** @} */

	/** @name Set methods
	 * @{ */
	void set(long n,void *v) { return set(n,(value_t)v,TYPE_VOID_P); }
	void set(long n,long v) { return set(n,(value_t)v,TYPE_INTPTR); }
	void set(long n,unsigned long v) { return set(n,(value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	void set(long n,float v) { double d = v;return set(n,*(value_t *)((void *)&d),TYPE_DOUBLE); }
#else
	void set(long n,float v) { return set(n,*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	void set(long n,double v) { return set(n,*(value_t *)((void *)&v),TYPE_DOUBLE); }
	void set(long n,const char *v) { return set(n,(value_t)v,TYPE_CHAR_P); }
	void set(long n,const aString *v) { return set(n,(value_t)v->str,TYPE_CHAR_P); }
	void set(long n,const aString &v) { return set(n,(value_t)v.str,TYPE_CHAR_P); }
	void set(long n,const aObject *v) { return set(n,(value_t)v,TYPE_OBJECT_P); }
	void set(long n,const aObject &v) { return set(n,(value_t)&v,TYPE_OBJECT_P); }
	/** @} */

	/** @name Remove methods
	 * @{ */
	size_t remove(void *v) { return remove((value_t)v,TYPE_VOID_P); }
	size_t remove(long v) { return remove((value_t)v,TYPE_INTPTR); }
	size_t remove(unsigned long v) { return remove((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	size_t remove(float v) { double d = v;return remove(*(value_t *)((void *)&d),TYPE_DOUBLE); }
#else
	size_t remove(float v) { return remove(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	size_t remove(double v) { return remove(*(value_t *)((void *)&v),TYPE_DOUBLE); }
	size_t remove(const char *v);
	size_t remove(const aString *v) { return remove(v->str); }
	size_t remove(const aString &v) { return remove(v.str); }
	size_t remove(const aObject *v) { return remove((value_t)v,TYPE_OBJECT_P); }
	size_t remove(const aObject &v) { return remove((value_t)&v,TYPE_OBJECT_P); }

	void removeAt(long n);
	void removeAll() { clear(); }
	/** @} */

	/** @name Find functions
	 * @{ */
	long find(void *v) { return find((value_t)v,TYPE_VOID_P); }
	long find(long v) { return find((value_t)v,TYPE_INTPTR); }
	long find(unsigned long v) { return find((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	long find(float v) { double d = v;return find(*(value_t *)((void *)&d),TYPE_DOUBLE); }
#else
	long find(float v) { return find(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	long find(double v) { return find(*(value_t *)((void *)&v),TYPE_DOUBLE); }
	long find(const char *v);
	long find(const aString *v) { return find(v->str); }
	long find(const aString &v) { return find(v.str); }
	long find(const aObject *v) { return find((value_t)v,TYPE_OBJECT_P); }
	long find(const aObject &v) { return find((value_t)&v,TYPE_OBJECT_P); }
	/** @} */

	/** @name Contains
	 * @{ */
	bool contains(void *v) { return find((value_t)v,TYPE_VOID_P)!=-1; }
	bool contains(long v) { return find((value_t)v,TYPE_INTPTR)!=-1; }
	bool contains(unsigned long v) { return find((value_t)v,TYPE_INTPTR)!=-1; }
#if _WORDSIZE == 64
	bool contains(float v) { double d = v;return find(*(value_t *)((void *)&d),TYPE_DOUBLE)!=-1; }
#else
	bool contains(float v) { return find(*(value_t *)((void *)&v),TYPE_FLOAT)!=-1; }
#endif
	bool contains(double v) { return find(*(value_t *)((void *)&v),TYPE_DOUBLE)!=-1; }
	bool contains(const char *v) { return find(v)!=-1; }
	bool contains(const aString *v) { return find(v->str)!=-1; }
	bool contains(const aString &v) { return find(v.str)!=-1; }
	bool contains(const aObject *v) { return find((value_t)v,TYPE_OBJECT_P)!=-1; }
	bool contains(const aObject &v) { return find((value_t)&v,TYPE_OBJECT_P)!=-1; }
	/** @} */

	/** @name Split
	 * @{ */
	size_t split(const aString &str,const char *delim,bool trim=false) { return split(str.str,delim,trim); }
	size_t split(const char *str,const char *delim,bool trim=false);
	/** @} */

	/** @name Administrative methods
	 * @{ */
	void resize(size_t s=0);
	void trim();
	void clear();
	/** @} */

	/** @name Input and Output methods
	 * @{ */
	size_t print(const char *fn);
	size_t print(FILE *fp);
	size_t load(const char *fn);
	size_t load(FILE *fp);
	size_t save(const char *fn);
	size_t save(FILE *fp);
	/** @} */

	/** @name Static functions for handling of arrays.
	 * @{ */
	/** Search through a null-terminated array arr for string s at offset o until l, and return index or -1 if not found. */
	static long find(const char *arr[],const char *s,long o=0,long l=0);
	/** @} */
};

#endif /* _AMANITA_VECTOR_H */




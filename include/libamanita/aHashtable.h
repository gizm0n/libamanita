#ifndef _LIBAMANITA_HASHTABLE_H
#define _LIBAMANITA_HASHTABLE_H

/**
 * @file libamanita/aHashtable.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2004-09-30
 */ 

#include <string.h>
#include <stdlib.h>
#include <libamanita/aCollection.h>
#include <libamanita/aObject.h>
#include <libamanita/aString.h>


enum {
	HASH_STYLE_CASE_SENSITIVE		= 0,			//!< 
	HASH_STYLE_CASE_INSENSITIVE	= 1,			//!< 
	HASH_STYLE_KEY_MULTIPLES		= 2,			//!< 
};

/** @cond */
#define _HT_SET_INT_VALUE(v,t) {if(at) at->value = (value_t)v,at->v_type = t;}
#define _HT_SET_FLOAT_VALUE(v,t) {if(at) at->value = *(value_t *)((void *)&v),at->v_type = t;}
#define _HT_SET_STRING_VALUE(v,t) {if(at) at->value = (value_t)strdup(v),at->v_type = t;}

class aVector;
/** @endcond */


/** A generic multitype hashtable.
 * 
 * This hashtable differs from the standard C++ hashtable in that it isn't build with templates,
 * and instead handles any of the build in C types, and the aString and aObject of libamanita.
 * 
 * There is an almost infinite collection of overloaded methods in this class. This is to ensure
 * that all types get accepted during compiletime. All integers, 8, 16, and 32 bits are stored as
 * such, both for keys and values, and in 64 bit systems, also 64 bit integers are stored so.
 * Floats are converted to int like this <tt>*(value_t *)((void *)&f)</tt> to make sure that the
 * same data in the float is stored in the table.
 * 
 * All these overloaded methods are actually inline methods, most of them, so no need for panic,
 * the compiled file is not very large, and there is very little overhead to handle all these
 * different types in reality.
 * @ingroup libamanita
 */
class aHashtable : public aCollection {
friend class iterator;

/** @cond */
RttiObjectInstance(aHashtable)
/** @endcond */

protected:
	struct node {
		value_t key;
		hash_t hash;
		value_t value;
		type_t k_type,v_type;
		node *next;
		node(value_t k,hash_t h,value_t v,type_t kt,type_t vt,node *n)
			: key(k),hash(h),value(v),k_type(kt),v_type(vt),next(n) {}
		~node() {
			if(k_type==_TYPE_CHAR_P) free((char *)key);
			else if(k_type==_TYPE_DOUBLE) free((double *)key);
			if(v_type==_TYPE_CHAR_P) free((char *)value);
			else if(v_type==_TYPE_DOUBLE) free((double *)value);
		}
	};
	node **table;
	size_t full;
	style_t style;
	float ld;

	/** @name Protected put methods
	 * @{ */
	void put(value_t key,value_t value,type_t kt,type_t vt);
	void put(const char *key,value_t value,type_t vt);
	/** @} */

	/** @name Protected get methods
	 * @{ */
	value_t get(value_t key,type_t kt);
	value_t get(value_t key,type_t kt,type_t &type);
	size_t get(value_t key,type_t kt,aVector &v);
	/** @} */

	/** @name Protected remove methods
	 * @{ */
	value_t remove(value_t key,type_t kt);
	value_t remove(value_t key,type_t kt,type_t &type);
	/** @} */

	/** Rehash the entire table.
	 * This is done automatically when the hashtable reach a certain fullness compared to
	 * it's capacity, i.e. it's load. The load is defined by the ld-member and can be set
	 * manually. Default value of ld is 0.5, meaning it doubles in size when it grows
	 * (cap/ld).
	 * @param st Rehashing style. */
	void rehash(style_t st);

public:
	/** Iterator class for aHashtable.
	 * This class is used to iterate through the values of the hashtable. */
	class iterator {
	friend class aHashtable;

	private:
		aHashtable *ht;		//!< The hashtable this iterator is iterating through.
		node *at;				//!< Node iterator is pointing at.
		long index;				//!< Index in table iterator is pointing at.

		/** Constructor.
		 * Only used by aHashtable.
		 * @param ht The hashtable. */
		iterator(aHashtable *ht) : ht(ht) { reset(); }

	public:
		/** Constructor. */
		iterator() : ht(0),at(0),index(ITER_INDEX_EMPTY) {}

		/** Reset iterator to startpoint. */
		void reset() { at = 0,index = ht && ht->sz? ITER_INDEX_START : ITER_INDEX_EMPTY; }

		/** Set iterator to first element.
		 * @return Value. */
		value_t first() { return first(_TYPE_EMPTY,_TYPE_EMPTY); }

		/** Set iterator to first element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t first(type_t kt,type_t vt=_TYPE_EMPTY);

		/** Set iterator to next element.
		 * @return Value. */
		value_t next();

		/** Set iterator to next element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t next(type_t kt,type_t vt=_TYPE_EMPTY);

		/** Set iterator to previous element.
		 * @return Value. */
		value_t previous() { return previous(_TYPE_EMPTY,_TYPE_EMPTY); }

		/** Set iterator to previous element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t previous(type_t kt,type_t vt=_TYPE_EMPTY);

		/** Set iterator to last element.
		 * @return Value. */
		value_t last() { return last(_TYPE_EMPTY,_TYPE_EMPTY); }

		/** Set iterator to last element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t last(type_t kt,type_t vt=_TYPE_EMPTY);

		/** Remove element at iterator's index.
		 * @return Value of the removed element. */
		value_t remove();

		/** Get the key at where the index is.
		 * @return Value. */
		value_t key() { return at? at->key : 0; }

		/** Get the type of the key at where the index is.
		 * @return Type of the value. */
		type_t keyType() { return at? at->k_type : _TYPE_EMPTY; }

		/** Get the value at where the index is.
		 * @return Value. */
		value_t value() { return at? at->value : 0; }

		/** Get the type of the value at where the index is.
		 * @return Type of the value. */
		type_t valueType() { return at? at->v_type : _TYPE_EMPTY; }

		/** @name Set value methods
		 * These methods set value to the element at where the iterator is pointing.
		 * @{ */
		void setValue(void *v) { _HT_SET_INT_VALUE(v,_TYPE_VOID_P); }
		void setValue(char v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(unsigned char v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(short v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(unsigned short v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(int v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(unsigned int v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(long v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(unsigned long v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
#if _WORDSIZE == 64
		void setValue(long long v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(unsigned long long v) { _HT_SET_INT_VALUE(v,_TYPE_INTPTR); }
		void setValue(float v) { double d = (double)v;_HT_SET_FLOAT_VALUE(d,_TYPE_DOUBLE); }
		void setValue(double v) { _HT_SET_FLOAT_VALUE(v,_TYPE_DOUBLE); }
#else
		//void setValue(long long v);
		//void setValue(unsigned long long v);
		void setValue(float v) { _HT_SET_FLOAT_VALUE(v,_TYPE_FLOAT); }
		//void setValue(double v);
#endif
		//void setValue(long double v);
		void setValue(const char *v) { _HT_SET_STRING_VALUE(v,_TYPE_CHAR_P); }
		void setValue(aString *v) { _HT_SET_STRING_VALUE(v->toCharArray(),_TYPE_CHAR_P); }
		void setValue(aString &v) { _HT_SET_STRING_VALUE(v.toCharArray(),_TYPE_CHAR_P); }
		void setValue(const aObject *v) { _HT_SET_INT_VALUE(v,_TYPE_OBJECT_P); }
		void setValue(const aObject &v) { _HT_SET_INT_VALUE(&v,_TYPE_OBJECT_P); }
		/** @} */

		/** @name Operators
		 * @{ */
		/** Iterate forward.
		 * @see next()
		 * @return Reference to this iterator. */
		iterator &operator++() { next(0);return *this; }

		/** Iterate backward.
		 * @see previous()
		 * @return Reference to this iterator. */
		iterator &operator--() { previous(0);return *this; }

		/** Compare with other iterator if they point at same index in the same hashtable.
		 * @param iter An iterator.
		 * @return Return true if equal. */
		bool operator==(const iterator &iter) { return at==iter.at; }

		/** Compare with other iterator if they don't point at same index in the same hashtable.
		 * @param iter An iterator.
		 * @return Return true if not equal. */
		bool operator!=(const iterator &iter) { return at!=iter.at; }
		//operator void *() const { return at? (void *)at->value : 0; }
		//operator long() const { return at && at->v_type==_TYPE_INTPTR? (long)at->value : 0; }
		//operator unsigned long() const { return at && at->v_type==_TYPE_INTPTR? (unsigned long)at->value : 0; }
		//operator char *() const { return at && at->v_type==_TYPE_CHAR_P? (char *)at->value : 0; }
		/** @} */
	};

	/** @name Constructos and Destructors
	 * @{ */
	aHashtable(size_t c=11,float l=0.5f,style_t st=0);
	~aHashtable() { removeAll(); }
	/** @} */

	iterator iterate();

	void setCaseInsensitiveKeys(bool b);
	void setAllowKeyMultiples(bool b);

	/** Generate hash value from a char string.
	 * @param key KEy from which to generate hash.
	 * @return Hashvalue. */
	hash_t hash(const char *key);



	/** @name Put methods
	 * These methods puts a value into the table using the given key. They are all inline methods
	 * making the correct type cast to store values in the table. At present time double and long long
	 * and long double are not supported on 32 bit processors and operating systems because of storage
	 * complications. Those can be stored as char arrays and compared with memcmp, but it has to be
	 * implemeneted also :) So far it really hasn't been a need for it yet.
	 * 
	 * Only put-methods with pointer to void as value are documented, but for every type of key there
	 * are methods for all types of values. All together, there are 308 put-methods, and only two of them
	 * are not inline.
	 * @{ */
	void put(void *key,void *value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_VOID_P); }
	/** @cond */
	void put(void *key,char value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,short value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,int value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,long value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(void *key,long long value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
	void put(void *key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_INTPTR); }
#endif
	void put(void *key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_VOID_P,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(void *key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_VOID_P,_TYPE_DOUBLE); }
#endif
	void put(void *key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_VOID_P,_TYPE_CHAR_P); }
	void put(void *key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_VOID_P,_TYPE_CHAR_P); }
	void put(void *key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_VOID_P,_TYPE_CHAR_P); }
	void put(void *key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_VOID_P,_TYPE_OBJECT_P); }
	void put(void *key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_VOID_P,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(char key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(char key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(char key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(char key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(char key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(char key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(char key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(char key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(char key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(char key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(char key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(unsigned char key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(unsigned char key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned char key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned char key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(unsigned char key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned charkey,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(unsigned char key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned char key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned char key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned char key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(unsigned char key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(short key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(short key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(short key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(short key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(short key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(short key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(short key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(short key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(short key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(short key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(short key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(unsigned short key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(unsigned short key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned short key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned short key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(unsigned short key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned short key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(unsigned short key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned short key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned short key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned short key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(unsigned short key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(int key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(int key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(int key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(int key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(int key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(int key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(int key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(int key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(int key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(int key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(int key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(unsigned int key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(unsigned int key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned int key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned int key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(unsigned int key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned int key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(unsigned int key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned int key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned int key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned int key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(unsigned int key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(long key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(long key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(long key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(long key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(long key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(long key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(long key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(long key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(unsigned long key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(unsigned long key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned long key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
#endif
	void put(unsigned long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
#endif
	void put(unsigned long key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned long key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned long key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned long key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(unsigned long key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */


#if _WORDSIZE == 64
	void put(long long key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(long long key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(long long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
	void put(long long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
	void put(long long key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(long long key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(long long key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(long long key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(long long key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(unsigned long long key,void *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_VOID_P); }
	/** @cond */
	void put(unsigned long long key,char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,unsigned char value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,unsigned short value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,unsigned int value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,unsigned long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,unsigned long long value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_INTPTR); }
	void put(unsigned long long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_FLOAT); }
	void put(unsigned long long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_TYPE_INTPTR,_TYPE_DOUBLE); }
	void put(unsigned long long key,const char *value) { put((value_t)key,(value_t)strdup(value),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned long long key,aString *value) { put((value_t)key,(value_t)strdup(value->toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned long long key,aString &value) { put((value_t)key,(value_t)strdup(value.toCharArray()),_TYPE_INTPTR,_TYPE_CHAR_P); }
	void put(unsigned long long key,const aObject *value) { put((value_t)key,(value_t)value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	void put(unsigned long long key,const aObject &value) { put((value_t)key,(value_t)&value,_TYPE_INTPTR,_TYPE_OBJECT_P); }
	/** @endcond */
#endif


	void put(float key,void *value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_VOID_P); }
	/** @cond */
	void put(float key,char value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,unsigned char value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,short value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,unsigned short value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,int value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,unsigned int value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,long value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,unsigned long value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(float key,long long value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
	void put(float key,unsigned long long value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_INTPTR); }
#endif
	void put(float key,float value) { put(*(value_t *)((void *)&key),*(value_t *)((void *)&value),_TYPE_FLOAT,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(float key,double value) { put(*(value_t *)((void *)&key),*(value_t *)((void *)&value),_TYPE_FLOAT,_TYPE_DOUBLE); }
#endif
	void put(float key,const char *value) { put(*(value_t *)((void *)&key),(value_t)strdup(value),_TYPE_FLOAT,_TYPE_CHAR_P); }
	void put(float key,aString *value) { put(*(value_t *)((void *)&key),(value_t)strdup(value->toCharArray()),_TYPE_FLOAT,_TYPE_CHAR_P); }
	void put(float key,aString &value) { put(*(value_t *)((void *)&key),(value_t)strdup(value.toCharArray()),_TYPE_FLOAT,_TYPE_CHAR_P); }
	void put(float key,const aObject *value) { put(*(value_t *)((void *)&key),(value_t)value,_TYPE_FLOAT,_TYPE_OBJECT_P); }
	void put(float key,const aObject &value) { put(*(value_t *)((void *)&key),(value_t)&value,_TYPE_FLOAT,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(const char *key,void *value) { put(key,(value_t)value,_TYPE_VOID_P); }
	/** @cond */
	void put(const char *key,char value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,unsigned char value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,short value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,unsigned short value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,int value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,unsigned int value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,long value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,unsigned long value) { put(key,(value_t)value,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(const char *key,long long value) { put(key,(value_t)value,_TYPE_INTPTR); }
	void put(const char *key,unsigned long long value) { put(key,(value_t)value,_TYPE_INTPTR); }
#endif
	void put(const char *key,float value) { put(key,*(value_t *)((void *)&value),_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(const char *key,double value) { put(key,*(value_t *)((void *)&value),_TYPE_DOUBLE); }
#endif
	void put(const char *key,const char *value) { put(key,(value_t)strdup(value),_TYPE_CHAR_P); }
	void put(const char *key,aString *value) { put(key,(value_t)strdup(value->toCharArray()),_TYPE_CHAR_P); }
	void put(const char *key,aString &value) { put(key,(value_t)strdup(value.toCharArray()),_TYPE_CHAR_P); }
	void put(const char *key,const aObject *value) { put(key,(value_t)value,_TYPE_OBJECT_P); }
	void put(const char *key,const aObject &value) { put(key,(value_t)&value,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(aString *key,void *value) { put(key->toCharArray(),(value_t)value,_TYPE_VOID_P); }
	/** @cond */
	void put(aString *key,char value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,unsigned char value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,short value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,unsigned short value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,int value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,unsigned int value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,long value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,unsigned long value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(aString *key,long long value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString *key,unsigned long long value) { put(key->toCharArray(),(value_t)value,_TYPE_INTPTR); }
#endif
	void put(aString *key,float value) { put(key->toCharArray(),*(value_t *)((void *)&value),_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(aString *key,double value) { put(key->toCharArray(),*(value_t *)((void *)&value),_TYPE_DOUBLE); }
#endif
	void put(aString *key,const char *value) { put(key->toCharArray(),(value_t)strdup(value),_TYPE_CHAR_P); }
	void put(aString *key,aString *value) { put(key->toCharArray(),(value_t)strdup(value->toCharArray()),_TYPE_CHAR_P); }
	void put(aString *key,aString &value) { put(key->toCharArray(),(value_t)strdup(value.toCharArray()),_TYPE_CHAR_P); }
	void put(aString *key,const aObject *value) { put(key->toCharArray(),(value_t)value,_TYPE_OBJECT_P); }
	void put(aString *key,const aObject &value) { put(key->toCharArray(),(value_t)&value,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(aString &key,void *value) { put(key.toCharArray(),(value_t)value,_TYPE_VOID_P); }
	/** @cond */
	void put(aString &key,char value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,unsigned char value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,short value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,unsigned short value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,int value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,unsigned int value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,long value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,unsigned long value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(aString &key,long long value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
	void put(aString &key,unsigned long long value) { put(key.toCharArray(),(value_t)value,_TYPE_INTPTR); }
#endif
	void put(aString &key,float value) { put(key.toCharArray(),*(value_t *)((void *)&value),_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(aString &key,double value) { put(key.toCharArray(),*(value_t *)((void *)&value),_TYPE_DOUBLE); }
#endif
	void put(aString &key,const char *value) { put(key.toCharArray(),(value_t)strdup(value),_TYPE_CHAR_P); }
	void put(aString &key,aString *value) { put(key.toCharArray(),(value_t)strdup(value->toCharArray()),_TYPE_CHAR_P); }
	void put(aString &key,aString &value) { put(key.toCharArray(),(value_t)strdup(value.toCharArray()),_TYPE_CHAR_P); }
	void put(aString &key,const aObject *value) { put(key.toCharArray(),(value_t)value,_TYPE_OBJECT_P); }
	void put(aString &key,const aObject &value) { put(key.toCharArray(),(value_t)&value,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(aObject *key,void *value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_VOID_P); }
	/** @cond */
	void put(aObject *key,char value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,unsigned char value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,short value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,unsigned short value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,int value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,unsigned int value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,long value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,unsigned long value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(aObject *key,long long value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject *key,unsigned long long value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
#endif
	void put(aObject *key,float value) { put((value_t)key->hash(),*(value_t *)((void *)&value),_TYPE_OBJECT_P,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(aObject *key,double value) { put((value_t)key->hash(),*(value_t *)((void *)&value),_TYPE_OBJECT_P,_TYPE_DOUBLE); }
#endif
	void put(aObject *key,const char *value) { put((value_t)key->hash(),(value_t)strdup(value),_TYPE_OBJECT_P,_TYPE_CHAR_P); }
	void put(aObject *key,aString *value) { put((value_t)key->hash(),(value_t)strdup(value->toCharArray()),_TYPE_OBJECT_P,_TYPE_CHAR_P); }
	void put(aObject *key,aString &value) { put((value_t)key->hash(),(value_t)strdup(value.toCharArray()),_TYPE_OBJECT_P,_TYPE_CHAR_P); }
	void put(aObject *key,const aObject *value) { put((value_t)key->hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_OBJECT_P); }
	void put(aObject *key,const aObject &value) { put((value_t)key->hash(),(value_t)&value,_TYPE_OBJECT_P,_TYPE_OBJECT_P); }
	/** @endcond */



	void put(aObject &key,void *value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_VOID_P); }
	/** @cond */
	void put(aObject &key,char value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,unsigned char value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,short value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,unsigned short value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,int value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,unsigned int value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,long value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,unsigned long value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
#if _WORDSIZE == 64
	void put(aObject &key,long long value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
	void put(aObject &key,unsigned long long value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_INTPTR); }
#endif
	void put(aObject &key,float value) { put((value_t)key.hash(),*(value_t *)((void *)&value),_TYPE_OBJECT_P,_TYPE_FLOAT); }
#if _WORDSIZE == 64
	void put(aObject &key,double value) { put((value_t)key.hash(),*(value_t *)((void *)&value),_TYPE_OBJECT_P,_TYPE_DOUBLE); }
#endif
	void put(aObject &key,const char *value) { put((value_t)key.hash(),(value_t)strdup(value),_TYPE_OBJECT_P,_TYPE_CHAR_P); }
	void put(aObject &key,aString *value) { put((value_t)key.hash(),(value_t)strdup(value->toCharArray()),_TYPE_OBJECT_P,_TYPE_CHAR_P); }
	void put(aObject &key,aString &value) { put((value_t)key.hash(),(value_t)strdup(value.toCharArray()),_TYPE_OBJECT_P,_TYPE_CHAR_P); }
	void put(aObject &key,const aObject *value) { put((value_t)key.hash(),(value_t)value,_TYPE_OBJECT_P,_TYPE_OBJECT_P); }
	void put(aObject &key,const aObject &value) { put((value_t)key.hash(),(value_t)&value,_TYPE_OBJECT_P,_TYPE_OBJECT_P); }
	/** @endcond */
	/** @} */



	/** @name Get value methods
	 * These methods finds and returns the corresponding value for the given key.
	 * @{ */
	value_t get(void *key) { return get((value_t)key,_TYPE_VOID_P); }
	value_t get(char key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(unsigned char key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(short key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(unsigned short key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(int key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(unsigned int key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(long key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(unsigned long key) { return get((value_t)key,_TYPE_INTPTR); }
	value_t get(float key) { return get(*((value_t *)((void *)&key)),_TYPE_FLOAT); }
	value_t get(const char *key);
	value_t get(aObject *key) { return get((value_t)key->hash(),_TYPE_OBJECT_P); }
	value_t get(aObject &key) { return get((value_t)key.hash(),_TYPE_OBJECT_P); }
	/** @} */

	/** @name Get value by key where value is of specified type
	 * These methods finds and returns the corresponding value for the given key,
	 * where the value is of specified type.
	 * @{ */
	value_t get(void *key,type_t &type) { return get((value_t)key,_TYPE_VOID_P,type); }
	value_t get(char key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(unsigned char key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(short key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(unsigned short key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(int key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(unsigned int key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(long key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(unsigned long key,type_t &type) { return get((value_t)key,_TYPE_INTPTR,type); }
	value_t get(float key,type_t &type) { return get(*((value_t *)((void *)&key)),_TYPE_FLOAT,type); }
	value_t get(const char *key,type_t &type);
	value_t get(aObject *key,type_t &type) { return get((value_t)key->hash(),_TYPE_OBJECT_P,type); }
	value_t get(aObject &key,type_t &type) { return get((value_t)key.hash(),_TYPE_OBJECT_P,type); }
	/** @} */


	/** @name Get char string value
	 * These methods finds and returns the corresponding value for the given key as a char string.
	 * @{ */
	char *getString(void *key) { return (char *)get((value_t)key,_TYPE_VOID_P); }
	char *getString(char key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(unsigned char key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(short key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(unsigned short key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(int key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(unsigned int key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(long key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(unsigned long key) { return (char *)get((value_t)key,_TYPE_INTPTR); }
	char *getString(float key) { return (char *)get(*((value_t *)((void *)&key)),_TYPE_FLOAT); }
	char *getString(const char *key) { return (char *)get(key); }
	char *getString(aObject *key) { return (char *)get((value_t)key->hash(),_TYPE_OBJECT_P); }
	char *getString(aObject &key) { return (char *)get((value_t)key.hash(),_TYPE_OBJECT_P); }
	/** @} */

	/** @name Get methods, where found values are placed in a vector
	 * These methods finds and inserts the corresponding value for the given key in a aVector,
	 * then returns number of found items.
	 * @{ */
	size_t get(void *key,aVector &v) { return get((value_t)key,_TYPE_VOID_P,v); }
	size_t get(char key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(unsigned char key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(short key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(unsigned short key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(int key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(unsigned int key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(long key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(unsigned long key,aVector &v) { return get((value_t)key,_TYPE_INTPTR,v); }
	size_t get(float key,aVector &v) { return get(*((value_t *)((void *)&key)),_TYPE_FLOAT,v); }
	size_t get(const char *key,aVector &v);
	size_t get(aObject *key,aVector &v) { return get((value_t)key->hash(),_TYPE_OBJECT_P,v); }
	size_t get(aObject &key,aVector &v) { return get((value_t)key.hash(),_TYPE_OBJECT_P,v); }
	/** @} */

	value_t getByIndex(size_t index,type_t &type);

	/** @name Contains methods
	 * Looks if a certain key is in the table.
	 * @{ */
	bool contains(void *key) { return get((value_t)key,_TYPE_VOID_P)!=0; }
	bool contains(char key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(unsigned char key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(short key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(unsigned short key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(int key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(unsigned int key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(long key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(unsigned long key) { return get((value_t)key,_TYPE_INTPTR)!=0; }
	bool contains(float key) { return get(*((value_t *)((void *)&key)),_TYPE_FLOAT)!=0; }
	bool contains(const char *key) { return get(key)!=0; }
	bool contains(aObject *key) { return get((value_t)key->hash(),_TYPE_OBJECT_P)!=0; }
	bool contains(aObject &key) { return get((value_t)key.hash(),_TYPE_OBJECT_P)!=0; }
	/** @} */

	/** @name Remove methods
	 * These methods finds and removes the corresponding value for the given key.
	 * @{ */
	value_t remove(void *key) { return remove((value_t)key,_TYPE_VOID_P); }
	value_t remove(char key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(unsigned char key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(short key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(unsigned short key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(int key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(unsigned int key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(long key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(unsigned long key) { return remove((value_t)key,_TYPE_INTPTR); }
	value_t remove(float key) { return remove(*((value_t *)((void *)&key)),_TYPE_FLOAT); }
	value_t remove(const char *key);
	value_t remove(aObject *key) { return remove((value_t)key->hash(),_TYPE_OBJECT_P); }
	value_t remove(aObject &key) { return remove((value_t)key.hash(),_TYPE_OBJECT_P); }
	/** @} */

	/** @name Remove value by key where value is of specified type
	 * These methods finds and removes the corresponding value for the given key,
	 * where the value is of specified type.
	 * @{ */
	value_t remove(void *key,type_t &type) { return remove((value_t)key,_TYPE_VOID_P,type); }
	value_t remove(char key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(unsigned char key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(short key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(unsigned short key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(int key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(unsigned int key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(long key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(unsigned long key,type_t &type) { return remove((value_t)key,_TYPE_INTPTR,type); }
	value_t remove(float key,type_t &type) { return remove(*((value_t *)((void *)&key)),_TYPE_FLOAT,type); }
	value_t remove(const char *key,type_t &type);
	value_t remove(aObject *key,type_t &type) { return remove((value_t)key->hash(),_TYPE_OBJECT_P,type); }
	value_t remove(aObject &key,type_t &type) { return remove((value_t)key.hash(),_TYPE_OBJECT_P,type); }
	/** @} */

	void removeAll();

	/** @name Input and Output methods
	 * @{ */
	size_t print(const char *fn);
	size_t print(FILE *fp);
	size_t load(const char *fn);
	size_t load(FILE *fp);
	size_t save(const char *fn);
	size_t save(FILE *fp);
	static void readPair(FILE *file,aString &key,aString &value);
	/** @} */
};


#endif /* _LIBAMANITA_HASHTABLE_H */


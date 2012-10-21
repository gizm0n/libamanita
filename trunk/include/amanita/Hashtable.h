#ifndef _AMANITA_HASHTABLE_H
#define _AMANITA_HASHTABLE_H

/**
 * @file amanita/Hashtable.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2004-09-30
 */ 

#include <string.h>
#include <stdlib.h>
#include <amanita/Collection.h>
#include <amanita/String.h>


/** Amanita Namespace */
namespace a {

enum {
	HASH_STYLE_CASE_SENSITIVE		= 0x00,	//!< 
	HASH_STYLE_CASE_INSENSITIVE	= 0x01,	//!< 
	HASH_STYLE_KEY_MULTIPLES		= 0x02,	//!< 
	HASH_STYLE_DEFAULT				= 0		//!< Default style for Hashtable objects.
};


/** @cond */
#define _HT_SET_INT_VALUE(v,t) if(at) at->value = (value_t)v,at->v_type = t;
#define _HT_SET_FLOAT_VALUE(v,t) if(at) at->value = *(value_t *)((void *)&v),at->v_type = t;
#define _HT_SET_STRING_VALUE(v,t) if(at) at->value = (value_t)strdup(v),at->v_type = t;

class Vector;
/** @endcond */


/** A generic multitype hashtable.
 * 
 * This hashtable differs from the standard C++ hashtable in that it isn't build with templates,
 * and instead handles any of the build in C types, and the String and Object of libamanita.
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
 * 
 * @ingroup amanita */
class Hashtable : public Collection {
friend class iterator;

/** @cond */
Object_Instance(Hashtable)
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
			if(k_type==CHAR_P) free((char *)key);
#if _WORDSIZE < 64
			else if(k_type==DOUBLE) free((double *)key);
#endif
			if(v_type==CHAR_P) free((char *)value);
#if _WORDSIZE < 64
			else if(v_type==DOUBLE) free((double *)value);
#endif
		}
	};
	node **table;
	size_t full;
	style_t style;
	float ld;

	/** @name Protected put methods
	 * @{ */
	void put(node &n);
	void put(value_t key,value_t value,type_t kt,type_t vt);
	void put(const char *key,value_t value,type_t vt);
	/** @} */

	/** @name Protected get methods
	 * @{ */
	value_t get(value_t key,type_t kt);
	value_t get(value_t key,type_t kt,type_t &type);
	size_t get(value_t key,type_t kt,Vector &v);
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
	/** Iterator class for Hashtable.
	 * This class is used to iterate through the values of the hashtable. */
	class iterator {
	friend class Hashtable;

	private:
		Hashtable *ht;		//!< The hashtable this iterator is iterating through.
		node *at;				//!< Node iterator is pointing at.
		long index;				//!< Index in table iterator is pointing at.

		/** Constructor.
		 * Only used by Hashtable.
		 * @param ht The hashtable. */
		iterator(Hashtable *ht) : ht(ht) { reset(); }

	public:
		/** Constructor. */
		iterator() : ht(0),at(0),index(ITER_EMPTY) {}

		/** Reset iterator to startpoint. */
		void reset() { at = 0,index = ht && ht->sz? ITER_START : ITER_EMPTY; }

		/** Set iterator to first element.
		 * @return Value. */
		value_t first() { return first(_EMPTY,_EMPTY); }

		/** Set iterator to first element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t first(type_t kt,type_t vt=_EMPTY);

		/** Set iterator to next element.
		 * @return Value. */
		value_t next();

		/** Set iterator to next element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t next(type_t kt,type_t vt=_EMPTY);

		/** Set iterator to previous element.
		 * @return Value. */
		value_t previous() { return previous(_EMPTY,_EMPTY); }

		/** Set iterator to previous element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t previous(type_t kt,type_t vt=_EMPTY);

		/** Set iterator to last element.
		 * @return Value. */
		value_t last() { return last(_EMPTY,_EMPTY); }

		/** Set iterator to last element.
		 * @param kt Type of key.
		 * @param vt Type of value. Optional.
		 * @return Value. */
		value_t last(type_t kt,type_t vt=_EMPTY);

		/** Remove element at iterator's index.
		 * @return Value of the removed element. */
		value_t remove();

		/** Get the key at where the index is.
		 * @return Value. */
		value_t key() { return at? at->key : 0; }

		/** Get the type of the key at where the index is.
		 * @return Type of the value. */
		type_t keyType() { return at? at->k_type : _EMPTY; }

		/** Get the value at where the index is.
		 * @return Value. */
		value_t value() { return at? at->value : 0; }

		/** Get the type of the value at where the index is.
		 * @return Type of the value. */
		type_t valueType() { return at? at->v_type : _EMPTY; }

		/** Set value method.
		 * This method set value to the element at where the iterator is pointing.
		 * Only the <tt>void *</tt> value method is documented, but all types have
		 * their respective setValue method.
		 * @param v Can be of any type.
		 */
		void setValue(void *v) { _HT_SET_INT_VALUE(v,_VOID_P); }
		void setValue(char v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(unsigned char v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(short v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(unsigned short v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(int v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(unsigned int v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(long v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(unsigned long v) { _HT_SET_INT_VALUE(v,_INTPTR); }
#if _WORDSIZE == 64
		void setValue(long long v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(unsigned long long v) { _HT_SET_INT_VALUE(v,_INTPTR); }
		void setValue(float v) { double d = (double)v;_HT_SET_FLOAT_VALUE(d,_DOUBLE); }
		void setValue(double v) { _HT_SET_FLOAT_VALUE(v,_DOUBLE); }
#else
		//void setValue(long long v);
		//void setValue(unsigned long long v);
		void setValue(float v) { _HT_SET_FLOAT_VALUE(v,_FLOAT); }
		//void setValue(double v);
#endif
		//void setValue(long double v);
		void setValue(const char *v) { _HT_SET_STRING_VALUE(v,_CHAR_P); }
		void setValue(const String *v) { _HT_SET_STRING_VALUE((const char *)*v,_CHAR_P); }
		void setValue(const String &v) { _HT_SET_STRING_VALUE((const char *)v,_CHAR_P); }
		void setValue(const Object *v) { _HT_SET_INT_VALUE(v,_OBJECT); }
		void setValue(const Object &v) { _HT_SET_INT_VALUE(&v,_OBJECT); }
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
		//operator long() const { return at && at->v_type==_INTPTR? (long)at->value : 0; }
		//operator unsigned long() const { return at && at->v_type==_INTPTR? (unsigned long)at->value : 0; }
		//operator char *() const { return at && at->v_type==_CHAR_P? (char *)at->value : 0; }
		/** @} */
	};

	/** @name Constructos and Destructors
	 * @{ */
	Hashtable(size_t c=11,float l=0.5f,style_t st=HASH_STYLE_DEFAULT);
	virtual ~Hashtable() { removeAll(); }
	/** @} */

	iterator iterate();

	void setCaseInsensitiveKeys(bool b);
	void setAllowKeyMultiples(bool b);

	/** Get Operator.
	 * Only the <tt>void *</tt> key operator is documented, but all types have their respective << operator.
	 * @param key Can be of any type.
	 * @return A value_t associated with key. */
	value_t operator[](void *key) { return get((value_t)key,_VOID_P); }
	/** @cond */
	value_t operator[](char key) { return get((value_t)key,_INTPTR); }
	value_t operator[](unsigned char key) { return get((value_t)key,_INTPTR); }
	value_t operator[](short key) { return get((value_t)key,_INTPTR); }
	value_t operator[](unsigned short key) { return get((value_t)key,_INTPTR); }
	value_t operator[](int key) { return get((value_t)key,_INTPTR); }
	value_t operator[](unsigned int key) { return get((value_t)key,_INTPTR); }
	value_t operator[](long key) { return get((value_t)key,_INTPTR); }
	value_t operator[](unsigned long key) { return get((value_t)key,_INTPTR); }
	value_t operator[](float key) { return get(*((value_t *)((void *)&key)),_FLOAT); }
	value_t operator[](const char *key);
	value_t operator[](const String *key) { return get((const char *)*key); }
	value_t operator[](const String &key) { return get((const char *)key); }
	value_t operator[](const Object *key) { return get((value_t)key->hash(),_OBJECT); }
	value_t operator[](const Object &key) { return get((value_t)key.hash(),_OBJECT); }
	/** @endcond */

	/** Insert operator.
	 * Insert into hashtable value and set index to size of hashtable. Same as <tt>put(size(),value)</tt>.
	 * Only the <tt>void *</tt> value operator is documented, but all types have their respective << operator.
	 * @param value Can be of any type.
	 * @return A reference to this hashtable. */
	Hashtable &operator<<(void *value) { put(sz,(value_t)value,_INTPTR,_VOID_P);return *this; }
	/** @cond */
	Hashtable &operator<<(char value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(unsigned char value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(short value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(unsigned short value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(int value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(unsigned int value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(long value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(unsigned long value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
#if _WORDSIZE == 64
	Hashtable &operator<<(long long value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
	Hashtable &operator<<(unsigned long long value) { put(sz,(value_t)value,_INTPTR,_INTPTR);return *this; }
#endif
	Hashtable &operator<<(float value) { put(sz,*(value_t *)((void *)&value),_INTPTR,_FLOAT);return *this; }
#if _WORDSIZE == 64
	Hashtable &operator<<(double value) { put(sz,*(value_t *)((void *)&value),_INTPTR,_DOUBLE);return *this; }
#endif
	Hashtable &operator<<(const char *value) { put(sz,(value_t)strdup(value),_INTPTR,_CHAR_P);return *this; }
	Hashtable &operator<<(const String *value) { put(sz,(value_t)strdup(*value),_INTPTR,_CHAR_P);return *this; }
	Hashtable &operator<<(const String &value) { put(sz,(value_t)strdup(value),_INTPTR,_CHAR_P);return *this; }
	Hashtable &operator<<(const Object *value) { put(sz,(value_t)value,_INTPTR,_OBJECT);return *this; }
	Hashtable &operator<<(const Object &value) { put(sz,(value_t)&value,_INTPTR,_OBJECT);return *this; }
	/** @endcond */

	/** Insert operator.
	 * Merge all items in the hashtable ht with this.
	 * @param ht Hashtable to merge.
	 * @return A reference to this hashtable. */
	Hashtable &operator<<(Hashtable *ht) { merge(*ht);return *this; }

	/** Insert operator.
	 * Merge all items in the hashtable ht with this.
	 * @param ht Hashtable to merge.
	 * @return A reference to this hashtable. */
	Hashtable &operator<<(Hashtable &ht) { merge(ht);return *this; }

	/** Insert operator.
	 * Merge all items in the vector vec with this, and set index to index in vector.
	 * @param vec Vector to merge.
	 * @return A reference to this hashtable. */
	Hashtable &operator<<(Vector *vec) { merge(*vec);return *this; }

	/** Insert operator.
	 * Merge all items in the vector vec with this, and set index to index in vector.
	 * @param vec Vector to merge.
	 * @return A reference to this hashtable. */
	Hashtable &operator<<(Vector &vec) { merge(vec);return *this; }

	/** Remove operator.
	 * Only the <tt>void *</tt> key operator is documented, but all types have their respective >> operator.
	 * @param key Can be of any type.
	 * @return A reference to this hashtable. */
	Hashtable &operator>>(void *key) { remove((value_t)key,_VOID_P);return *this; }
	/** @cond */
	Hashtable &operator>>(char key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(unsigned char key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(short key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(unsigned short key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(int key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(unsigned int key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(long key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(unsigned long key) { remove((value_t)key,_INTPTR);return *this; }
	Hashtable &operator>>(float key) { remove(*((value_t *)((void *)&key)),_FLOAT);return *this; }
	Hashtable &operator>>(const char *key) { remove(key);return *this; }
	Hashtable &operator>>(const String *key) { remove((const char *)*key);return *this; }
	Hashtable &operator>>(const String &key) { remove((const char *)key);return *this; }
	Hashtable &operator>>(const Object *key) { remove((value_t)key->hash(),_OBJECT);return *this; }
	Hashtable &operator>>(const Object &key) { remove((value_t)key.hash(),_OBJECT);return *this; }
	/** @endcond */

	/** Put method.
	 * This method puts a value into the table using the given key. Only the <tt>void *</tt> key
	 * and value method is documented, but all types have their respective put method.
	 * @param key Can be of any type.
	 * @param value Can be of any type. */
	void put(void *key,void *value) { put((value_t)key,(value_t)value,_VOID_P,_VOID_P); }
	/** @cond */
	void put(void *key,char value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,unsigned char value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,short value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,unsigned short value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,int value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,unsigned int value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,long value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,unsigned long value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
#if _WORDSIZE == 64
	void put(void *key,long long value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
	void put(void *key,unsigned long long value) { put((value_t)key,(value_t)value,_VOID_P,_INTPTR); }
#endif
	void put(void *key,float value) { put((value_t)key,*(value_t *)((void *)&value),_VOID_P,_FLOAT); }
#if _WORDSIZE == 64
	void put(void *key,double value) { put((value_t)key,*(value_t *)((void *)&value),_VOID_P,_DOUBLE); }
#endif
	void put(void *key,const char *value) { put((value_t)key,(value_t)strdup(value),_VOID_P,_CHAR_P); }
	void put(void *key,const String *value) { put((value_t)key,(value_t)strdup(*value),_VOID_P,_CHAR_P); }
	void put(void *key,const String &value) { put((value_t)key,(value_t)strdup(value),_VOID_P,_CHAR_P); }
	void put(void *key,const Object *value) { put((value_t)key,(value_t)value,_VOID_P,_OBJECT); }
	void put(void *key,const Object &value) { put((value_t)key,(value_t)&value,_VOID_P,_OBJECT); }


	void put(char key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(char key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(char key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(char key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(char key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(char key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(char key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(char key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(char key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(char key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(char key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(unsigned char key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(unsigned char key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned char key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned char key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(unsigned char key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned charkey,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(unsigned char key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned char key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(unsigned char key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned char key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(unsigned char key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(short key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(short key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(short key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(short key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(short key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(short key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(short key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(short key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(short key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(short key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(short key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(unsigned short key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(unsigned short key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned short key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned short key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(unsigned short key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned short key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(unsigned short key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned short key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(unsigned short key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned short key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(unsigned short key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(int key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(int key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(int key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(int key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(int key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(int key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(int key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(int key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(int key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(int key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(int key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(unsigned int key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(unsigned int key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned int key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned int key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(unsigned int key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned int key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(unsigned int key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned int key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(unsigned int key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned int key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(unsigned int key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(long key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(long key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(long key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(long key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(long key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(long key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(long key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(long key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(unsigned long key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(unsigned long key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#if _WORDSIZE == 64
	void put(unsigned long key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
#endif
	void put(unsigned long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
#if _WORDSIZE == 64
	void put(unsigned long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
#endif
	void put(unsigned long key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned long key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(unsigned long key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned long key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(unsigned long key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

#if _WORDSIZE == 64
	void put(long long key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(long long key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(long long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
	void put(long long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
	void put(long long key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(long long key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(long long key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(long long key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(long long key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }

	void put(unsigned long long key,void *value) { put((value_t)key,(value_t)value,_INTPTR,_VOID_P); }
	void put(unsigned long long key,char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,unsigned char value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,unsigned short value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,unsigned int value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,unsigned long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,unsigned long long value) { put((value_t)key,(value_t)value,_INTPTR,_INTPTR); }
	void put(unsigned long long key,float value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_FLOAT); }
	void put(unsigned long long key,double value) { put((value_t)key,*(value_t *)((void *)&value),_INTPTR,_DOUBLE); }
	void put(unsigned long long key,const char *value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned long long key,const String *value) { put((value_t)key,(value_t)strdup(*value),_INTPTR,_CHAR_P); }
	void put(unsigned long long key,const String &value) { put((value_t)key,(value_t)strdup(value),_INTPTR,_CHAR_P); }
	void put(unsigned long long key,const Object *value) { put((value_t)key,(value_t)value,_INTPTR,_OBJECT); }
	void put(unsigned long long key,const Object &value) { put((value_t)key,(value_t)&value,_INTPTR,_OBJECT); }
#endif

	void put(float key,void *value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_VOID_P); }
	void put(float key,char value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,unsigned char value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,short value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,unsigned short value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,int value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,unsigned int value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,long value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,unsigned long value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
#if _WORDSIZE == 64
	void put(float key,long long value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
	void put(float key,unsigned long long value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_INTPTR); }
#endif
	void put(float key,float value) { put(*(value_t *)((void *)&key),*(value_t *)((void *)&value),_FLOAT,_FLOAT); }
#if _WORDSIZE == 64
	void put(float key,double value) { put(*(value_t *)((void *)&key),*(value_t *)((void *)&value),_FLOAT,_DOUBLE); }
#endif
	void put(float key,const char *value) { put(*(value_t *)((void *)&key),(value_t)strdup(value),_FLOAT,_CHAR_P); }
	void put(float key,const String *value) { put(*(value_t *)((void *)&key),(value_t)strdup(*value),_FLOAT,_CHAR_P); }
	void put(float key,const String &value) { put(*(value_t *)((void *)&key),(value_t)strdup(value),_FLOAT,_CHAR_P); }
	void put(float key,const Object *value) { put(*(value_t *)((void *)&key),(value_t)value,_FLOAT,_OBJECT); }
	void put(float key,const Object &value) { put(*(value_t *)((void *)&key),(value_t)&value,_FLOAT,_OBJECT); }

	void put(const char *key,void *value) { put(key,(value_t)value,_VOID_P); }
	void put(const char *key,char value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,unsigned char value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,short value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,unsigned short value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,int value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,unsigned int value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,long value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,unsigned long value) { put(key,(value_t)value,_INTPTR); }
#if _WORDSIZE == 64
	void put(const char *key,long long value) { put(key,(value_t)value,_INTPTR); }
	void put(const char *key,unsigned long long value) { put(key,(value_t)value,_INTPTR); }
#endif
	void put(const char *key,float value) { put(key,*(value_t *)((void *)&value),_FLOAT); }
#if _WORDSIZE == 64
	void put(const char *key,double value) { put(key,*(value_t *)((void *)&value),_DOUBLE); }
#endif
	void put(const char *key,const char *value) { put(key,(value_t)strdup(value),_CHAR_P); }
	void put(const char *key,const String *value) { put(key,(value_t)strdup(*value),_CHAR_P); }
	void put(const char *key,const String &value) { put(key,(value_t)strdup(value),_CHAR_P); }
	void put(const char *key,const Object *value) { put(key,(value_t)value,_OBJECT); }
	void put(const char *key,const Object &value) { put(key,(value_t)&value,_OBJECT); }

	void put(String *key,void *value) { put((const char *)*key,(value_t)value,_VOID_P); }
	void put(String *key,char value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,unsigned char value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,short value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,unsigned short value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,int value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,unsigned int value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,long value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,unsigned long value) { put((const char *)*key,(value_t)value,_INTPTR); }
#if _WORDSIZE == 64
	void put(String *key,long long value) { put((const char *)*key,(value_t)value,_INTPTR); }
	void put(String *key,unsigned long long value) { put((const char *)*key,(value_t)value,_INTPTR); }
#endif
	void put(String *key,float value) { put((const char *)*key,*(value_t *)((void *)&value),_FLOAT); }
#if _WORDSIZE == 64
	void put(String *key,double value) { put((const char *)*key,*(value_t *)((void *)&value),_DOUBLE); }
#endif
	void put(String *key,const char *value) { put((const char *)*key,(value_t)strdup(value),_CHAR_P); }
	void put(String *key,const String *value) { put((const char *)*key,(value_t)strdup(*value),_CHAR_P); }
	void put(String *key,const String &value) { put((const char *)*key,(value_t)strdup(value),_CHAR_P); }
	void put(String *key,const Object *value) { put((const char *)*key,(value_t)value,_OBJECT); }
	void put(String *key,const Object &value) { put((const char *)*key,(value_t)&value,_OBJECT); }

	void put(String &key,void *value) { put((const char *)key,(value_t)value,_VOID_P); }
	void put(String &key,char value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,unsigned char value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,short value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,unsigned short value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,int value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,unsigned int value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,long value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,unsigned long value) { put((const char *)key,(value_t)value,_INTPTR); }
#if _WORDSIZE == 64
	void put(String &key,long long value) { put((const char *)key,(value_t)value,_INTPTR); }
	void put(String &key,unsigned long long value) { put((const char *)key,(value_t)value,_INTPTR); }
#endif
	void put(String &key,float value) { put((const char *)key,*(value_t *)((void *)&value),_FLOAT); }
#if _WORDSIZE == 64
	void put(String &key,double value) { put((const char *)key,*(value_t *)((void *)&value),_DOUBLE); }
#endif
	void put(String &key,const char *value) { put((const char *)key,(value_t)strdup(value),_CHAR_P); }
	void put(String &key,const String *value) { put((const char *)key,(value_t)strdup(*value),_CHAR_P); }
	void put(String &key,const String &value) { put((const char *)key,(value_t)strdup(value),_CHAR_P); }
	void put(String &key,const Object *value) { put((const char *)key,(value_t)value,_OBJECT); }
	void put(String &key,const Object &value) { put((const char *)key,(value_t)&value,_OBJECT); }

	void put(Object *key,void *value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_VOID_P); }
	void put(Object *key,char value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,unsigned char value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,short value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,unsigned short value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,int value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,unsigned int value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,long value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,unsigned long value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
#if _WORDSIZE == 64
	void put(Object *key,long long value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object *key,unsigned long long value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_INTPTR); }
#endif
	void put(Object *key,float value) { put((value_t)key->hash(),*(value_t *)((void *)&value),_OBJECT,_FLOAT); }
#if _WORDSIZE == 64
	void put(Object *key,double value) { put((value_t)key->hash(),*(value_t *)((void *)&value),_OBJECT,_DOUBLE); }
#endif
	void put(Object *key,const char *value) { put((value_t)key->hash(),(value_t)strdup(value),_OBJECT,_CHAR_P); }
	void put(Object *key,const String *value) { put((value_t)key->hash(),(value_t)strdup(*value),_OBJECT,_CHAR_P); }
	void put(Object *key,const String &value) { put((value_t)key->hash(),(value_t)strdup(value),_OBJECT,_CHAR_P); }
	void put(Object *key,const Object *value) { put((value_t)key->hash(),(value_t)value,_OBJECT,_OBJECT); }
	void put(Object *key,const Object &value) { put((value_t)key->hash(),(value_t)&value,_OBJECT,_OBJECT); }

	void put(Object &key,void *value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_VOID_P); }
	void put(Object &key,char value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,unsigned char value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,short value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,unsigned short value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,int value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,unsigned int value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,long value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,unsigned long value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
#if _WORDSIZE == 64
	void put(Object &key,long long value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
	void put(Object &key,unsigned long long value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_INTPTR); }
#endif
	void put(Object &key,float value) { put((value_t)key.hash(),*(value_t *)((void *)&value),_OBJECT,_FLOAT); }
#if _WORDSIZE == 64
	void put(Object &key,double value) { put((value_t)key.hash(),*(value_t *)((void *)&value),_OBJECT,_DOUBLE); }
#endif
	void put(Object &key,const char *value) { put((value_t)key.hash(),(value_t)strdup(value),_OBJECT,_CHAR_P); }
	void put(Object &key,const String *value) { put((value_t)key.hash(),(value_t)strdup(*value),_OBJECT,_CHAR_P); }
	void put(Object &key,const String &value) { put((value_t)key.hash(),(value_t)strdup(value),_OBJECT,_CHAR_P); }
	void put(Object &key,const Object *value) { put((value_t)key.hash(),(value_t)value,_OBJECT,_OBJECT); }
	void put(Object &key,const Object &value) { put((value_t)key.hash(),(value_t)&value,_OBJECT,_OBJECT); }
	/** @endcond */


	/** Get value method.
	 * This method finds and returns the corresponding value for the given key.
	 * Only the <tt>void *</tt> key method is documented, but all types have their respective get method.
	 * @param key Can be of any type.
	 * @return If found a value_t associated with key, otherwise 0. */
	value_t get(void *key) { return get((value_t)key,_VOID_P); }
	/** @cond */
	value_t get(char key) { return get((value_t)key,_INTPTR); }
	value_t get(unsigned char key) { return get((value_t)key,_INTPTR); }
	value_t get(short key) { return get((value_t)key,_INTPTR); }
	value_t get(unsigned short key) { return get((value_t)key,_INTPTR); }
	value_t get(int key) { return get((value_t)key,_INTPTR); }
	value_t get(unsigned int key) { return get((value_t)key,_INTPTR); }
	value_t get(long key) { return get((value_t)key,_INTPTR); }
	value_t get(unsigned long key) { return get((value_t)key,_INTPTR); }
	value_t get(float key) { return get(*((value_t *)((void *)&key)),_FLOAT); }
	value_t get(const char *key);
	value_t get(const String *key) { return get((const char *)*key); }
	value_t get(const String &key) { return get((const char *)key); }
	value_t get(const Object *key) { return get((value_t)key->hash(),_OBJECT); }
	value_t get(const Object &key) { return get((value_t)key.hash(),_OBJECT); }
	/** @endcond */

	/** Get value by key where value is of specified type.
	 * This method finds and returns the corresponding value for the given key,
	 * where the value is of specified type. Only the <tt>void *</tt> key method
	 * is documented, but all types have their respective getString method.
	 * @param key Can be of any type.
	 * @param type Specified type key must be of.
	 * @return If found a value_t associated with key, otherwise 0. */
	value_t get(void *key,type_t &type) { return get((value_t)key,_VOID_P,type); }
	/** @cond */
	value_t get(char key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(unsigned char key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(short key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(unsigned short key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(int key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(unsigned int key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(long key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(unsigned long key,type_t &type) { return get((value_t)key,_INTPTR,type); }
	value_t get(float key,type_t &type) { return get(*((value_t *)((void *)&key)),_FLOAT,type); }
	value_t get(const char *key,type_t &type);
	value_t get(const String *key,type_t &type) { return get((const char *)*key,type); }
	value_t get(const String &key,type_t &type) { return get((const char *)key,type); }
	value_t get(const Object *key,type_t &type) { return get((value_t)key->hash(),_OBJECT,type); }
	value_t get(const Object &key,type_t &type) { return get((value_t)key.hash(),_OBJECT,type); }
	/** @endcond */


	/** Get char string value.
	 * This method finds and returns the corresponding value for the given key as a char string.
	 * Only the <tt>void *</tt> key method is documented, but all types have their respective getString method.
	 * @param key Can be of any type.
	 * @return If found a string associated with key, otherwise 0. */
	const char *getString(void *key) { return (const char *)get((value_t)key,_VOID_P); }
	/** @cond */
	const char *getString(char key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(unsigned char key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(short key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(unsigned short key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(int key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(unsigned int key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(long key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(unsigned long key) { return (const char *)get((value_t)key,_INTPTR); }
	const char *getString(float key) { return (const char *)get(*((value_t *)((void *)&key)),_FLOAT); }
	const char *getString(const char *key) { return (const char *)get(key); }
	const char *getString(const String *key) { return (const char *)get((const char *)*key); }
	const char *getString(const String &key) { return (const char *)get((const char *)key); }
	const char *getString(const Object *key) { return (const char *)get((value_t)key->hash(),_OBJECT); }
	const char *getString(const Object &key) { return (const char *)get((value_t)key.hash(),_OBJECT); }
	/** @endcond */

	/** Get method, where found values are placed in a vector.
	 * This method finds and inserts the corresponding value for the given key in a Vector,
	 * then returns number of found items. Only the <tt>void *</tt> key method is documented,
	 * but all types have their respective get method.
	 * @param key Can be of any type.
	 * @param v Vector to place found objects in.
	 * @return Number of found objects. */
	size_t get(void *key,Vector &v) { return get((value_t)key,_VOID_P,v); }
	/** @cond */
	size_t get(char key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(unsigned char key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(short key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(unsigned short key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(int key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(unsigned int key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(long key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(unsigned long key,Vector &v) { return get((value_t)key,_INTPTR,v); }
	size_t get(float key,Vector &v) { return get(*((value_t *)((void *)&key)),_FLOAT,v); }
	size_t get(const char *key,Vector &v);
	size_t get(const String *key,Vector &v) { return get((const char *)*key,v); }
	size_t get(const String &key,Vector &v) { return get((const char *)key,v); }
	size_t get(const Object *key,Vector &v) { return get((value_t)key->hash(),_OBJECT,v); }
	size_t get(const Object &key,Vector &v) { return get((value_t)key.hash(),_OBJECT,v); }
	/** @endcond */

	value_t getByIndex(size_t index,type_t &type);

	/** Contains.
	 * Check if a certain key is in the table. Only the <tt>void *</tt> key
	 * method is documented, but all types have their respective contains method.
	 * @param key Can be of any type.
	 * @return true if found, else false. */
	bool contains(void *key) { return get((value_t)key,_VOID_P)!=0; }
	/** @cond */
	bool contains(char key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(unsigned char key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(short key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(unsigned short key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(int key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(unsigned int key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(long key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(unsigned long key) { return get((value_t)key,_INTPTR)!=0; }
	bool contains(float key) { return get(*((value_t *)((void *)&key)),_FLOAT)!=0; }
	bool contains(const char *key) { return get(key)!=0; }
	bool contains(const String *key) { return get((const char *)*key)!=0; }
	bool contains(const String &key) { return get((const char *)key)!=0; }
	bool contains(const Object *key) { return get((value_t)key->hash(),_OBJECT)!=0; }
	bool contains(const Object &key) { return get((value_t)key.hash(),_OBJECT)!=0; }
	/** @endcond */

	/** Remove.
	 * This method finds and removes the corresponding value for the given
	 * key. Only the <tt>void *</tt> key method is documented, but all types have
	 * their respective remove method.
	 * @param key Can be of any type.
	 * @return Value associated with the key. */
	value_t remove(void *key) { return remove((value_t)key,_VOID_P); }
	/** @cond */
	value_t remove(char key) { return remove((value_t)key,_INTPTR); }
	value_t remove(unsigned char key) { return remove((value_t)key,_INTPTR); }
	value_t remove(short key) { return remove((value_t)key,_INTPTR); }
	value_t remove(unsigned short key) { return remove((value_t)key,_INTPTR); }
	value_t remove(int key) { return remove((value_t)key,_INTPTR); }
	value_t remove(unsigned int key) { return remove((value_t)key,_INTPTR); }
	value_t remove(long key) { return remove((value_t)key,_INTPTR); }
	value_t remove(unsigned long key) { return remove((value_t)key,_INTPTR); }
	value_t remove(float key) { return remove(*((value_t *)((void *)&key)),_FLOAT); }
	value_t remove(const char *key);
	value_t remove(const String *key) { return remove((const char *)*key); }
	value_t remove(const String &key) { return remove((const char *)key); }
	value_t remove(const Object *key) { return remove((value_t)key->hash(),_OBJECT); }
	value_t remove(const Object &key) { return remove((value_t)key.hash(),_OBJECT); }
	/** @endcond */

	/** Remove value by key where value is of specified type.
	 * Finds and removes the corresponding value for the given key, where the value is
	 * of specified type. Only the <tt>void *</tt> key method is documented, but all
	 * types have their respective remove method.
	 * @param key Can be of any type.
	 * @param type Specified type key must be of.
	 * @return Value associated with the key of specified type. */
	value_t remove(void *key,type_t &type) { return remove((value_t)key,_VOID_P,type); }
	/** @cond */
	value_t remove(char key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(unsigned char key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(short key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(unsigned short key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(int key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(unsigned int key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(long key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(unsigned long key,type_t &type) { return remove((value_t)key,_INTPTR,type); }
	value_t remove(float key,type_t &type) { return remove(*((value_t *)((void *)&key)),_FLOAT,type); }
	value_t remove(const char *key,type_t &type);
	value_t remove(const String *key,type_t &type) { return remove((const char *)*key,type); }
	value_t remove(const String &key,type_t &type) { return remove((const char *)key,type); }
	value_t remove(const Object *key,type_t &type) { return remove((value_t)key->hash(),_OBJECT,type); }
	value_t remove(const Object &key,type_t &type) { return remove((value_t)key.hash(),_OBJECT,type); }
	/** @endcond */

	/** Remove all items in hashtable. */
	void removeAll();

	/** @name Input and Output methods
	 * @{ */
	size_t print(const char *fn);
	size_t print(FILE *fp=stdout);
	size_t load(const char *fn,const char *k=0,const char *l=0);
	size_t load(FILE *fp,const char *k=0,const char *l=0);
	size_t save(const char *fn,const char *k=0,const char *l=0);
	size_t save(FILE *fp,const char *k=0,const char *l=0);
	/** @} */

	/** Parse and merge a string of text-data.
	 * @see merge(const char *str) */
	size_t parse(const char *str) { return merge(str); }

	/** @name Merge methods
	 * @{ */
	/** Parse and merge a string of text-data.
	 * @param str String of data to be parsed and joined with the hashtable. Each element is separated
	 * by a new line, and key and value separated with a '=' or a ':'. Excess whitespace is stripped.
	 * Values are read similar to c-strings, and accept backslash codes ('\n' for new line), and
	 * additional lines can be added with a backslash.
	 * @return Number of items added to the table. */
	size_t merge(const char *str);

	/** Merge hashtable ht with this hashtable, all ht's items are joined with this hashtable.
	 * @param ht Hashtable to merge.
	 * @return Number of items merged. */
	size_t merge(Hashtable *ht) { return ht? merge(*ht) : 0; }

	/** Merge hashtable ht with this hashtable, all ht's items are joined with this hashtable.
	 * @param ht Hashtable to merge.
	 * @return Number of items merged. */
	size_t merge(Hashtable &ht);

	/** Merge vector vec with this hashtable, all vec's items are joined with this hashtable using index as key.
	 * @param vec Vector to merge.
	 * @return Number of items merged. */
	size_t merge(Vector *vec) { return vec? merge(*vec) : 0; }

	/** Merge vector vec with this hashtable, all vec's items are joined with this hashtable using index as key.
	 * @param vec Vector to merge.
	 * @return Number of items merged. */
	size_t merge(Vector &vec);
	/** @} */
};

}; /* namespace a */


#endif /* _AMANITA_HASHTABLE_H */


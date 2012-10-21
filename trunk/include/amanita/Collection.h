#ifndef _AMANITA_COLLECTION_H
#define _AMANITA_COLLECTION_H

/**
 * @file amanita/Collection.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/Object.h>


/** Amanita Namespace */
namespace a {


/** Enumeration used by the iteration class and subclasses
 * for extended iteration positions. */
enum {
	ITER_EMPTY				= -4,		//!< Iteration points to an empty value.
	ITER_START				= -3,		//!< Iteration points at the start of the Collection.
	ITER_AFTER_LAST		= -2,		//!< Iteration points at one step after last value in the Collection.
	ITER_BEFORE_FIRST		= -1,		//!< Iteration points at one step before first value in the Collection.
};

/** Enumeration used by all subclasses of the Collection
 * class to tell what type a value contains. */
enum {
	EMPTY				= 0x00,		//!< Value is empty, meaning this position in the Collection is not filled with a value.
	VOID_P,							//!< (void *)
	INT8,								//!< (int8_t)
	UINT8,							//!< (uint8_t)
	INT16,							//!< (int16_t)
	UINT16,							//!< (uint16_t)
	INT32,							//!< (int32_t)
	UINT32,							//!< (uint32_t)
	INT64,							//!< (int64_t)
	UINT64,							//!< (uint64_t)
	PTRDIFF,							//!< (ptrdiff_t)
	INTPTR,							//!< (intptr_t)
	FLOAT,							//!< (float)
	DOUBLE,							//!< (double)
	LDOUBLE,							//!< (long double)
	CHAR_P,							//!< (char *)

	OBJECT,							//!< (Object *)
	STRING,							//!< (String *)
	VECTOR,							//!< (Vector *)
	HASHTABLE,						//!< (Hashtable *)
};

/** @name Type flags
 * Can be XOR:ed to any type.
 * @{ */
enum {
	VALUE_NAN		= 0x20,		//!< Not a Number
	VALUE_INF		= 0x40,		//!< Infinite (Generally a result of x/0)
	VALUE_NEG		= 0x80,		//!< Negative
};
/** @} */

/** Should contain a value from the type-enumeration to tell
 * what type a Collection-value contains. */
typedef unsigned char type_t;

/** @cond */
#define _EMPTY ((type_t)EMPTY)
#define _VOID_P ((type_t)VOID_P)
#define _INT8 ((type_t)INT8)
#define _UINT8 ((type_t)UINT8)
#define _INT16 ((type_t)INT16)
#define _UINT16 ((type_t)UINT16)
#define _INT32 ((type_t)INT32)
#define _UINT32 ((type_t)UINT32)
#define _INT64 ((type_t)INT64)
#define _UINT64 ((type_t)UINT64)
#define _PTRDIFF ((type_t)PTRDIFF)
#define _INTPTR ((type_t)INTPTR)
#define _FLOAT ((type_t)FLOAT)
#define _DOUBLE ((type_t)DOUBLE)
#define _LDOUBLE ((type_t)LDOUBLE)
#define _CHAR_P ((type_t)CHAR_P)
#define _OBJECT ((type_t)OBJECT)
#define _STRING ((type_t)STRING)
#define _VECTOR ((type_t)VECTOR)
#define _HASHTABLE ((type_t)HASHTABLE)
/** @endcond */

/** Is a variant, it may contain any value from the type-enumeration.
 * Types larger than 32 bits are allocated with malloc and the
 * adress is stored in the value_t instead. */
typedef intptr_t value_t;

/** Contains information about how the Collection should be handled. */
typedef uint32_t style_t;

/** A parent class for collection classes.
 * 
 * The Collection-class is the top-class for all types of collections.
 * I decided creating my own set of Container-classes simply because of learning the algorithms and keeping
 * to my principal idea of making everything by hand. I consider programming an artform, so I had no intention
 * of making the most efficient or "best" choice, but simply making as much as possible handmade. Also, I
 * dislike templates because I find them ugly and I wanted my set of Collection-classes to contain any types
 * and still be relatively type-safe. These classes do work fine and have been thoroughly tested, of course
 * they could be better optimized, but that is an ongoing project.
 *
 * The Collection-class is more or less an abstract class, even though it has no pure virtual methods at the time.
 * It extends the Object-class so it can be implemented in the class-hierarchy of that class. All subclasses of
 * Collection obviously has to implement the Object-class style to maintain rtti-structure, though not doing so
 * only means pointing to the super-class Class instead.
 * 
 * @ingroup amanita */
class Collection : public Object {
/** @cond */
Object_Instance(Collection)
/** @endcond */

protected:
	size_t sz;	//!< Size of the collection.
	size_t cap;	//!< Capacity of the collection.

public:
	/** Constructor. */
	Collection() : Object(),sz(0),cap(0) {}

	/** @name Operators
	 * @{ */
	operator bool() const { return (bool)(sz>0); }
	operator int() const { return (int)sz; }
	operator unsigned int() const { return (unsigned int)sz; }
	operator long() const { return (long)sz; }
	operator unsigned long() const { return (unsigned long)sz; }
	/** @} */

	/** Size of the collection.
	 * @return Size of the collection. */
	size_t size() { return sz; }

	/** Capacity of the collection.
	 * @return Capacity of the collection. */
	size_t capacity() { return cap; }
};

}; /* namespace a */


#endif /* _AMANITA_COLLECTION_H */


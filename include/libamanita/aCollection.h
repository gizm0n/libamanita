#ifndef _LIBAMANITA_COLLECTION_H
#define _LIBAMANITA_COLLECTION_H

/**
 * @file libamanita/aCollection.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <libamanita/aObject.h>

/** Enumeration used by the iteration class and subclasses for extended iteration positions. */
enum {
	ITER_INDEX_EMPTY			= -4,		//!< Iteration points to an empty value.
	ITER_INDEX_START			= -3,		//!< Iteration points at the start of the aCollection.
	ITER_INDEX_AFTER_LAST	= -2,		//!< Iteration points at one step after last value in the aCollection.
	ITER_INDEX_BEFORE_FIRST	= -1,		//!< Iteration points at one step before first value in the aCollection.
};

/** Enumeration used by all subclasses of the aCollection class to tell what type a value contains. */
enum {
	TYPE_EMPTY			= 0x00,	//!< Value is empty, meaning this position in the aCollection is not filled with a value..
	TYPE_VOID_P,					//!< (void *)
	TYPE_INT8,						//!< (int8_t)
	TYPE_UINT8,						//!< (uint8_t)
	TYPE_INT16,						//!< (int16_t)
	TYPE_UINT16,					//!< (uint16_t)
	TYPE_INT32,						//!< (int32_t)
	TYPE_UINT32,					//!< (uint32_t)
	TYPE_INT64,						//!< (int64_t)
	TYPE_UINT64,					//!< (uint64_t)
	TYPE_PTRDIFF,					//!< (ptrdiff_t)
	TYPE_INTPTR,					//!< (intptr_t)
	TYPE_FLOAT,						//!< (float)
	TYPE_DOUBLE,					//!< (double)
	TYPE_LDOUBLE,					//!< (long double)
	TYPE_CHAR_P,					//!< (char *)
	TYPE_OBJECT_P,					//!< (aObject *)

	/** @name Type flags
	 * Can be XOR:ed to any other type.
	 * @{ */
	TYPE_NAN			= 0x20,		//!< Not a Number
	TYPE_INF			= 0x40,		//!< Infinite (Generally a result of x/0)
	TYPE_NEG			= 0x80,		//!< Negative
	/** @} */
};

/** Should contain a value from the type-enumeration to tell what type a aCollection-value contains. */
typedef unsigned char type_t;

/** @cond */
#define _TYPE_EMPTY ((type_t)TYPE_EMPTY)
#define _TYPE_VOID_P ((type_t)TYPE_VOID_P)
#define _TYPE_INT8 ((type_t)TYPE_INT8)
#define _TYPE_UINT8 ((type_t)TYPE_UINT8)
#define _TYPE_INT16 ((type_t)TYPE_INT16)
#define _TYPE_UINT16 ((type_t)TYPE_UINT16)
#define _TYPE_INT32 ((type_t)TYPE_INT32)
#define _TYPE_UINT32 ((type_t)TYPE_UINT32)
#define _TYPE_INT64 ((type_t)TYPE_INT64)
#define _TYPE_UINT64 ((type_t)TYPE_UINT64)
#define _TYPE_PTRDIFF ((type_t)TYPE_PTRDIFF)
#define _TYPE_INTPTR ((type_t)TYPE_INTPTR)
#define _TYPE_FLOAT ((type_t)TYPE_FLOAT)
#define _TYPE_DOUBLE ((type_t)TYPE_DOUBLE)
#define _TYPE_LDOUBLE ((type_t)TYPE_LDOUBLE)
#define _TYPE_CHAR_P ((type_t)TYPE_CHAR_P)
#define _TYPE_OBJECT_P ((type_t)TYPE_OBJECT_P)
#define _TYPE_NAN ((type_t)TYPE_NAN)
#define _TYPE_INF ((type_t)TYPE_INF)
#define _TYPE_NEG ((type_t)TYPE_NEG)
/** @endcond */

/** Is a variant, it may contain any value from the type-enumeration.
 * Types larger than 32 bits are allocated with malloc and the adress is stored in the value_t instead. */
typedef intptr_t value_t;

/** Contains information about how the aCollection should be handled. */
typedef uint32_t style_t;

/** A parent class for collection classes.
 * 
 * The aCollection-class is the top-class for all types of collections.
 * I decided creating my own set of Container-classes simply because of learning the algorithms and keeping
 * to my principal idea of making everything by hand. I consider programming an artform, so I had no intention
 * of making the most efficient or "best" choice, but simply making as much as possible handmade. Also, I
 * dislike templates because I find them ugly and I wanted my set of aCollection-classes to contain any types
 * and still be relatively type-safe. These classes do work fine and have been thoroughly tested, of course
 * they could be optimized, but that is an ongoing project.
 *
 * The aCollection-class is more or less an abstract class, even though it has no pure virtual methods at the time.
 * It extends the aObject-class so it can be implemented in the class-hierarchy of that class. All subclasses of
 * aCollection obviously has to implement the aObject-class style to maintain rtti-structure, though not doing so
 * only means pointing to the super-class aClass instead.
 * @ingroup libamanita
 */
class aCollection : public aObject {
/** @cond */
RttiObjectInstance(aCollection)
/** @endcond */

protected:
	size_t sz;	//!< Size of the collection.
	size_t cap;	//!< Capacity of the collection.

public:
	/** Constructor. */
	aCollection() : aObject(),sz(0),cap(0) {}

	/** Size of the collection.
	 * @return Size of the collection. */
	size_t size() { return sz; }

	/** Capacity of the collection.
	 * @return Capacity of the collection. */
	size_t capacity() { return cap; }
};

#endif /* _LIBAMANITA_COLLECTION_H */


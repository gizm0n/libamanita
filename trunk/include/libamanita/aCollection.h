#ifndef _LIBAMANITA_ACOLLECTION_H
#define _LIBAMANITA_ACOLLECTION_H

#include <stdint.h>
#include <libamanita/aObject.h>

/** Enumeration used by the iteration class and subclasses for extended iteration positions. */
enum {
	ITER_INDEX_EMPTY			= -4,	/** < Iteration points to an empty value. */
	ITER_INDEX_START			= -3,	/** < Iteration points at the start of the aCollection. */
	ITER_INDEX_AFTER_LAST	= -2,	/** < Iteration points at one step after last value in the aCollection. */
	ITER_INDEX_BEFORE_FIRST	= -1,	/** < Iteration points at one step before first value in the aCollection. */
};

/** Enumeration used by all subclasses of the aCollection class to tell what type a value contains. */
enum {
	_TYPE_EMPTY,			/** < Value is empty, meaning this position in the aCollection is not filled with a value.. */
	_TYPE_VOID_P,			/** < (void *) */
	_TYPE_INT8,				/** < (int8_t) */
	_TYPE_UINT8,			/** < (uint8_t) */
	_TYPE_INT16,			/** < (int16_t) */
	_TYPE_UINT16,			/** < (uint16_t) */
	_TYPE_INT32,			/** < (int32_t) */
	_TYPE_UINT32,			/** < (uint32_t) */
	_TYPE_INT64,			/** < (int64_t) */
	_TYPE_UINT64,			/** < (uint64_t) */
	_TYPE_PTRDIFF,			/** < (ptrdiff_t) */
	_TYPE_INTPTR,			/** < (intptr_t) */
	_TYPE_FLOAT,			/** < (float) */
	_TYPE_DOUBLE,			/** < (double) */
	_TYPE_LDOUBLE,			/** < (long double) */
	_TYPE_CHAR_P,			/** < (char *) */
	_TYPE_OBJECT_P,		/** < (aObject *) */
};

/** type_t should contain a value from the type-enumeration to tell what type a aCollection-value contains. */
typedef unsigned char type_t;

#define TYPE_EMPTY ((type_t)_TYPE_EMPTY)
#define TYPE_VOID_P ((type_t)_TYPE_VOID_P)
#define TYPE_INT8 ((type_t)_TYPE_INT8)
#define TYPE_UINT8 ((type_t)_TYPE_UINT8)
#define TYPE_INT16 ((type_t)_TYPE_INT16)
#define TYPE_UINT16 ((type_t)_TYPE_UINT16)
#define TYPE_INT32 ((type_t)_TYPE_INT32)
#define TYPE_UINT32 ((type_t)_TYPE_UINT32)
#define TYPE_INT64 ((type_t)_TYPE_INT64)
#define TYPE_UINT64 ((type_t)_TYPE_UINT64)
#define TYPE_PTRDIFF ((type_t)_TYPE_PTRDIFF)
#define TYPE_INTPTR ((type_t)_TYPE_INTPTR)
#define TYPE_FLOAT ((type_t)_TYPE_FLOAT)
#define TYPE_DOUBLE ((type_t)_TYPE_DOUBLE)
#define TYPE_LDOUBLE ((type_t)_TYPE_LDOUBLE)
#define TYPE_CHAR_P ((type_t)_TYPE_CHAR_P)
#define TYPE_OBJECT_P ((type_t)_TYPE_OBJECT_P)

/** value_t is a variant, it may contain any value from the type-enumeration.
 * Types larger than 32 bits are allocated with malloc and the adress is stored in the value_t instead. */
typedef intptr_t value_t;

/** style_t contains information about how the aCollection should be handled, e.g. case insensitive. */
typedef uint32_t style_t;

/** The aCollection-class is the top-class for all types of collections.
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
 */
class aCollection : public aObject {
RttiObjectInstance(aCollection)

protected:
	size_t sz;	/** < Size of the collection. */
	size_t cap;	/** < Capacity of the collection. */

public:
	/** Constructor. */
	aCollection() : aObject(),sz(0),cap(0) {}

	size_t size() { return sz; }			/** < Size of the collection. */
	size_t capacity() { return cap; }	/** < Capacity of the collection. */
};

#endif /* _LIBAMANITA_ACOLLECTION_H */


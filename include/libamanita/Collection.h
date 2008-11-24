#ifndef _LIBAMANITA_COLLECTION_H
#define _LIBAMANITA_COLLECTION_H

#include <libamanita/Object.h>

/** Enumeration used by the iteration class and subclasses for extended iteration positions. */
enum {
	ITER_INDEX_EMPTY			= -4,	/** < Iteration points to an empty value. */
	ITER_INDEX_START			= -3,	/** < Iteration points at the start of the Collection. */
	ITER_INDEX_AFTER_LAST	= -2,	/** < Iteration points at one step after last value in the Collection. */
	ITER_INDEX_BEFORE_FIRST	= -1,	/** < Iteration points at one step before first value in the Collection. */
};

/** Enumeration used by all subclasses of the Collection class to tell what type a value contains. */
enum {
	_TYPE_EMPTY,			/** < Value is empty, meaning this position in the Collection is not filled with a value.. */
	_TYPE_VOID_P,			/** < (void *) */
	_TYPE_INT8,				/** < (char) */
	_TYPE_UINT8,			/** < (unsigned char) */
	_TYPE_INT16,			/** < (short) */
	_TYPE_UINT16,			/** < (unsigned short) */
	_TYPE_INT32,			/** < (long) */
	_TYPE_UINT32,			/** < (unsigned long) */
	_TYPE_INT64,			/** < (long long) */
	_TYPE_UINT64,			/** < (unsigned long long) */
	_TYPE_FLOAT,			/** < (float) */
	_TYPE_DOUBLE,			/** < (double) */
	_TYPE_LONG_DOUBLE,	/** < (long double) */
	_TYPE_CHAR_P,			/** < (char *) */
	_TYPE_OBJECT_P,		/** < (Object *) */
};

/** type_t should contain a value from the type-enumeration to tell what type a Collection-value contains. */
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
#define TYPE_FLOAT ((type_t)_TYPE_FLOAT)
#define TYPE_DOUBLE ((type_t)_TYPE_DOUBLE)
#define TYPE_LONG_DOUBLE ((type_t)_TYPE_LONG_DOUBLE)
#define TYPE_CHAR_P ((type_t)_TYPE_CHAR_P)
#define TYPE_OBJECT_P ((type_t)_TYPE_OBJECT_P)

/** value_t is a variant, it may contain any value from the type-enumeration.
 * Types larger than 32 bits are allocated with malloc and the adress is stored in the value_t instead. */
typedef unsigned long value_t;

/** style_t contains information about how the Collection should be handled, e.g. case incensitive. */
typedef unsigned long style_t;

/** The Collection-class is the top-class for all types of collections.
 * I decided creating my own set of Container-classes simply because of learning the algorithms and keeping
 * to my principal idea of making everything by hand. I consider programming an artform, so I had no intention
 * of making the most efficient or "best" choice, but simply making as much as possible handmade. Also, I
 * dislike templates because I find them ugly and I wanted my set of Collection-classes to contain any types
 * and still be relatively type-safe. These classes do work fine and have been thoroughly tested, of course
 * they could be optimized, but that is an ongoing project.
 *
 * The Collection-class is more or less an abstract class, even though it has no pure virtual methods at the time.
 * It extends the Object-class so it can be implemented in the class-hierarchy of that class. All subclasses of
 * Collection obviously has to implement the Object-class style to maintain rtti-structure, though not doing so
 * only means pointing to the super-class Class instead.
 */
class Collection : public Object {
RttiObjectInstance(Collection)

protected:
	size_t sz;	/** < Size of the collection. */
	size_t cap;	/** < Capacity of the collection. */

public:
	/** Constructor. */
	Collection() : Object(),sz(0ul),cap(0ul) {}

	size_t size() { return sz; }			/** < Size of the collection. */
	size_t capacity() { return cap; }	/** < Capacity of the collection. */
};

#endif /* _LIBAMANITA_COLLECTION_H */


#ifndef _AMANITA_COLLECTION_H
#define _AMANITA_COLLECTION_H

/**
 * @file amanita/aCollection.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <stdint.h>
#include <amanita/aObject.h>

enum {
	aHASH_STYLE_CASE_SENSITIVE		= 0,	//!< 
	aHASH_STYLE_CASE_INSENSITIVE	= 1,	//!< 
	aHASH_STYLE_KEY_MULTIPLES		= 2,	//!< 
};

/** Enumeration used by the iteration class and subclasses for extended iteration positions. */
enum {
	aITER_INDEX_EMPTY			= -4,			//!< Iteration points to an empty value.
	aITER_INDEX_START			= -3,			//!< Iteration points at the start of the aCollection.
	aITER_INDEX_AFTER_LAST	= -2,			//!< Iteration points at one step after last value in the aCollection.
	aITER_INDEX_BEFORE_FIRST	= -1,		//!< Iteration points at one step before first value in the aCollection.
};

/** Enumeration used by all subclasses of the aCollection class to tell what type a value contains. */
enum {
	aEMPTY			= 0x00,		//!< Value is empty, meaning this position in the aCollection is not filled with a value..
	aVOID,							//!< (void *)
	aINT8,							//!< (int8_t)
	aUINT8,							//!< (uint8_t)
	aINT16,							//!< (int16_t)
	aUINT16,							//!< (uint16_t)
	aINT32,							//!< (int32_t)
	aUINT32,							//!< (uint32_t)
	aINT64,							//!< (int64_t)
	aUINT64,							//!< (uint64_t)
	aPTRDIFF,						//!< (ptrdiff_t)
	aINTPTR,							//!< (intptr_t)
	aFLOAT,							//!< (float)
	aDOUBLE,							//!< (double)
	aLDOUBLE,						//!< (long double)
	aCHAR_P,							//!< (char *)
	aOBJECT,							//!< (aObject *)
	aSTRING,							//!< (aString *)
	aVECTOR,							//!< (aVector *)
	aHASHTABLE,						//!< (aHashtable *)

	/** @name Type flags
	 * Can be XOR:ed to any other type.
	 * @{ */
	aNAN			= 0x20,			//!< Not a Number
	aINF			= 0x40,			//!< Infinite (Generally a result of x/0)
	aNEG			= 0x80,			//!< Negative
	/** @} */
};

/** Should contain a value from the type-enumeration to tell what type a aCollection-value contains. */
typedef unsigned char type_t;

/** @cond */
#define _aEMPTY ((type_t)aEMPTY)
#define _aVOID ((type_t)aVOID)
#define _aINT8 ((type_t)aINT8)
#define _aUINT8 ((type_t)aUINT8)
#define _aINT16 ((type_t)aINT16)
#define _aUINT16 ((type_t)aUINT16)
#define _aINT32 ((type_t)aINT32)
#define _aUINT32 ((type_t)aUINT32)
#define _aINT64 ((type_t)aINT64)
#define _aUINT64 ((type_t)aUINT64)
#define _aPTRDIFF ((type_t)aPTRDIFF)
#define _aINTPTR ((type_t)aINTPTR)
#define _aFLOAT ((type_t)aFLOAT)
#define _aDOUBLE ((type_t)aDOUBLE)
#define _aLDOUBLE ((type_t)aLDOUBLE)
#define _aCHAR_P ((type_t)aCHAR_P)
#define _aOBJECT ((type_t)aOBJECT)
#define _aSTRING ((type_t)aSTRING)
#define _aVECTOR ((type_t)aVECTOR)
#define _aHASHTABLE ((type_t)aHASHTABLE)
#define _aNAN ((type_t)aNAN)
#define _aINF ((type_t)aINF)
#define _aNEG ((type_t)aNEG)
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
 * @ingroup amanita */
class aCollection : public aObject {
/** @cond */
aObject_Instance(aCollection)
/** @endcond */

protected:
	size_t sz;	//!< Size of the collection.
	size_t cap;	//!< Capacity of the collection.

public:
	/** Constructor. */
	aCollection() : aObject(),sz(0),cap(0) {}

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

#endif /* _AMANITA_COLLECTION_H */


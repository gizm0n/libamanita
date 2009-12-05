#ifndef _LIBAMANITA_OBJECT_H
#define _LIBAMANITA_OBJECT_H

/**
 * @file libamanita/aObject.h  
 * @author Per Löwgren
 * @date Modified: 2009-12-02
 * @date Created: 2008-09-07
 */ 

#include <stddef.h>
#include <stdint.h>


/** A class for handling RTTI.
 * 
 * All classes that inherit the aObject class get by default a static instance of the
 * aClass that specifically defines that class and place it in a treelike hierarchy.
 * @ingroup libamanita
 */
class aClass {
friend class aObject;

private:
	static uint32_t id_index;		//!< Static counter for new unique IDs.

	uint32_t id;						//!< Unique ID for this class.
	const char *name;					//!< Name of class.
	int d;								//!< Depth of class in hierarchy.
	aClass *super;						//!< Parent class that this class inherit from.
	size_t nsub;						//!< Number of child classes that inherit from this class.
	aClass **sub;						//!< Child classes that inherit from this class.

	void setDepth(int n);			//!< Set depth of class
	void print(char *ind);			//!< Print class to stdout.

public:
	aClass(const char *nm,aClass *s);
	~aClass();

	bool instanceOf(aClass &c);						//!< Test if this is an instance of c, that is, if this is the same or inherit c.
	uint32_t getID() { return id; }					//!< Get ID for class.
	const char *getName() { return name; }			//!< Get class name.
	aClass *getSuperClass() { return super; }		//!< Get class that this class inherit from.
	operator uint32_t() const { return id; }		//!< Get ID for class.
};

/** Is used by the aHashtable class to store a hash-values, and to get a hash-value from the aObject-class. */
typedef intptr_t hash_t;

/** @name RTTI Macros
 * These macros are very important for classes that inherit the aObject class.
 * Be careful to read their instructions.
 * @{ */
/** In the class definition for every class inheriting the aObject class must be
 * a RttiObjectInstance macro somewhere. For example:
 * @code
// File: A.h
#include <libamanita/aObject.h>

class A : public aObject {
RttiObjectInstance(A)

private:
	int a;

public:
	A() : aObject() { a = 0; }
};
 * @endcode
 * If this is omitted, the could be runtime errors that are difficult to trace.
 * 
 * Note, it should not be ended with a semicolon.
 * @param class_name This must be the name of the class, as it is. No quotes or anything. */
#define RttiObjectInstance(class_name) \
private:\
	static aClass instance;\
	virtual aClass &getInstance() { return class_name::instance; }\
public:\
	static aClass &getClass() { return class_name::instance; }

/** This macro must be somewhere in the file containing the code for the class.
 * For example:
 * @code
// File: A.cpp
#include "A.h"

RttiObjectInheritance(A,aObject)
 * @endcode
 * Note, this should end with a semicolon.
 * @param class_name This must be the name of the class, as it is. No quotes or anything.
 * @param super_name This must be the name of the class that this class inherit. */
#define RttiObjectInheritance(class_name,super_name) \
aClass class_name::instance(#class_name,&super_name::getClass())
/** @} */

/** The aObject-class is the top-class of all objects that need to be defined within a class-structure with
 * a simple RTTI-interface.
 * 
 * When inheriting this class, it is very important that you include the RttiObjectInheritance macro in
 * your class definition and the other one in the source file somewhere.
 * 
 * All classes that inherit the aObject class and adds these macros will have one ststic aClass instance
 * attached to the class, and one virtual method that returns the static aClass instance specific for
 * that class. This way no extra data is needed when creating objects to store RTTI-data, and very
 * little overhead is added.
 * 
 * So, whenever you need to make some test to see if one class inherits another or similar,
 * all you need to do is call a virtual method that returns a static aClass instance. As you see
 * it's pretty quick.
 * @see RttiObjectInstance(class_name)
 * @see RttiObjectInheritance(class_name,super_name)
 * @ingroup libamanita
 */
class aObject {
private:
	/** Statically stored instance of aClass. */
	static aClass instance;

	/** Virtual method to get the statically stored aClass instance for this class. */
	virtual aClass &getInstance() { return aObject::instance; }

public:
	/** Get the aClass object statically attached to this class. */
	static aClass &getClass() { return aObject::instance; }

	/** @name Constructors and Destructors
	 * @{ */
	aObject() {}					//!< Constructor
	virtual ~aObject() {}		//!< Destructor
	/** @} */

	/** @name Instance of
	 * @{ */
	/** Test if this is an instance of c, that is, if this is the same or inherit c. */
	bool instanceOf(aClass *c) { return c && getInstance().instanceOf(*c); }

	/** Test if this is an instance of c, that is, if this is the same or inherit c. */
	bool instanceOf(aClass &c) { return getInstance().instanceOf(c); }

	/** Test if this is an instance of o, that is, if this is the same or inherit c. */
	bool instanceOf(aObject *o) { return o && getInstance().instanceOf(o->getInstance()); }

	/** Test if this is an instance of o, that is, if this is the same or inherit c. */
	bool instanceOf(aObject &o) { return getInstance().instanceOf(o.getInstance()); }
	/** @} */

	/** @name Operators
	 * @{ */
	operator aClass &() { return getInstance(); }						//!< Get aClass instance.
	bool operator==(aClass &c) { return getInstance().id==c.id; }	//!< Compare if this class equals c.
	bool operator!=(aClass &c) { return getInstance().id!=c.id; }	//!< Compare if this class isn't equal c.

	virtual operator char *() const { return 0; }						//!< Virtual method to return aObject as a char array.
	/** @} */

	/** @name Hash
	 * @{ */
	virtual hash_t hash() const { return (unsigned long)this; }
	operator hash_t() const { return hash(); }
	/** @} */
};


#endif /* _LIBAMANITA_OBJECT_H */


#ifndef _AMANITA_OBJECT_H
#define _AMANITA_OBJECT_H

/**
 * @file amanita/aObject.h  
 * @author Per Löwgren
 * @date Modified: 2010-08-01
 * @date Created: 2008-09-07
 */ 

#include <stddef.h>
#include <stdint.h>




/** A class for handling RTTI.
 * 
 * All classes that inherit the aObject class get by default a static instance of the
 * aClass that specifically defines that class and place it in a treelike hierarchy.
 *
 * When instantiated statically with aObject, the aClass object is created before
 * main is called and therefore it can be used safely itn the code except for in
 * other static code.
 * @ingroup amanita
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
	/** Get class that this class inherit from. */
	aClass *getSuperClass() { return super; }
	size_t subClasses() { return nsub; }			//!< Get number of inheriting classes.
	/** Get the class inheriting this class with index n. */
	aClass *getSubClass(int n) { return n>=0 && n<(int)nsub? sub[n] : 0; }
	operator uint32_t() const { return id; }		//!< Get ID for class.
};



/** Is used by the aHashtable class to store a hash-values, and to get a hash-value from the aObject-class. */
typedef intptr_t hash_t;



/** @name RTTI Macros
 * These macros are very important for classes that inherit the aObject class.
 * Be careful to read their instructions.
 * @{ */
/** In the class definition for every class inheriting the aObject class must be
 * a aObject_Instance macro somewhere. For example:
 * @code
// File: A.h
#include <amanita/aObject.h>

class A : public aObject {
aObject_Instance(A)

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
#define aObject_Instance(class_name) \
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

aObject_Inheritance(A,aObject)
 * @endcode
 * Note, this should end with a semicolon.
 * @param class_name This must be the name of the class, as it is. No quotes or anything.
 * @param super_name This must be the name of the class that this class inherit. */
#define aObject_Inheritance(class_name,super_name) \
aClass class_name::instance(#class_name,&super_name::getClass())

/* Work in progress: These macros will implement capability for multiple inheritance. */
/*#define aObject_SuperClass(class_name) \
	&class_name::getClass(),
#define aObject_MultipleInheritance(class_name,super_names) \
aClass class_name::instance(#class_name,super_names,0) */

/* Work in progress: This macro is planned to replace aObject_Inheritance. It will
 * take a complete declaration of the class as a string and extract metadata for the
 * class to implement reflection for the class. */
/*#define aObject_Declaration(class_name,declaration) \
aClass class_name::instance(#class_name,declaration) */
/** @} */




/** The aObject-class implements a smple RTTI-interface. Any class inheriting this class
 * must also implement a certain structure, which is managed with two very simple to
 * use macros.
 * 
 * Note that at the moment multiple inheritance if not supported. It's possible though
 * to have multiple inheritance with classes not inheriting the aObject class.
 *
 * When inheriting this class, it is very important that you include the
 * aObject_Instance macro in your class definition and the aObject_Inheritance macro
 * in the source file somewhere.
 * 
 * All classes that inherit the aObject class and adds these macros will have one ststic aClass instance
 * attached to the class, and one virtual method that returns the static aClass instance specific for
 * that class. This way no extra data is needed when creating objects to store RTTI-data, and very
 * little overhead is added.
 * 
 * So, whenever you need to make some test to see if one class inherits another or similar,
 * all you need to do is call a virtual method that returns a static aClass instance. As you see
 * it's pretty quick.
 *
 * Here's an example of how you can check inheritance and even get the name of the class
 * or the name of the class this class inherits from, and all classes inheriting from this class:
 * @code
// File: rtti.cpp
#include <stdio.h>
#include <amanita/aObject.h>

class A : public aObject {
aObject_Instance(A) // Note that this macro must not be ended with a semicolon.
public:
	int a;
	A() : aObject() { a = 0; }
	virtual ~A() {}
};

class B : public A {
aObject_Instance(B)
public:
	int b;
	B() : A() { b = 0; }
	virtual ~B() {}
};

class C {
public:
	int c;
	C() { c = 0; }
	virtual ~C() {}
};

class D : public B, public C {
aObject_Instance(D)
public:
	int d;
	D() : B(),C() { d = 0; }
	virtual ~D() {}
};

aObject_Inheritance(A,aObject); // Note that this macro must be ended with a semicolon.
aObject_Inheritance(B,A);
aObject_Inheritance(D,B);

int main(int argc,char *argv[]) {
	B b;
	D d;
	if(b.instanceOf(d)) printf("%s is instance of %s.\n",
		b.getClass().getName(),d.getClass().getName());
	if(d.instanceOf(b)) printf("%s is instance of %s.\n",
		d.getClass().getName(),b.getClass().getName());
	// Note that at present time you cannot test instance of the aObject class
	// itself, but must use the static method getClass().
	if(b.instanceOf(A::getClass())) printf("%s is instance of %s.\n",
		b.getClass().getName(),A::getClass().getName());
	printf("d is of class %s and inherits %s.\n",
		d.getClass().getName(),d.getClass().getSuperClass()->getName());
	printf("This/these class(es) inherit b:\n");
	for(int i=0,n=b.getClass().subClasses(); i<n; i++)
		printf(" %d. %s\n",i+1,b.getClass().getSubClass(i)->getName());
	printf("class %s inherits %s.\n",
		A::getClass().getName(),A::getClass().getSuperClass()->getName());
	printf("This/these class(es) inherit the class A:\n");
	for(int i=0,n=A::getClass().subClasses(); i<n; i++)
		printf(" %d. %s\n",i+1,A::getClass().getSubClass(i)->getName());
}

 * @endcode
 * Compile the above code like this:
 * @code
 * g++ -o rtti rtti.cpp -lamanita
 * @endcode
 * It will generate the following output:
 * @code
D is instance of B.
B is instance of A.
d is of class D and inherits B.
This/these class(es) inherit b:
 1. D
class A inherits aObject.
This/these class(es) inherit the class A:
 1. B
 * @endcode
 * @see aObject_Instance(class_name)
 * @see aObject_Inheritance(class_name,super_name)
 * @ingroup amanita
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


#endif /* _AMANITA_OBJECT_H */


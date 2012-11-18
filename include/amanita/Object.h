#ifndef _AMANITA_OBJECT_H
#define _AMANITA_OBJECT_H

/**
 * @file amanita/Object.h  
 * @author Per Löwgren
 * @date Modified: 2012-10-21
 * @date Created: 2008-09-07
 */ 

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


/** Amanita Namespace */
namespace a {


/** A class for handling RTTI.
 * 
 * All classes that inherit the Object class get by default a static instance of the
 * Class that specifically defines that class and place it in a treelike hierarchy.
 *
 * When instantiated statically with Object, the Class object is created before
 * main is called and therefore it can be used safely itn the code except for in
 * other static code.
 * 
 * @ingroup amanita */
class Class {
friend class Object;

private:
	static uint16_t id_index;		//!< Static counter for new unique IDs.

	uint16_t id;						//!< Unique ID for this class.
	const char *name;					//!< Name of class.
//	int d;								//!< Depth of class in hierarchy.
	Class *super;						//!< Parent class that this class inherit from.
	uint16_t nsub;						//!< Number of child classes that inherit from this class.
	Class **sub;						//!< Child classes that inherit from this class.

//	void setDepth(int n);			//!< Set depth of class

	/** Print class to fp, where ind is indentation and d depth. */
	void print(FILE *fp,char *ind,int d);			

public:
	Class(const char *nm,Class *s);
	~Class();

	bool instanceOf(Class &c);							//!< Test if this is an instance of c, that is, if this is the same or inherit c.
	uint16_t getID() { return id; }					//!< Get ID for class.
	const char *getName() { return name; }			//!< Get class name.
	/** Get class that this class inherit from. */
	Class *getSuperClass() { return super; }
	size_t subClasses() { return nsub; }			//!< Get number of inheriting classes.
	/** Get the class inheriting this class with index n. */
	Class *getSubClass(int n) { return n>=0 && n<(int)nsub? sub[n] : 0; }
	operator uint16_t() const { return id; }		//!< Get ID for class.
};



/** Is used by the Hashtable class to store a hash-values, and to get a hash-value from the Object-class. */
typedef uint32_t hash_t;



/** @name RTTI Macros
 * These macros are very important for classes that inherit the Object class.
 * Be careful to read their instructions.
 * @{ */
/** In the class definition for every class inheriting the Object class must be
 * a Object_Instance macro somewhere. For example:
 * @code
// File: A.h
#include <amanita/Object.h>

class A : public Object {
Object_Instance(A)

private:
	int a;

public:
	A() : Object() { a = 0; }
};
 * @endcode
 * If this is omitted, there could be runtime errors that are difficult to trace.
 * 
 * Note, this macro should not be ended with a semicolon.
 * @param class_name This must be the name of the class, as it is. No quotes or anything. */
#define Object_Instance(class_name) \
public:\
	static a::Class &getClass() { return class_name::instance; }\
	virtual a::Class &getInstance() { return class_name::instance; }\
private:\
	static a::Class instance;



/** This macro must be somewhere in the file containing the code for the class.
 * For example:
 * @code
// File: A.cpp
#include "A.h"

Object_Inheritance(A,Object)
 * @endcode
 * Note, this macro should not end with a semicolon.
 * @param class_name This must be the name of the class, as it is. No quotes or anything.
 * @param super_name This must be the name of the class that this class inherit. */
#define Object_Inheritance(class_name,super_name) \
a::Class class_name::instance(#class_name,&super_name::getClass());

/* Work in progress: These macros will implement capability for multiple inheritance. */
/*#define aObject_SuperClass(class_name) \
	&class_name::getClass(),
#define aObject_MultipleInheritance(class_name,super_names) \
Class class_name::instance(#class_name,super_names,0) */

/* Work in progress: This macro is planned to replace Object_Inheritance. It will
 * take a complete declaration of the class as a string and extract metadata for the
 * class to implement reflection for the class. */
/*#define aObject_Declaration(class_name,declaration) \
Class class_name::instance(#class_name,declaration) */
/** @} */




/** The Object-class implements a smple RTTI-interface. Any class inheriting this class
 * must also implement a certain structure, which is managed with two very simple to
 * use macros.
 * 
 * Note that at the moment multiple inheritance if not supported. It's possible though
 * to have multiple inheritance with classes not inheriting the Object class.
 *
 * When inheriting this class, it is very important that you include the
 * Object_Instance macro in your class definition and the Object_Inheritance macro
 * in the source file somewhere.
 * 
 * All classes that inherit the Object class and adds these macros will have one ststic Class instance
 * attached to the class, and one virtual method that returns the static Class instance specific for
 * that class. This way no extra data is needed when creating objects to store RTTI-data, and very
 * little overhead is added.
 * 
 * So, whenever you need to make some test to see if one class inherits another or similar,
 * all you need to do is call a virtual method that returns a static Class instance. As you see
 * it's pretty quick.
 *
 * Here's an example of how you can check inheritance and even get the name of the class
 * or the name of the class this class inherits from, and all classes inheriting from this class:
 * @code
// File: rtti.cpp
#include <stdio.h>
#include <amanita/Object.h>

class A : public Object {
Object_Instance(A) // Note that this macro must not be ended with a semicolon.
public:
	int a;
	A() : Object() { a = 0; }
	virtual ~A() {}
};

class B : public A {
Object_Instance(B)
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
Object_Instance(D)
public:
	int d;
	D() : B(),C() { d = 0; }
	virtual ~D() {}
};

Object_Inheritance(A,Object); // Note that this macro must be ended with a semicolon.
Object_Inheritance(B,A);
Object_Inheritance(D,B);

int main(int argc,char *argv[]) {
	B b;
	D d;
	if(b.instanceOf(d)) printf("%s is instance of %s.\n",
		b.getClass().getName(),d.getClass().getName());
	if(d.instanceOf(b)) printf("%s is instance of %s.\n",
		d.getClass().getName(),b.getClass().getName());
	// Note that at present time you cannot test instance of the Object class
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
class A inherits Object.
This/these class(es) inherit the class A:
 1. B
 * @endcode
 * @see Object_Instance(class_name)
 * @see Object_Inheritance(class_name,super_name)
 * 
 * @ingroup amanita */
class Object {
private:
	/** Statically stored instance of Class. */
	static Class instance;

public:
	/** Get the Class object statically attached to this class. */
	static Class &getClass() { return Object::instance; }

	/** Print all classes inheriting Object to fp. */
	static void printClasses(FILE *fp);

	/** @name Constructors and Destructors
	 * @{ */
	Object() {}					//!< Constructor
	virtual ~Object() {}		//!< Destructor
	/** @} */

	/** Virtual method to get the statically stored Class instance for this class. */
	virtual Class &getInstance() { return Object::instance; }

	/** @name Instance of
	 * @{ */
	/** Test if this is an instance of c, that is, if this is the same or inherit c. */
	bool instanceOf(Class *c) { return c && getInstance().instanceOf(*c); }

	/** Test if this is an instance of c, that is, if this is the same or inherit c. */
	bool instanceOf(Class &c) { return getInstance().instanceOf(c); }

	/** Test if this is an instance of o, that is, if this is the same or inherit c. */
	bool instanceOf(Object *o) { return o && getInstance().instanceOf(o->getInstance()); }

	/** Test if this is an instance of o, that is, if this is the same or inherit c. */
	bool instanceOf(Object &o) { return getInstance().instanceOf(o.getInstance()); }
	/** @} */

	/** @name Operators
	 * @{ */
	operator Class &() { return getInstance(); }						//!< Get Class instance.
	bool operator==(Class &c) { return getInstance().id==c.id; }	//!< Compare if this class equals c.
	bool operator!=(Class &c) { return getInstance().id!=c.id; }	//!< Compare if this class isn't equal c.

	virtual operator char *() const { return 0; }						//!< Virtual method to return Object as a char array.
	/** @} */

	/** @name Hash
	 * @{ */
	virtual hash_t hash() const { return *(hash_t *)this; }
	operator hash_t() const { return hash(); }
	/** @} */
};

}; /* namespace a */


#endif /* _AMANITA_OBJECT_H */


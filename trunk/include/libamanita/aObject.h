#ifndef _LIBAMANITA_OBJECT_H
#define _LIBAMANITA_OBJECT_H

#include <stddef.h>
#include <stdint.h>


class Class {
friend class Object;

private:
	static uint32_t id_index;

	uint32_t id;
	const char *name;
	int d;
	Class *super,**sub;
	size_t nsub;

	void setDepth(int n);
	void print(char *ind);

public:
	Class(const char *nm,Class *s);
	~Class();
	bool instanceOf(Class &c);
	uint32_t getID() { return id; }
	const char *getName() { return name; }
	Class *getSuperClass() { return super; }
	operator uint32_t() const { return id; }
};

/** hash_t is used by the Hashtable class to store a hash-values, and to get a hash-value from the Object-class. */
typedef intptr_t hash_t;

#define RttiObjectInstance(class_name) \
	private:\
		static Class instance;\
		virtual Class &getInstance() { return class_name::instance; }\
	public:\
		static Class &getClass() { return class_name::instance; }

#define RttiObjectInheritance(class_name,super_name) \
Class class_name::instance(#class_name,&super_name::getClass())


/** The Object-class is the top-class of all objects that need to be defined within a class-structure with
 * a simple RTTI-interface. */
class Object {
private:
	static Class instance;
	virtual Class &getInstance() { return Object::instance; }

public:
	static Class &getClass() { return Object::instance; }

	Object() {}
	virtual ~Object() {}

	bool instanceOf(Class *c) { return c && getInstance().instanceOf(*c); }
	bool instanceOf(Class &c) { return getInstance().instanceOf(c); }
	bool instanceOf(Object *o) { return o && getInstance().instanceOf(o->getInstance()); }
	bool instanceOf(Object &o) { return getInstance().instanceOf(o.getInstance()); }

	operator Class &() { return getInstance(); }
	bool operator==(Class &c) { return getInstance().id==c.id; }
	bool operator!=(Class &c) { return getInstance().id!=c.id; }

	virtual hash_t hash() const { return (unsigned long)this; }
	operator hash_t() const { return hash(); }

	virtual operator char *() const { return 0; }
};


#endif /* _LIBAMANITA_OBJECT_H */


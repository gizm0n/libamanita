#ifndef _LIBAMANITA_AOBJECT_H
#define _LIBAMANITA_AOBJECT_H

#include <stddef.h>
#include <stdint.h>


class aClass {
friend class aObject;

private:
	static uint32_t id_index;

	uint32_t id;
	const char *name;
	int d;
	aClass *super,**sub;
	size_t nsub;

	void setDepth(int n);
	void print(char *ind);

public:
	aClass(const char *nm,aClass *s);
	~aClass();
	bool instanceOf(aClass &c);
	uint32_t getID() { return id; }
	const char *getName() { return name; }
	aClass *getSuperClass() { return super; }
	operator uint32_t() const { return id; }
};

/** hash_t is used by the aHashtable class to store a hash-values, and to get a hash-value from the aObject-class. */
typedef intptr_t hash_t;

#define RttiObjectInstance(class_name) \
private:\
	static aClass instance;\
	virtual aClass &getInstance() { return class_name::instance; }\
public:\
	static aClass &getClass() { return class_name::instance; }

#define RttiObjectInheritance(class_name,super_name) \
aClass class_name::instance(#class_name,&super_name::getClass())


/** The aObject-class is the top-class of all objects that need to be defined within a class-structure with
 * a simple RTTI-interface. */
class aObject {
private:
	static aClass instance;
	virtual aClass &getInstance() { return aObject::instance; }

public:
	static aClass &getClass() { return aObject::instance; }

	aObject() {}
	virtual ~aObject() {}

	bool instanceOf(aClass *c) { return c && getInstance().instanceOf(*c); }
	bool instanceOf(aClass &c) { return getInstance().instanceOf(c); }
	bool instanceOf(aObject *o) { return o && getInstance().instanceOf(o->getInstance()); }
	bool instanceOf(aObject &o) { return getInstance().instanceOf(o.getInstance()); }

	operator aClass &() { return getInstance(); }
	bool operator==(aClass &c) { return getInstance().id==c.id; }
	bool operator!=(aClass &c) { return getInstance().id!=c.id; }

	virtual hash_t hash() const { return (unsigned long)this; }
	operator hash_t() const { return hash(); }

	virtual operator char *() const { return 0; }
};


#endif /* _LIBAMANITA_AOBJECT_H */


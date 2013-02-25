#ifndef _AMANITA_LIST_H
#define _AMANITA_LIST_H

/**
 * @file amanita/List.h  
 * @author Per Löwgren
 * @date Modified: 2013-02-23
 * @date Created: 2013-02-23
 */ 

#include <stdint.h>
#include <amanita/Collection.h>
#include <amanita/Config.h>
#include <amanita/String.h>

/** @cond */
#if _WORDSIZE == 64
#define _LST_DBLDUP(d) *((value_t *)((void *)&d))
#else
#define _LST_DBLDUP(d) dbldup(d)
#endif
#define _LST_DOUBLE_VALUE(v) *((value_t *)((void *)&v))
/** @endcond */


/** Amanita Namespace */
namespace a {


/** A generic multitype linked list.
 * 
 * Like with the Vector and Hashtable classes, strings (and in 32-bit systems also double) are allocated and stored inside the structure.
 * 
 * @ingroup amanita */
class List : public Collection {
/** @cond */
Object_Instance(List)
/** @endcond */

private:
	struct node {
		value_t value;
		type_t type;
		node *prev;
		node *next;

		node(value_t v,type_t t,node *p,node *n) : value(v),type(t),prev(p),next(n) {}
		~node() { clear(); }
		void clear() {
			if(value && (type==CHAR_P
#if _WORDSIZE < 64
				|| type==DOUBLE
#endif
					)) free((void *)value);
		}
	};
	node *first;
	node *last;

	size_t unshift(value_t v,type_t t);
	size_t push(value_t v,type_t t);
	size_t remove(value_t v,type_t t);

public:
	List();
	virtual ~List();

	/** @name Push operator
	 * @{ */
	List &operator<<(void *v) { push((value_t)v,VOID_P);return *this; }
	List &operator<<(long v) { push((value_t)v,INTPTR);return *this; }
	List &operator<<(unsigned long v) { push((value_t)v,INTPTR);return *this; }
	List &operator<<(float v) { double d = v;push((value_t)_LST_DBLDUP(d),DOUBLE);return *this; }
	List &operator<<(double v) { push((value_t)_LST_DBLDUP(v),DOUBLE);return *this; }
	List &operator<<(const char *v) { push((value_t)strdup(v),CHAR_P);return *this; }
	List &operator<<(const char **v) { push(v);return *this; }
	List &operator<<(const String *v) { push((value_t)strdup(*v),CHAR_P);return *this; }
	List &operator<<(const String &v) { push((value_t)strdup(v),CHAR_P);return *this; }
	List &operator<<(const Object *v) { push((value_t)v,OBJECT);return *this; }
	List &operator<<(const Object &v) { push((value_t)&v,OBJECT);return *this; }
	/** @} */

	/** @name Unshift operator
	 * @{ */
	List &operator>>(void *v) { unshift((value_t)v,VOID_P);return *this; }
	List &operator>>(long v) { unshift((value_t)v,INTPTR);return *this; }
	List &operator>>(unsigned long v) { unshift((value_t)v,INTPTR);return *this; }
	List &operator>>(float v) { double d = v;unshift((value_t)_LST_DBLDUP(d),DOUBLE);return *this; }
	List &operator>>(double v) { unshift((value_t)_LST_DBLDUP(v),DOUBLE);return *this; }
	List &operator>>(const char *v) { unshift((value_t)strdup(v),CHAR_P);return *this; }
	List &operator>>(const char **v) { unshift(v);return *this; }
	List &operator>>(const String *v) { unshift((value_t)strdup(*v),CHAR_P);return *this; }
	List &operator>>(const String &v) { unshift((value_t)strdup(v),CHAR_P);return *this; }
	List &operator>>(const Object *v) { unshift((value_t)v,OBJECT);return *this; }
	List &operator>>(const Object &v) { unshift((value_t)&v,OBJECT);return *this; }
	/** @} */

	/** @name Insert methods
	 * @{ */
	size_t push(void *v) { return push((value_t)v,VOID_P); }
	size_t push(long v) { return push((value_t)v,INTPTR); }
	size_t push(unsigned long v) { return push((value_t)v,INTPTR); }
	size_t push(float v) { double d = v;return push((value_t)_LST_DBLDUP(d),DOUBLE); }
	size_t push(double v) { return push((value_t)_LST_DBLDUP(v),DOUBLE); }
	size_t push(const char *v) { return push((value_t)strdup(v),CHAR_P); }
	size_t push(const char **v);
	size_t push(const String *v) { return push((value_t)strdup(*v),CHAR_P); }
	size_t push(const String &v) { return push((value_t)strdup(v),CHAR_P); }
	size_t push(const Object *v) { return push((value_t)v,OBJECT); }
	size_t push(const Object &v) { return push((value_t)&v,OBJECT); }

	size_t unshift(void *v) { return unshift((value_t)v,VOID_P); }
	size_t unshift(long v) { return unshift((value_t)v,INTPTR); }
	size_t unshift(unsigned long v) { return unshift((value_t)v,INTPTR); }
	size_t unshift(float v) { double d = v;return unshift((value_t)_LST_DBLDUP(d),DOUBLE); }
	size_t unshift(double v) { return unshift((value_t)_LST_DBLDUP(v),DOUBLE); }
	size_t unshift(const char *v) { return unshift((value_t)strdup(v),CHAR_P); }
	size_t unshift(const char **v);
	size_t unshift(const String *v) { return unshift((value_t)strdup(*v),CHAR_P); }
	size_t unshift(const String &v) { return unshift((value_t)strdup(v),CHAR_P); }
	size_t unshift(const Object *v) { return unshift((value_t)v,OBJECT); }
	size_t unshift(const Object &v) { return unshift((value_t)&v,OBJECT); }
	/** @} */

	/** @name Shift, Pop & Peek methods
	 * @{ */
	value_t pop();
	value_t shift();
	/** @} */

	/** @name Remove methods
	 * @{ */
	size_t remove(void *v) { return remove((value_t)v,VOID_P); }
	size_t remove(long v) { return remove((value_t)v,INTPTR); }
	size_t remove(unsigned long v) { return remove((value_t)v,INTPTR); }
	size_t remove(float v) { double d = v;return remove(d); }
	size_t remove(double v);
	size_t remove(const char *v);
	size_t remove(const char **v);
	size_t remove(const String *v) { return remove((const char *)*v); }
	size_t remove(const String &v) { return remove((const char *)v); }
	size_t remove(const Object *v) { return remove((value_t)v,OBJECT); }
	size_t remove(const Object &v) { return remove((value_t)&v,OBJECT); }

	void removeAt(long n);
	void removeAll() { clear(); }

	void clear();
	/** @} */
};

}; /* namespace a */



#endif /* _AMANITA_LIST_H */




#ifndef _LIBAMANITA_VECTOR_H
#define _LIBAMANITA_VECTOR_H

#include <stdint.h>
#include <libamanita/Collection.h>
#include <libamanita/Object.h>
#include <libamanita/String.h>

class Hashtable;

class Vector : public Collection {
RttiObjectInstance(Vector)

friend class Hashtable;

private:
	struct node {
		value_t value;
		type_t type;
	};
	size_t inc;
	node *list;

	Vector &insert(value_t v,long n,type_t t);
	Vector &remove(value_t v,type_t t);
	long indexOf(value_t v,type_t t);

public:
	class iterator {
		friend class Vector;

		private:
			Vector *vec;
			long index;
			iterator(Vector *vec) : vec(vec) { reset(); }

		public:
			void reset() { index = vec && vec->sz? ITER_INDEX_START : ITER_INDEX_EMPTY; }
			value_t first(type_t type=TYPE_EMPTY);
			value_t next(type_t type=TYPE_EMPTY);
			value_t previous(type_t type=TYPE_EMPTY);
			value_t last(type_t type=TYPE_EMPTY);
			value_t remove();

			value_t value() { return index>=0? vec->list[index].value : 0; }
			value_t type() { return index>=0? vec->list[index].type : TYPE_EMPTY; }

			iterator &operator++() { next(TYPE_EMPTY);return *this; }
			iterator &operator--() { previous(TYPE_EMPTY);return *this; }
			bool operator==(const iterator &iter) { return vec==iter.vec && index==iter.index; }
			bool operator!=(const iterator &iter) { return vec!=iter.vec || index!=iter.index; }
			operator void *() const { return index>=0? (void *)vec->list[index].value : 0; }
	};

	Vector();
	Vector(size_t s);
	~Vector();

	iterator iterate();

	Vector &operator+=(void *v) { return insert((value_t)v,sz,TYPE_VOID_P); }
	Vector &operator+=(long v) { return insert((value_t)v,sz,TYPE_INTPTR); }
	Vector &operator+=(unsigned long v) { return insert((value_t)v,sz,TYPE_INTPTR); }
#if _WORDSIZE == 64
	Vector &operator+=(float v) { double d = v;return insert(*(value_t *)((void *)&d),sz,TYPE_DOUBLE); }
	Vector &operator+=(double v) { return insert(*(value_t *)((void *)&v),sz,TYPE_DOUBLE); }
#else
	Vector &operator+=(float v) { return insert(*(value_t *)((void *)&v),sz,TYPE_FLOAT); }
#endif
	Vector &operator+=(const char *v) { return insert((value_t)v,sz,TYPE_CHAR_P); }
	Vector &operator+=(String *v) { return insert((value_t)v->toCharArray(),sz,TYPE_CHAR_P); }
	Vector &operator+=(String &v) { return insert((value_t)v.toCharArray(),sz,TYPE_CHAR_P); }
	Vector &operator+=(Object *v) { return insert((value_t)v,sz,TYPE_OBJECT_P); }
	Vector &operator+=(Object &v) { return insert((value_t)&v,sz,TYPE_OBJECT_P); }
	Vector &operator+=(Vector &a) { return insert(a,sz); }
	Vector &insert(void *v,long n=-1) { return insert((value_t)v,n,TYPE_VOID_P); }
	Vector &insert(long v,long n=-1) { return insert((value_t)v,n,TYPE_INTPTR); }
	Vector &insert(unsigned long v,long n=-1) { return insert((value_t)v,n,TYPE_INTPTR); }
#if _WORDSIZE == 64
	Vector &insert(float v,long n=-1) { double d = v;return insert(*(value_t *)((void *)&d),n,TYPE_DOUBLE); }
	Vector &insert(double v,long n=-1) { return insert(*(value_t *)((void *)&v),n,TYPE_DOUBLE); }
#else
	Vector &insert(float v,long n=-1) { return insert(*(value_t *)((void *)&v),n,TYPE_FLOAT); }
#endif
	Vector &insert(const char *v,long n=-1) { return insert((value_t)v,n,TYPE_CHAR_P); }
	Vector &insert(String *v,long n=-1) { return insert((value_t)v->toCharArray(),n,TYPE_CHAR_P); }
	Vector &insert(String &v,long n=-1) { return insert((value_t)v.toCharArray(),n,TYPE_CHAR_P); }
	Vector &insert(Object *v,long n=-1) { return insert((value_t)v,n,TYPE_OBJECT_P); }
	Vector &insert(Object &v,long n=-1) { return insert((value_t)&v,n,TYPE_OBJECT_P); }
	Vector &insert(Vector &v,long n=-1);

	Vector &operator-=(void *v) { return remove((value_t)v,TYPE_VOID_P); }
	Vector &operator-=(long v) { return remove((value_t)v,TYPE_INTPTR); }
	Vector &operator-=(unsigned long v) { return remove((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	Vector &operator-=(float v) { double d = v;return remove(*(value_t *)((void *)&d),TYPE_DOUBLE); }
	Vector &operator-=(double v) { return remove(*(value_t *)((void *)&v),TYPE_DOUBLE); }
#else
	Vector &operator-=(float v) { return remove(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	Vector &operator-=(const char *v) { return remove(v); }
	Vector &operator-=(String *v) { return remove(v->toCharArray()); }
	Vector &operator-=(String &v) { return remove(v.toCharArray()); }
	Vector &operator-=(Object *v) { return remove((value_t)v,TYPE_OBJECT_P); }
	Vector &operator-=(Object &v) { return remove((value_t)&v,TYPE_OBJECT_P); }
	Vector &remove(void *v) { return remove((value_t)v,TYPE_VOID_P); }
	Vector &remove(long v) { return remove((value_t)v,TYPE_INTPTR); }
	Vector &remove(unsigned long v) { return remove((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	Vector &remove(float v) { double d = v;return remove(*(value_t *)((void *)&d),TYPE_DOUBLE); }
	Vector &remove(double v) { return remove(*(value_t *)((void *)&v),TYPE_DOUBLE); }
#else
	Vector &remove(float v) { return remove(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	Vector &remove(const char *v);
	Vector &remove(String *v) { return remove(v->toCharArray()); }
	Vector &remove(String &v) { return remove(v.toCharArray()); }
	Vector &remove(Object *v) { return remove((value_t)v,TYPE_OBJECT_P); }
	Vector &remove(Object &v) { return remove((value_t)&v,TYPE_OBJECT_P); }
	Vector &removeAt(size_t n);
	Vector &removeAll() { clear();return *this; }

	long indexOf(void *v) { return indexOf((value_t)v,TYPE_VOID_P); }
	long indexOf(long v) { return indexOf((value_t)v,TYPE_INTPTR); }
	long indexOf(unsigned long v) { return indexOf((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	long indexOf(float v) { double d = v;return indexOf(*(value_t *)((void *)&d),TYPE_DOUBLE); }
	long indexOf(double v) { return indexOf(*(value_t *)((void *)&v),TYPE_DOUBLE); }
#else
	long indexOf(float v) { return indexOf(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	long indexOf(const char *v);
	long indexOf(String *v) { return indexOf(v->toCharArray()); }
	long indexOf(String &v) { return indexOf(v.toCharArray()); }
	long indexOf(Object *v) { return indexOf((value_t)v,TYPE_OBJECT_P); }
	long indexOf(Object &v) { return indexOf((value_t)&v,TYPE_OBJECT_P); }

	bool contains(void *v) { return indexOf((value_t)v,TYPE_VOID_P)!=-1; }
	bool contains(long v) { return indexOf((value_t)v,TYPE_INTPTR)!=-1; }
	bool contains(unsigned long v) { return indexOf((value_t)v,TYPE_INTPTR)!=-1; }
#if _WORDSIZE == 64
	bool contains(float v) { double d = v;return indexOf(*(value_t *)((void *)&d),TYPE_DOUBLE)!=-1; }
	bool contains(double v) { return indexOf(*(value_t *)((void *)&v),TYPE_DOUBLE)!=-1; }
#else
	bool contains(float v) { return indexOf(*(value_t *)((void *)&v),TYPE_FLOAT)!=-1; }
#endif
	bool contains(const char *v) { return indexOf(v)!=-1; }
	bool contains(String *v) { return indexOf(v->toCharArray())!=-1; }
	bool contains(String &v) { return indexOf(v.toCharArray())!=-1; }
	bool contains(Object *v) { return indexOf((value_t)v,TYPE_OBJECT_P)!=-1; }
	bool contains(Object &v) { return indexOf((value_t)&v,TYPE_OBJECT_P)!=-1; }

	Vector &split(String &str,const char *delim,bool trim=false) { return split(str.toCharArray(),delim,trim); }
	Vector &split(const char *str,const char *delim,bool trim=false);

	void resize(size_t s=0);
	void trim();
	void clear();

	value_t operator[](char n) { return sz && n>=0 && (size_t)n<sz? list[(int)n].value : 0; }
	value_t operator[](unsigned char n) { return sz && (size_t)n<sz? list[n].value : 0; }
	value_t operator[](short n) { return sz && n>=0 && (size_t)n<sz? list[n].value : 0; }
	value_t operator[](unsigned short n) { return sz && (size_t)n<sz? list[n].value : 0; }
	value_t operator[](int n) { return sz && n>=0 && (size_t)n<sz? list[n].value : 0; }
	value_t operator[](unsigned int n) { return sz && (size_t)n<sz? list[n].value : 0; }
	value_t operator[](long n) { return sz && n>=0 && (size_t)n<sz? list[n].value : 0; }
	value_t operator[](unsigned long n) { return sz && (size_t)n<sz? list[n].value : 0; }
	operator int() const { return (int)sz; }
	operator long() const { return (long)sz; }
	operator unsigned long() const { return (unsigned long)sz; }

	size_t print(const char *fn);
	size_t print(FILE *fp);
	size_t load(const char *fn);
	size_t load(FILE *fp);
	size_t save(const char *fn);
	size_t save(FILE *fp);
};

#endif /* _LIBAMANITA_VECTOR_H */




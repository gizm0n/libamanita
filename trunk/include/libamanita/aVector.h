#ifndef _LIBAMANITA_AVECTOR_H
#define _LIBAMANITA_AVECTOR_H

#include <stdint.h>
#include <libamanita/aCollection.h>
#include <libamanita/aObject.h>
#include <libamanita/aString.h>

class aHashtable;

class aVector : public aCollection {
RttiObjectInstance(aVector)

friend class aHashtable;

private:
	struct node {
		value_t value;
		type_t type;
	};
	size_t inc;
	node *list;

	aVector &insert(value_t v,long n,type_t t);
	aVector &remove(value_t v,type_t t);
	long indexOf(value_t v,type_t t);

public:
	class iterator {
		friend class aVector;

		private:
			aVector *vec;
			long index;
			iterator(aVector *vec) : vec(vec) { reset(); }

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

	aVector();
	aVector(size_t s);
	~aVector();

	iterator iterate();

	aVector &operator+=(void *v) { return insert((value_t)v,sz,TYPE_VOID_P); }
	aVector &operator+=(long v) { return insert((value_t)v,sz,TYPE_INTPTR); }
	aVector &operator+=(unsigned long v) { return insert((value_t)v,sz,TYPE_INTPTR); }
#if _WORDSIZE == 64
	aVector &operator+=(float v) { double d = v;return insert(*(value_t *)((void *)&d),sz,TYPE_DOUBLE); }
	aVector &operator+=(double v) { return insert(*(value_t *)((void *)&v),sz,TYPE_DOUBLE); }
#else
	aVector &operator+=(float v) { return insert(*(value_t *)((void *)&v),sz,TYPE_FLOAT); }
#endif
	aVector &operator+=(const char *v) { return insert((value_t)v,sz,TYPE_CHAR_P); }
	aVector &operator+=(aString *v) { return insert((value_t)v->toCharArray(),sz,TYPE_CHAR_P); }
	aVector &operator+=(aString &v) { return insert((value_t)v.toCharArray(),sz,TYPE_CHAR_P); }
	aVector &operator+=(aObject *v) { return insert((value_t)v,sz,TYPE_OBJECT_P); }
	aVector &operator+=(aObject &v) { return insert((value_t)&v,sz,TYPE_OBJECT_P); }
	aVector &operator+=(aVector &a) { return insert(a,sz); }
	aVector &insert(void *v,long n=-1) { return insert((value_t)v,n,TYPE_VOID_P); }
	aVector &insert(long v,long n=-1) { return insert((value_t)v,n,TYPE_INTPTR); }
	aVector &insert(unsigned long v,long n=-1) { return insert((value_t)v,n,TYPE_INTPTR); }
#if _WORDSIZE == 64
	aVector &insert(float v,long n=-1) { double d = v;return insert(*(value_t *)((void *)&d),n,TYPE_DOUBLE); }
	aVector &insert(double v,long n=-1) { return insert(*(value_t *)((void *)&v),n,TYPE_DOUBLE); }
#else
	aVector &insert(float v,long n=-1) { return insert(*(value_t *)((void *)&v),n,TYPE_FLOAT); }
#endif
	aVector &insert(const char *v,long n=-1) { return insert((value_t)v,n,TYPE_CHAR_P); }
	aVector &insert(aString *v,long n=-1) { return insert((value_t)v->toCharArray(),n,TYPE_CHAR_P); }
	aVector &insert(aString &v,long n=-1) { return insert((value_t)v.toCharArray(),n,TYPE_CHAR_P); }
	aVector &insert(aObject *v,long n=-1) { return insert((value_t)v,n,TYPE_OBJECT_P); }
	aVector &insert(aObject &v,long n=-1) { return insert((value_t)&v,n,TYPE_OBJECT_P); }
	aVector &insert(aVector &v,long n=-1);

	aVector &operator-=(void *v) { return remove((value_t)v,TYPE_VOID_P); }
	aVector &operator-=(long v) { return remove((value_t)v,TYPE_INTPTR); }
	aVector &operator-=(unsigned long v) { return remove((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	aVector &operator-=(float v) { double d = v;return remove(*(value_t *)((void *)&d),TYPE_DOUBLE); }
	aVector &operator-=(double v) { return remove(*(value_t *)((void *)&v),TYPE_DOUBLE); }
#else
	aVector &operator-=(float v) { return remove(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	aVector &operator-=(const char *v) { return remove(v); }
	aVector &operator-=(aString *v) { return remove(v->toCharArray()); }
	aVector &operator-=(aString &v) { return remove(v.toCharArray()); }
	aVector &operator-=(aObject *v) { return remove((value_t)v,TYPE_OBJECT_P); }
	aVector &operator-=(aObject &v) { return remove((value_t)&v,TYPE_OBJECT_P); }
	aVector &remove(void *v) { return remove((value_t)v,TYPE_VOID_P); }
	aVector &remove(long v) { return remove((value_t)v,TYPE_INTPTR); }
	aVector &remove(unsigned long v) { return remove((value_t)v,TYPE_INTPTR); }
#if _WORDSIZE == 64
	aVector &remove(float v) { double d = v;return remove(*(value_t *)((void *)&d),TYPE_DOUBLE); }
	aVector &remove(double v) { return remove(*(value_t *)((void *)&v),TYPE_DOUBLE); }
#else
	aVector &remove(float v) { return remove(*(value_t *)((void *)&v),TYPE_FLOAT); }
#endif
	aVector &remove(const char *v);
	aVector &remove(aString *v) { return remove(v->toCharArray()); }
	aVector &remove(aString &v) { return remove(v.toCharArray()); }
	aVector &remove(aObject *v) { return remove((value_t)v,TYPE_OBJECT_P); }
	aVector &remove(aObject &v) { return remove((value_t)&v,TYPE_OBJECT_P); }
	aVector &removeAt(size_t n);
	aVector &removeAll() { clear();return *this; }

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
	long indexOf(aString *v) { return indexOf(v->toCharArray()); }
	long indexOf(aString &v) { return indexOf(v.toCharArray()); }
	long indexOf(aObject *v) { return indexOf((value_t)v,TYPE_OBJECT_P); }
	long indexOf(aObject &v) { return indexOf((value_t)&v,TYPE_OBJECT_P); }

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
	bool contains(aString *v) { return indexOf(v->toCharArray())!=-1; }
	bool contains(aString &v) { return indexOf(v.toCharArray())!=-1; }
	bool contains(aObject *v) { return indexOf((value_t)v,TYPE_OBJECT_P)!=-1; }
	bool contains(aObject &v) { return indexOf((value_t)&v,TYPE_OBJECT_P)!=-1; }

	aVector &split(aString &str,const char *delim,bool trim=false) { return split(str.toCharArray(),delim,trim); }
	aVector &split(const char *str,const char *delim,bool trim=false);

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

#endif /* _LIBAMANITA_AVECTOR_H */




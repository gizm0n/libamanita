#ifndef _LIBAMANITA_HASHTABLE_H
#define _LIBAMANITA_HASHTABLE_H

#include <string.h>
#include <stdlib.h>
#include <libamanita/Collection.h>
#include <libamanita/Object.h>
#include <libamanita/String.h>


enum {
	HASH_STYLE_CASE_SENSITIVE				= 0,
	HASH_STYLE_CASE_INSENSITIVE			= 1,
	HASH_STYLE_KEY_MULTIPLES				= 2,
};

#define _HT_SET_INT_VALUE(v,t) {if(at) at->value = (value_t)v,at->v_type = t;}
#define _HT_SET_FLOAT_VALUE(v,t) {if(at) at->value = *(value_t *)((void *)&v),at->v_type = t;}
#define _HT_SET_STRING_VALUE(v,t) {if(at) at->value = (value_t)strdup(v),at->v_type = t;}

#define _HT_PUT_KEY_INT_VALUE_LIST(key_type1,key_value_t,key_type2) \
	void put(key_type1 key,void *value) { put(key_value_t,(value_t)value,key_type2,TYPE_VOID_P); } \
	void put(key_type1 key,char value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned char value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,short value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned short value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,int value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned int value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,long value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned long value) { put(key_value_t,(value_t)value,key_type2,TYPE_INTPTR); } \
	void put(key_type1 key,float value) { put(key_value_t,*(value_t *)((void *)&value),key_type2,TYPE_FLOAT); } \
	void put(key_type1 key,const char *value) { put(key_value_t,(value_t)strdup(value),key_type2,TYPE_CHAR_P); } \
	void put(key_type1 key,String *value) { put(key_value_t,(value_t)strdup(value->toString()),key_type2,TYPE_CHAR_P); } \
	void put(key_type1 key,String &value) { put(key_value_t,(value_t)strdup(value.toString()),key_type2,TYPE_CHAR_P); } \
	void put(key_type1 key,const Object *value) { put(key_value_t,(value_t)value,key_type2,TYPE_OBJECT_P); } \
	void put(key_type1 key,const Object &value) { put(key_value_t,(value_t)&value,key_type2,TYPE_OBJECT_P); }

#define _HT_PUT_KEY_STRING_VALUE_LIST(key_type1,key_value_t) \
	void put(key_type1 key,void *value) { put(key_value_t,(value_t)value,TYPE_VOID_P); } \
	void put(key_type1 key,char value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned char value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,short value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned short value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,int value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned int value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,long value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,unsigned long value) { put(key_value_t,(value_t)value,TYPE_INTPTR); } \
	void put(key_type1 key,float value) { put(key_value_t,*(value_t *)((void *)&value),TYPE_FLOAT); } \
	void put(key_type1 key,const char *value) { put(key_value_t,(value_t)strdup(value),TYPE_CHAR_P); } \
	void put(key_type1 key,String *value) { put(key_value_t,(value_t)strdup(value->toString()),TYPE_CHAR_P); } \
	void put(key_type1 key,String &value) { put(key_value_t,(value_t)strdup(value.toString()),TYPE_CHAR_P); } \
	void put(key_type1 key,const Object *value) { put(key_value_t,(value_t)value,TYPE_OBJECT_P); } \
	void put(key_type1 key,const Object &value) { put(key_value_t,(value_t)&value,TYPE_OBJECT_P); }


class Vector;

class Hashtable : public Collection {
RttiObjectInstance(Hashtable)

protected:
	struct node {
		value_t key;
		hash_t hash;
		value_t value;
		type_t k_type,v_type;
		node *next;
		node(value_t k,hash_t h,value_t v,type_t kt,type_t vt,node *n)
			: key(k),hash(h),value(v),k_type(kt),v_type(vt),next(n) {}
		~node() {
			if(k_type==TYPE_CHAR_P) free((char *)key);
			else if(k_type==TYPE_DOUBLE) free((double *)key);
			if(v_type==TYPE_CHAR_P) free((char *)value);
			else if(v_type==TYPE_DOUBLE) free((double *)value);
		}
	};
	node **table;
	size_t full;
	style_t style;
	float ld;

	void put(value_t key,value_t value,type_t kt,type_t vt);
	void put(const char *key,value_t value,type_t vt);
	value_t get(value_t key,type_t kt);
	value_t get(value_t key,type_t kt,type_t &type);
	size_t get(value_t key,type_t kt,Vector &v);
	value_t remove(value_t key,type_t kt);
	value_t remove(value_t key,type_t kt,type_t &type);
	void rehash(style_t st);

public:
	class iterator {
		friend class Hashtable;
		private:
			Hashtable *ht;
			node *at;
			long index;
			iterator(Hashtable *ht) : ht(ht) { reset(); }
		public:
			iterator() : ht(0),at(0),index(ITER_INDEX_EMPTY) {}
			void reset() { at = 0,index = ht && ht->sz? ITER_INDEX_START : ITER_INDEX_EMPTY; }
			value_t first() { return first(TYPE_EMPTY,TYPE_EMPTY); }
			value_t first(type_t kt,type_t vt=TYPE_EMPTY);
			value_t next();
			value_t next(type_t kt,type_t vt=TYPE_EMPTY);
			value_t previous() { return previous(TYPE_EMPTY,TYPE_EMPTY); }
			value_t previous(type_t kt,type_t vt=TYPE_EMPTY);
			value_t last() { return last(TYPE_EMPTY,TYPE_EMPTY); }
			value_t last(type_t kt,type_t vt=TYPE_EMPTY);
			value_t remove();

			value_t key() { return at? at->key : 0; }
			type_t keyType() { return at? at->k_type : TYPE_EMPTY; }
			value_t value() { return at? at->value : 0; }
			type_t valueType() { return at? at->v_type : TYPE_EMPTY; }
			void setValue(void *v) { _HT_SET_INT_VALUE(v,TYPE_VOID_P); }
			void setValue(char v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(unsigned char v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(short v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(unsigned short v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(int v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(unsigned int v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(long v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
			void setValue(unsigned long v) { _HT_SET_INT_VALUE(v,TYPE_INTPTR); }
#if _WORDSIZE == 64
			void setValue(float v) { double d = (double)v;_HT_SET_FLOAT_VALUE(d,TYPE_DOUBLE); }
			void setValue(double v) { _HT_SET_FLOAT_VALUE(v,TYPE_DOUBLE); }
#else
			//void setValue(long long v);
			//void setValue(unsigned long long v);
			void setValue(float v) { _HT_SET_FLOAT_VALUE(v,TYPE_FLOAT); }
			//void setValue(double v);
#endif
			//void setValue(long double v);
			void setValue(const char *v) { _HT_SET_STRING_VALUE(v,TYPE_CHAR_P); }
			void setValue(String *v) { _HT_SET_STRING_VALUE(v->toString(),TYPE_CHAR_P); }
			void setValue(String &v) { _HT_SET_STRING_VALUE(v.toString(),TYPE_CHAR_P); }
			void setValue(const Object *v) { _HT_SET_INT_VALUE(v,TYPE_OBJECT_P); }
			void setValue(const Object &v) { _HT_SET_INT_VALUE(&v,TYPE_OBJECT_P); }

			iterator &operator++() { next(0);return *this; }
			iterator &operator--() { previous(0);return *this; }
			bool operator==(const iterator &iter) { return at==iter.at; }
			bool operator!=(const iterator &iter) { return at!=iter.at; }
			//operator void *() const { return at? (void *)at->value : 0; }
			//operator long() const { return at && at->v_type==TYPE_INTPTR? (long)at->value : 0; }
			//operator unsigned long() const { return at && at->v_type==TYPE_INTPTR? (unsigned long)at->value : 0; }
			//operator char *() const { return at && at->v_type==TYPE_CHAR_P? (char *)at->value : 0; }
	};
	friend class iterator;

	Hashtable(size_t c=11,float l=0.5f,style_t st=0);
	~Hashtable() { removeAll(); }

	iterator iterate();

	void setCaseInsensitiveKeys(bool b);
	void setAllowKeyMultiples(bool b);

	hash_t hash(const char *key);

_HT_PUT_KEY_INT_VALUE_LIST(void *,(value_t)key,TYPE_VOID_P)
_HT_PUT_KEY_INT_VALUE_LIST(char,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(unsigned char,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(short,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(unsigned short,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(int,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(unsigned int,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(long,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(unsigned long,(value_t)key,TYPE_INTPTR)
_HT_PUT_KEY_INT_VALUE_LIST(float,*(value_t *)((void *)&key),TYPE_FLOAT)
_HT_PUT_KEY_STRING_VALUE_LIST(const char *,key)
_HT_PUT_KEY_STRING_VALUE_LIST(String *,key->toString())
_HT_PUT_KEY_STRING_VALUE_LIST(String &,key.toString())
_HT_PUT_KEY_INT_VALUE_LIST(Object *,(value_t)key->hash(),TYPE_OBJECT_P)
_HT_PUT_KEY_INT_VALUE_LIST(Object &,(value_t)key.hash(),TYPE_OBJECT_P)

	value_t get(void *key) { return get((value_t)key,TYPE_VOID_P); }
	value_t get(char key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(unsigned char key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(short key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(unsigned short key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(int key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(unsigned int key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(long key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(unsigned long key) { return get((value_t)key,TYPE_INTPTR); }
	value_t get(float key) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT); }
	value_t get(const char *key);
	value_t get(Object *key) { return get((value_t)key->hash(),TYPE_OBJECT_P); }
	value_t get(Object &key) { return get((value_t)key.hash(),TYPE_OBJECT_P); }

	value_t get(void *key,type_t &type) { return get((value_t)key,TYPE_VOID_P,type); }
	value_t get(char key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(unsigned char key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(short key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(unsigned short key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(int key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(unsigned int key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(long key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(unsigned long key,type_t &type) { return get((value_t)key,TYPE_INTPTR,type); }
	value_t get(float key,type_t &type) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT,type); }
	value_t get(const char *key,type_t &type);
	value_t get(Object *key,type_t &type) { return get((value_t)key->hash(),TYPE_OBJECT_P,type); }
	value_t get(Object &key,type_t &type) { return get((value_t)key.hash(),TYPE_OBJECT_P,type); }

	char *getString(void *key) { return (char *)get((value_t)key,TYPE_VOID_P); }
	char *getString(char key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(unsigned char key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(short key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(unsigned short key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(int key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(unsigned int key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(long key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(unsigned long key) { return (char *)get((value_t)key,TYPE_INTPTR); }
	char *getString(float key) { return (char *)get(*((value_t *)((void *)&key)),TYPE_FLOAT); }
	char *getString(const char *key) { return (char *)get(key); }
	char *getString(Object *key) { return (char *)get((value_t)key->hash(),TYPE_OBJECT_P); }
	char *getString(Object &key) { return (char *)get((value_t)key.hash(),TYPE_OBJECT_P); }

	size_t get(void *key,Vector &v) { return get((value_t)key,TYPE_VOID_P,v); }
	size_t get(char key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(unsigned char key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(short key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(unsigned short key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(int key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(unsigned int key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(long key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(unsigned long key,Vector &v) { return get((value_t)key,TYPE_INTPTR,v); }
	size_t get(float key,Vector &v) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT,v); }
	size_t get(const char *key,Vector &v);
	size_t get(Object *key,Vector &v) { return get((value_t)key->hash(),TYPE_OBJECT_P,v); }
	size_t get(Object &key,Vector &v) { return get((value_t)key.hash(),TYPE_OBJECT_P,v); }

	value_t getByIndex(size_t index,type_t &type);

	bool contains(void *key) { return get((value_t)key,TYPE_VOID_P)!=0; }
	bool contains(char key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(unsigned char key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(short key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(unsigned short key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(int key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(unsigned int key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(long key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(unsigned long key) { return get((value_t)key,TYPE_INTPTR)!=0; }
	bool contains(float key) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT)!=0; }
	bool contains(const char *key) { return get(key)!=0; }
	bool contains(Object *key) { return get((value_t)key->hash(),TYPE_OBJECT_P)!=0; }
	bool contains(Object &key) { return get((value_t)key.hash(),TYPE_OBJECT_P)!=0; }

	value_t remove(void *key) { return remove((value_t)key,TYPE_VOID_P); }
	value_t remove(char key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(unsigned char key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(short key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(unsigned short key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(int key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(unsigned int key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(long key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(unsigned long key) { return remove((value_t)key,TYPE_INTPTR); }
	value_t remove(float key) { return remove(*((value_t *)((void *)&key)),TYPE_FLOAT); }
	value_t remove(const char *key);
	value_t remove(Object *key) { return remove((value_t)key->hash(),TYPE_OBJECT_P); }
	value_t remove(Object &key) { return remove((value_t)key.hash(),TYPE_OBJECT_P); }

	value_t remove(void *key,type_t &type) { return remove((value_t)key,TYPE_VOID_P,type); }
	value_t remove(char key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(unsigned char key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(short key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(unsigned short key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(int key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(unsigned int key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(long key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(unsigned long key,type_t &type) { return remove((value_t)key,TYPE_INTPTR,type); }
	value_t remove(float key,type_t &type) { return remove(*((value_t *)((void *)&key)),TYPE_FLOAT,type); }
	value_t remove(const char *key,type_t &type);
	value_t remove(Object *key,type_t &type) { return remove((value_t)key->hash(),TYPE_OBJECT_P,type); }
	value_t remove(Object &key,type_t &type) { return remove((value_t)key.hash(),TYPE_OBJECT_P,type); }

	void removeAll();

	size_t print(const char *fn);
	size_t print(FILE *fp);
	size_t load(const char *fn);
	size_t load(FILE *fp);
	size_t save(const char *fn);
	size_t save(FILE *fp);
	static void readPair(FILE *file,String &key,String &value);
};


#endif /* _LIBAMANITA_HASHTABLE_H */


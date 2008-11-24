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

			value_t key() { return at? at->key : 0ul; }
			type_t keyType() { return at? at->k_type : TYPE_EMPTY; }
			value_t value() { return at? at->value : 0ul; }
			type_t valueType() { return at? at->v_type : TYPE_EMPTY; }
			void setValue(void *v) { if(at) at->value = (value_t)v,at->v_type = TYPE_VOID_P; }
			void setValue(long v) { if(at) at->value = (value_t)v,at->v_type = TYPE_INT32; }
			void setValue(unsigned long v) { if(at) at->value = (value_t)v,at->v_type = TYPE_UINT32; }
			void setValue(float v) { if(at) at->value = *(value_t *)((void *)&v),at->v_type = TYPE_FLOAT; }
			void setValue(const char *v) { if(at) at->value = (value_t)strdup(v),at->v_type = TYPE_CHAR_P; }
			void setValue(String *v) { if(at) at->value = (value_t)strdup(v->toString()),at->v_type = TYPE_CHAR_P; }
			void setValue(String &v) { if(at) at->value = (value_t)strdup(v.toString()),at->v_type = TYPE_CHAR_P; }
			void setValue(const Object *v) { if(at) at->value = (value_t)v,at->v_type = TYPE_OBJECT_P; }
			void setValue(const Object &v) { if(at) at->value = (value_t)&v,at->v_type = TYPE_OBJECT_P; }

			iterator &operator++() { next(0);return *this; }
			iterator &operator--() { previous(0);return *this; }
			bool operator==(const iterator &iter) { return at==iter.at; }
			bool operator!=(const iterator &iter) { return at!=iter.at; }
			operator void *() const { return at? (void *)at->value : 0; }
			operator long() const { return at && at->v_type==TYPE_INT32? (long)at->value : 0l; }
			operator unsigned long() const { return at && at->v_type==TYPE_UINT32? (unsigned long)at->value : 0ul; }
			operator char *() const { return at && at->v_type==TYPE_CHAR_P? (char *)at->value : 0; }
	};
	friend class iterator;

	Hashtable(size_t c=11ul,float l=0.5f,style_t st=0ul);
	~Hashtable() { removeAll(); }

	iterator iterate();

	void setCaseInsensitiveKeys(bool b);
	void setAllowKeyMultiples(bool b);

	hash_t hash(const char *key);

	void put(void *key,void *value) { put((value_t)key,(value_t)value,TYPE_VOID_P,TYPE_VOID_P); }
	void put(void *key,long value) { put((value_t)key,(value_t)value,TYPE_VOID_P,TYPE_INT32); }
	void put(void *key,unsigned long value) { put((value_t)key,(value_t)value,TYPE_VOID_P,TYPE_UINT32); }
	void put(void *key,float value) { put((value_t)key,*(value_t *)((void *)&value),TYPE_VOID_P,TYPE_FLOAT); }
	void put(void *key,const char *value) { put((value_t)key,(value_t)strdup(value),TYPE_VOID_P,TYPE_CHAR_P); }
	void put(void *key,String *value) { put((value_t)key,(value_t)strdup(value->toString()),TYPE_VOID_P,TYPE_CHAR_P); }
	void put(void *key,String &value) { put((value_t)key,(value_t)strdup(value.toString()),TYPE_VOID_P,TYPE_CHAR_P); }
	void put(void *key,const Object *value) { put((value_t)key,(value_t)value,TYPE_VOID_P,TYPE_OBJECT_P); }
	void put(void *key,const Object &value) { put((value_t)key,(value_t)&value,TYPE_VOID_P,TYPE_OBJECT_P); }

	void put(long key,void *value) { put((value_t)key,(value_t)value,TYPE_INT32,TYPE_VOID_P); }
	void put(long key,long value) { put((value_t)key,(value_t)value,TYPE_INT32,TYPE_INT32); }
	void put(long key,unsigned long value) { put((value_t)key,(value_t)value,TYPE_INT32,TYPE_UINT32); }
	void put(long key,float value) { put((value_t)key,*(value_t *)((void *)&value),TYPE_INT32,TYPE_FLOAT); }
	void put(long key,const char *value) { put((value_t)key,(value_t)strdup(value),TYPE_INT32,TYPE_CHAR_P); }
	void put(long key,String *value) { put((value_t)key,(value_t)strdup(value->toString()),TYPE_INT32,TYPE_CHAR_P); }
	void put(long key,String &value) { put((value_t)key,(value_t)strdup(value.toString()),TYPE_INT32,TYPE_CHAR_P); }
	void put(long key,const Object *value) { put((value_t)key,(value_t)value,TYPE_INT32,TYPE_OBJECT_P); }
	void put(long key,const Object &value) { put((value_t)key,(value_t)&value,TYPE_INT32,TYPE_OBJECT_P); }

	void put(unsigned long key,void *value) { put((value_t)key,(value_t)value,TYPE_UINT32,TYPE_VOID_P); }
	void put(unsigned long key,long value) { put((value_t)key,(value_t)value,TYPE_UINT32,TYPE_INT32); }
	void put(unsigned long key,unsigned long value) { put((value_t)key,(value_t)value,TYPE_UINT32,TYPE_UINT32); }
	void put(unsigned long key,float value) { put((value_t)key,*(value_t *)((void *)&value),TYPE_UINT32,TYPE_FLOAT); }
	void put(unsigned long key,const char *value) { put((value_t)key,(value_t)strdup(value),TYPE_UINT32,TYPE_CHAR_P); }
	void put(unsigned long key,String *value) { put((value_t)key,(value_t)strdup(value->toString()),TYPE_UINT32,TYPE_CHAR_P); }
	void put(unsigned long key,String &value) { put((value_t)key,(value_t)strdup(value.toString()),TYPE_UINT32,TYPE_CHAR_P); }
	void put(unsigned long key,const Object *value) { put((value_t)key,(value_t)value,TYPE_UINT32,TYPE_OBJECT_P); }
	void put(unsigned long key,const Object &value) { put((value_t)key,(value_t)&value,TYPE_UINT32,TYPE_OBJECT_P); }

	void put(float key,void *value) { put(*(value_t *)((void *)&key),(value_t)value,TYPE_FLOAT,TYPE_VOID_P); }
	void put(float key,long value) { put(*(value_t *)((void *)&key),(value_t)value,TYPE_FLOAT,TYPE_INT32); }
	void put(float key,unsigned long value) { put(*(value_t *)((void *)&key),(value_t)value,TYPE_FLOAT,TYPE_UINT32); }
	void put(float key,float value) { put(*(value_t *)((void *)&key),*(value_t *)((void *)&value),TYPE_FLOAT,TYPE_FLOAT); }
	void put(float key,const char *value) { put(*(value_t *)((void *)&key),(value_t)strdup(value),TYPE_FLOAT,TYPE_CHAR_P); }
	void put(float key,String *value) { put(*(value_t *)((void *)&key),(value_t)strdup(value->toString()),TYPE_FLOAT,TYPE_CHAR_P); }
	void put(float key,String &value) { put(*(value_t *)((void *)&key),(value_t)strdup(value.toString()),TYPE_FLOAT,TYPE_CHAR_P); }
	void put(float key,const Object *value) { put(*(value_t *)((void *)&key),(value_t)value,TYPE_FLOAT,TYPE_OBJECT_P); }
	void put(float key,const Object &value) { put(*(value_t *)((void *)&key),(value_t)&value,TYPE_FLOAT,TYPE_OBJECT_P); }

	void put(const char *key,void *value) { put(key,(value_t)value,TYPE_VOID_P); }
	void put(const char *key,long value) { put(key,(value_t)value,TYPE_INT32); }
	void put(const char *key,unsigned long value) { put(key,(value_t)value,TYPE_UINT32); }
	void put(const char *key,float value) { put(key,*(value_t *)((void *)&value),TYPE_FLOAT); }
	void put(const char *key,const char *value) { put(key,(value_t)strdup(value),TYPE_CHAR_P); }
	void put(const char *key,String *value) { put(key,(value_t)strdup(value->toString()),TYPE_CHAR_P); }
	void put(const char *key,String &value) { put(key,(value_t)strdup(value.toString()),TYPE_CHAR_P); }
	void put(const char *key,const Object *value) { put(key,(value_t)value,TYPE_OBJECT_P); }
	void put(const char *key,const Object &value) { put(key,(value_t)&value,TYPE_OBJECT_P); }

	void put(String *key,void *value) { put(key->toString(),(value_t)value,TYPE_VOID_P); }
	void put(String *key,long value) { put(key->toString(),(value_t)value,TYPE_INT32); }
	void put(String *key,unsigned long value) { put(key->toString(),(value_t)value,TYPE_UINT32); }
	void put(String *key,float value) { put(key->toString(),*(value_t *)((void *)&value),TYPE_FLOAT); }
	void put(String *key,const char *value) { put(key->toString(),(value_t)strdup(value),TYPE_CHAR_P); }
	void put(String *key,String *value) { put(key->toString(),(value_t)strdup(value->toString()),TYPE_CHAR_P); }
	void put(String *key,String &value) { put(key->toString(),(value_t)strdup(value.toString()),TYPE_CHAR_P); }
	void put(String *key,const Object *value) { put(key->toString(),(value_t)value,TYPE_OBJECT_P); }
	void put(String *key,const Object &value) { put(key->toString(),(value_t)&value,TYPE_OBJECT_P); }

	void put(String &key,void *value) { put(key.toString(),(value_t)value,TYPE_VOID_P); }
	void put(String &key,long value) { put(key.toString(),(value_t)value,TYPE_INT32); }
	void put(String &key,unsigned long value) { put(key.toString(),(value_t)value,TYPE_UINT32); }
	void put(String &key,float value) { put(key.toString(),*(value_t *)((void *)&value),TYPE_FLOAT); }
	void put(String &key,const char *value) { put(key.toString(),(value_t)strdup(value),TYPE_CHAR_P); }
	void put(String &key,String *value) { put(key.toString(),(value_t)strdup(value->toString()),TYPE_CHAR_P); }
	void put(String &key,String &value) { put(key.toString(),(value_t)strdup(value.toString()),TYPE_CHAR_P); }
	void put(String &key,const Object *value) { put(key.toString(),(value_t)value,TYPE_OBJECT_P); }
	void put(String &key,const Object &value) { put(key.toString(),(value_t)&value,TYPE_OBJECT_P); }

	void put(Object *key,void *value) { put((value_t)key->hash(),(value_t)value,TYPE_OBJECT_P,TYPE_VOID_P); }
	void put(Object *key,long value) { put((value_t)key->hash(),(value_t)value,TYPE_OBJECT_P,TYPE_INT32); }
	void put(Object *key,unsigned long value) { put((value_t)key->hash(),(value_t)value,TYPE_OBJECT_P,TYPE_UINT32); }
	void put(Object *key,float value) { put((value_t)key->hash(),*(value_t *)((void *)&value),TYPE_OBJECT_P,TYPE_FLOAT); }
	void put(Object *key,const char *value) { put((value_t)key->hash(),(value_t)strdup(value),TYPE_OBJECT_P,TYPE_CHAR_P); }
	void put(Object *key,String *value) { put((value_t)key->hash(),(value_t)strdup(value->toString()),TYPE_OBJECT_P,TYPE_CHAR_P); }
	void put(Object *key,String &value) { put((value_t)key->hash(),(value_t)strdup(value.toString()),TYPE_OBJECT_P,TYPE_CHAR_P); }
	void put(Object *key,const Object *value) { put((value_t)key->hash(),(value_t)value,TYPE_OBJECT_P,TYPE_OBJECT_P); }
	void put(Object *key,const Object &value) { put((value_t)key->hash(),(value_t)&value,TYPE_OBJECT_P,TYPE_OBJECT_P); }

	void put(Object &key,void *value) { put((value_t)key.hash(),(value_t)value,TYPE_OBJECT_P,TYPE_VOID_P); }
	void put(Object &key,long value) { put((value_t)key.hash(),(value_t)value,TYPE_OBJECT_P,TYPE_INT32); }
	void put(Object &key,unsigned long value) { put((value_t)key.hash(),(value_t)value,TYPE_OBJECT_P,TYPE_UINT32); }
	void put(Object &key,float value) { put((value_t)key.hash(),*(value_t *)((void *)&value),TYPE_OBJECT_P,TYPE_FLOAT); }
	void put(Object &key,const char *value) { put((value_t)key.hash(),(value_t)strdup(value),TYPE_OBJECT_P,TYPE_CHAR_P); }
	void put(Object &key,String *value) { put((value_t)key.hash(),(value_t)strdup(value->toString()),TYPE_OBJECT_P,TYPE_CHAR_P); }
	void put(Object &key,String &value) { put((value_t)key.hash(),(value_t)strdup(value.toString()),TYPE_OBJECT_P,TYPE_CHAR_P); }
	void put(Object &key,const Object *value) { put((value_t)key.hash(),(value_t)value,TYPE_OBJECT_P,TYPE_OBJECT_P); }
	void put(Object &key,const Object &value) { put((value_t)key.hash(),(value_t)&value,TYPE_OBJECT_P,TYPE_OBJECT_P); }

	value_t get(void *key) { return get((value_t)key,TYPE_VOID_P); }
	value_t get(long key) { return get((value_t)key,TYPE_INT32); }
	value_t get(unsigned long key) { return get((value_t)key,TYPE_UINT32); }
	value_t get(float key) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT); }
	value_t get(const char *key);
	value_t get(Object *key) { return get((value_t)key->hash(),TYPE_OBJECT_P); }
	value_t get(Object &key) { return get((value_t)key.hash(),TYPE_OBJECT_P); }

	value_t get(void *key,type_t &type) { return get((value_t)key,TYPE_VOID_P,type); }
	value_t get(long key,type_t &type) { return get((value_t)key,TYPE_INT32,type); }
	value_t get(unsigned long key,type_t &type) { return get((value_t)key,TYPE_UINT32,type); }
	value_t get(float key,type_t &type) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT,type); }
	value_t get(const char *key,type_t &type);
	value_t get(Object *key,type_t &type) { return get((value_t)key->hash(),TYPE_OBJECT_P,type); }
	value_t get(Object &key,type_t &type) { return get((value_t)key.hash(),TYPE_OBJECT_P,type); }

	char *getString(void *key) { return (char *)get((value_t)key,TYPE_VOID_P); }
	char *getString(long key) { return (char *)get((value_t)key,TYPE_INT32); }
	char *getString(unsigned long key) { return (char *)get((value_t)key,TYPE_UINT32); }
	char *getString(float key) { return (char *)get(*((value_t *)((void *)&key)),TYPE_FLOAT); }
	char *getString(const char *key) { return (char *)get(key); }
	char *getString(Object *key) { return (char *)get((value_t)key->hash(),TYPE_OBJECT_P); }
	char *getString(Object &key) { return (char *)get((value_t)key.hash(),TYPE_OBJECT_P); }

	size_t get(void *key,Vector &v) { return get((value_t)key,TYPE_VOID_P,v); }
	size_t get(long key,Vector &v) { return get((value_t)key,TYPE_INT32,v); }
	size_t get(unsigned long key,Vector &v) { return get((value_t)key,TYPE_UINT32,v); }
	size_t get(float key,Vector &v) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT,v); }
	size_t get(const char *key,Vector &v);
	size_t get(Object *key,Vector &v) { return get((value_t)key->hash(),TYPE_OBJECT_P,v); }
	size_t get(Object &key,Vector &v) { return get((value_t)key.hash(),TYPE_OBJECT_P,v); }

	value_t getByIndex(size_t index,type_t &type);

	bool contains(void *key) { return get((value_t)key,TYPE_VOID_P)!=0ul; }
	bool contains(long key) { return get((value_t)key,TYPE_INT32)!=0ul; }
	bool contains(unsigned long key) { return get((value_t)key,TYPE_UINT32)!=0ul; }
	bool contains(float key) { return get(*((value_t *)((void *)&key)),TYPE_FLOAT)!=0ul; }
	bool contains(const char *key) { return get(key)!=0ul; }
	bool contains(Object *key) { return get((value_t)key->hash(),TYPE_OBJECT_P)!=0ul; }
	bool contains(Object &key) { return get((value_t)key.hash(),TYPE_OBJECT_P)!=0ul; }

	value_t remove(void *key) { return remove((value_t)key,TYPE_VOID_P); }
	value_t remove(long key) { return remove((value_t)key,TYPE_INT32); }
	value_t remove(unsigned long key) { return remove((value_t)key,TYPE_UINT32); }
	value_t remove(float key) { return remove(*((value_t *)((void *)&key)),TYPE_FLOAT); }
	value_t remove(const char *key);
	value_t remove(Object *key) { return remove((value_t)key->hash(),TYPE_OBJECT_P); }
	value_t remove(Object &key) { return remove((value_t)key.hash(),TYPE_OBJECT_P); }

	value_t remove(void *key,type_t &type) { return remove((value_t)key,TYPE_VOID_P,type); }
	value_t remove(long key,type_t &type) { return remove((value_t)key,TYPE_INT32,type); }
	value_t remove(unsigned long key,type_t &type) { return remove((value_t)key,TYPE_UINT32,type); }
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


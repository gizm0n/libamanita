#ifndef _IPOMOEA_HASHTABLE_T_H
#define _IPOMOEA_HASHTABLE_T_H

/**
 * @file ipomoea/hashtable_t.h  
 * @author Per Löwgren
 * @date Modified: 2013-12-29
 * @date Created: 2012-10-08
 */ 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum {
	HT_TYPE_EMPTY,
	HT_TYPE_INT,
	HT_TYPE_STRING,
	HT_TYPE_TABLE,
	HT_TYPE_VECTOR,
	HT_TYPE_POINTER
};


typedef struct ht_node {
	uintptr_t hash;
	char *key;
	void *value;
	char vtype;
	struct ht_node *next;
} ht_node;


typedef struct hashtable {
	size_t size;
	size_t cap;
	ht_node **table;
} hashtable_t;


hashtable_t *ht_new(size_t c);
void ht_delete(hashtable_t *ht);
hashtable_t *ht_dup(const hashtable_t *ht);
void ht_resize(hashtable_t *ht,size_t c);
uintptr_t ht_hash(const char *key);
void ht_put_int(hashtable_t *ht,const char *k,int v);
void ht_put_string(hashtable_t *ht,const char *k,const char *v);
void ht_put_table(hashtable_t *ht,const char *k,void *v);
void ht_put_vector(hashtable_t *ht,const char *k,void *v);
void ht_put(hashtable_t *ht,const char *k,void *v);
void ht_put_type(hashtable_t *ht,const char *k,void *v,int t);
void *ht_get(const hashtable_t *ht,const char *k);
void *ht_get_type(const hashtable_t *ht,const char *k,int *t);
void ht_remove(hashtable_t *ht,const char *k);
int ht_size(const hashtable_t *ht);
int ht_capacity(const hashtable_t *ht);
void ht_print(const hashtable_t *ht);
void ht_print_indent(const hashtable_t *ht,int ind);


#ifdef __cplusplus
}
#endif
#endif /* _IPOMOEA_HASHTABLE_T_H */


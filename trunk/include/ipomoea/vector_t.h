#ifndef _IPOMOEA_VECTOR_T_H
#define _IPOMOEA_VECTOR_T_H

#include <stdint.h>


enum {
	VEC_TYPE_EMPTY,
	VEC_TYPE_INT,
	VEC_TYPE_STRING,
	VEC_TYPE_TABLE,
	VEC_TYPE_VECTOR,
	VEC_TYPE_POINTER
};


typedef struct vec_node {
	void *value;
	char vtype;
} vec_node;


typedef struct vector {
	size_t size;
	size_t cap;
	vec_node *list;
} vector_t;


vector_t *vec_new(size_t c);
void vec_delete(vector_t *vec);
vector_t *vec_dup(vector_t *vec);
void vec_resize(vector_t *vec,size_t c);
void vec_add_int(vector_t *vec,int v);
void vec_add_string(vector_t *vec,const char *v);
void vec_add_table(vector_t *vec,void *v);
void vec_add_vector(vector_t *vec,void *v);
void vec_add(vector_t *vec,void *v);
void vec_add_type(vector_t *vec,void *v,int t);
void *vec_get(vector_t *vec,int i);
void *vec_get_type(vector_t *vec,int i,int *t);
void vec_remove(vector_t *vec,int i);
void vec_split(vector_t *vec,const char *str,const char *delim);
void vec_split_all(vector_t *vec,const char *str,const char *delim);
char *vec_join(vector_t *vec,const char *delim);
void vec_sort(vector_t *vec);
void vec_isort(vector_t *vec);
void vec_reverse(vector_t *vec);
int vec_size(vector_t *vec);
int vec_capacity(vector_t *vec);
void vec_print(vector_t *vec);
void vec_print_indent(vector_t *vec,int ind);

#endif /* _IPOMOEA_VECTOR_T_H */


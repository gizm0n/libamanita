
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipomoea/hashtable_t.h>
#include <ipomoea/string_t.h>
#include <ipomoea/vector_t.h>

/*
enum {
	VEC_TYPE_EMPTY,
	VEC_TYPE_INT,
	VEC_TYPE_STRING,
	VEC_TYPE_TABLE,
	VEC_TYPE_VECTOR,
	VEC_TYPE_POINTER
};


typedef struct hashtable;


typedef struct vec_node {
	void *value;
	char vtype;
} vec_node;


typedef struct vector {
	size_t size;
	size_t cap;
	vec_node *list;
} vector_t;
*/

extern FILE *pm_out;

vector_t *vec_new(size_t c) {
	vector_t *vec = (vector_t *)malloc(sizeof(vector_t));
	memset(vec,0,sizeof(vector_t));
	if(c<=0) c = 4;
	vec->size = 0;
	vec->cap = c;
	vec->list = (vec_node *)malloc(sizeof(vec_node)*c);
	return vec;
}

void vec_delete(vector_t *vec) {
	size_t i;
	for(i=0; i<vec->size; i++)
		if(vec->list[i].vtype==VEC_TYPE_STRING) free(vec->list[i].value);
	free(vec->list);
	free(vec);
}

vector_t *vec_dup(vector_t *vec) {
	size_t i;
	vector_t *vec2;
	vec_node *n1,*n2;
	if(vec->size==0) return vec_new(0);
	vec2 = vec_new(vec->cap);
	for(i=0; i<vec->size; i++) {
		n1 = &vec->list[i];
		n2 = &vec2->list[i];
		if(n1->vtype==VEC_TYPE_STRING) n2->value = strdup(n1->value);
		else n2->value = n1->value;
		n2->vtype = n1->vtype;
	}
	return vec2;
}

void vec_resize(vector_t *vec,size_t c) {
	if(c<=vec->cap) {
		if(c<=0) c = vec->cap*2;
		else c = vec->cap+c;
	}
	vec->cap = c;
	vec->list = (vec_node *)realloc(vec->list,sizeof(vec_node)*c);
}

void vec_add_int(vector_t *vec,int v) { vec_add_type(vec,(void *)((intptr_t)v),VEC_TYPE_INT); }
void vec_add_string(vector_t *vec,const char *v) { vec_add_type(vec,(void *)v,VEC_TYPE_STRING); }
void vec_add_table(vector_t *vec,void *v) { vec_add_type(vec,v,VEC_TYPE_TABLE); }
void vec_add_vector(vector_t *vec,void *v) { vec_add_type(vec,v,VEC_TYPE_VECTOR); }
void vec_add(vector_t *vec,void *v) { vec_add_type(vec,v,VEC_TYPE_POINTER); }

void vec_add_type(vector_t *vec,void *v,int t) {
//fprintf(pm_out,"vec_add(%s,%d[%d])\n",k,hash,m);
//fflush(pm_out);
	vec_node *n;
	if(vec->size==vec->cap) vec_resize(vec,0);
	n = &vec->list[vec->size];
	if(v==NULL) t = VEC_TYPE_EMPTY;
	if(t==VEC_TYPE_STRING) n->value = strdup((char *)v);
	else n->value = v;
	n->vtype = t;
	vec->size++;
}

void *vec_get(vector_t *vec,int i) {
	if(i>=0 && i<vec->size) return vec->list[i].value;
	return 0;
}

void *vec_get_type(vector_t *vec,int i,int *t) {
	if(i>=0 && i<vec->size) {
		*t = vec->list[i].vtype;
		return vec->list[i].value;
	}
	*t = VEC_TYPE_EMPTY;
	return 0;
}

void vec_remove(vector_t *vec,int i) {
	if(i>=0 && i<vec->size) {
		if(i<vec->size-1) memmove(&vec->list[i],&vec->list[i+1],sizeof(vec_node)*(vec->size-i-1));
		vec->size--;
	}
}

void vec_split(vector_t *vec,const char *str,const char *delim) {
	char *s,*p1,*p2;
	if(str==NULL) return;
	s = strdup(str);
	p1 = s;
	if(delim!=NULL && *delim!='\0') {
		int dlen = strlen(delim);
		while(*p1!='\0') {
			p2 = strstr(p1,delim);
			if(p2==NULL) break;
			*p2 = '\0',p2 += dlen;
			if(*p2=='\0') break;
			if(*p1!='\0') vec_add_string(vec,p1);
			p1 = p2;
		}
	}
	vec_add_string(vec,p1);
	free(s);
}

void vec_split_all(vector_t *vec,const char *str,const char *delim) {
	char *s,*p1,*p2;
	if(str==NULL) return;
	s = strdup(str);
	p1 = s;
	if(delim!=NULL && *delim!='\0') {
		int dlen = strlen(delim);
		while(*p1!='\0') {
			p2 = strstr(p1,delim);
			if(p2==NULL) break;
			*p2 = '\0',p2 += dlen;
			vec_add_string(vec,p1);
			p1 = p2;
		}
	}
	vec_add_string(vec,p1);
	free(s);
}

char *vec_join(vector_t *vec,const char *delim) {
	int i;
	vec_node *n;
	string_t *str = str_new(NULL);
	char *s;
	for(i=0; i<vec->size; i++) {
		n = &vec->list[i];
		if(n->vtype==VEC_TYPE_INT) str_append_int(str,(int)((intptr_t)n->value));
		else if(n->vtype==VEC_TYPE_STRING) str_append(str,(char *)n->value);
		else continue;
		if(i<vec->size-1) str_append(str,delim);
	}
	s = strdup(str->str);
	str_delete(str);
	return s;
}

static int vec_compare(const void *a,const void *b) {
	vec_node *n1 = (vec_node *)a,*n2 = (vec_node *)b;
	if(n1->vtype!=n2->vtype) return n1->vtype>n2->vtype? 1 : -1;
	if(n1->vtype==VEC_TYPE_INT) return (int)((intptr_t)n1->value)-((intptr_t)n2->value);
	if(n1->vtype==VEC_TYPE_STRING) {
		char *s1 = (char *)n1->value,*s2 = (char *)n2->value;
		if(*s1=='\0' || *s2=='\0') return *s1=='\0'? (*s2=='\0'? 0 : 1) : -1;
		if(*s1>='a' && *s1<='z' && *s2>='A' && *s2<='Z') return -1;
		if(*s1>='A' && *s1<='Z' && *s2>='a' && *s2<='z') return 1;
		return strcmp(s1,s2);
	}
	return 0;
}

static int vec_icompare(const void *a,const void *b) {
	vec_node *n1 = (vec_node *)a,*n2 = (vec_node *)b;
	if(n1->vtype!=n2->vtype) return n1->vtype>n2->vtype? 1 : -1;
	if(n1->vtype==VEC_TYPE_INT) return (int)((intptr_t)n1->value)-((intptr_t)n2->value);
	if(n1->vtype==VEC_TYPE_STRING) {
		char *s1 = (char *)n1->value,*s2 = (char *)n2->value;
		if(*s1=='\0' || *s2=='\0') return *s1=='\0'? (*s2=='\0'? 0 : 1) : -1;
		return stricmp(s1,s2);
	}
	return 0;
}

void vec_sort(vector_t *vec) {
	qsort((void *)vec->list,vec->size,sizeof(vec_node),vec_compare);
}

void vec_isort(vector_t *vec) {
	qsort((void *)vec->list,vec->size,sizeof(vec_node),vec_icompare);
}

void vec_reverse(vector_t *vec) {
	int i,n = vec->size/2;
	vec_node n1;
	for(i=0; i<n; i++) {
		n1 = vec->list[i];
		vec->list[i] = vec->list[vec->size-1-i];
		vec->list[vec->size-1-i] = n1;
	}
}

int vec_size(vector_t *vec) {
	return vec!=NULL? (int)vec->size : 0;
}

int vec_capacity(vector_t *vec) {
	return vec!=NULL? (int)vec->cap : 0;
}

void vec_print(vector_t *vec) {
	printf("Vector (size=%lu,capacity=%lu):\n",(unsigned long)vec->size,(unsigned long)vec->cap);
	vec_print_indent(vec,0);
}

void vec_print_indent(vector_t *vec,int ind) {
	size_t i;
	int j;
	vec_node *n;
	for(i=0; i<vec->size; i++) {
		n = &vec->list[i];
		for(j=0; j<ind; j++) fputc('\t',stdout);
		printf("[%lu] ",(unsigned long)i);
		if(n->vtype==VEC_TYPE_EMPTY) printf("(empty)\n");
		else if(n->vtype==VEC_TYPE_INT) printf("%d\n",(int)((intptr_t)n->value));
		else if(n->vtype==VEC_TYPE_STRING) printf("\"%s\"\n",(char *)n->value);
		else if(n->vtype==VEC_TYPE_TABLE) {
			printf("table[\n");
			ht_print_indent((hashtable_t *)n->value,ind+1);
			for(j=0; j<ind; j++) fputc('\t',stdout);
			printf("]\n");
		} else if(n->vtype==VEC_TYPE_VECTOR) {
			printf("vector_t[\n");
			vec_print_indent((vector_t *)n->value,ind+1);
			for(j=0; j<ind; j++) fputc('\t',stdout);
			printf("]\n");
		} else printf("%p\n",n->value);
	}
	fflush(stdout);
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ipomoea/hashtable_t.h>
#include <ipomoea/vector_t.h>

/*
enum {
	HT_TYPE_INT,
	HT_TYPE_STRING,
	HT_TYPE_POINTER
};

typedef struct ht_node {
	intptr_t hash;
	char *key;
	void *value;
	char vtype
	struct ht_node *next;
} ht_node;


typedef struct hashtable {
	int size;
	int cap;
	ht_node **table;
} hashtable_t;
*/

extern FILE *pm_out;

hashtable_t *ht_new(size_t c) {
	hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));
	memset(ht,0,sizeof(hashtable_t));
	if(c<=0) c = 4;
	ht->size = 0,ht->cap = c;
	ht->table = (ht_node **)malloc(sizeof(ht_node *)*c);
	memset(ht->table,0,sizeof(ht_node *)*c);
	return ht;
}

void ht_delete(hashtable_t *ht) {
	size_t i;
	ht_node *n1,*n2;
	for(i=0; i<ht->cap; i++) for(n1=ht->table[i]; n1; n1=n2) {
		n2 = n1->next;
		free(n1->key);
		if(n1->vtype==HT_TYPE_STRING) free(n1->value);
		free(n1);
	}
	free(ht->table);
	free(ht);
}

hashtable_t *ht_dup(const hashtable_t *ht) {
	size_t i,m;
	hashtable_t *ht2;
	ht_node *n,*n1,*n2;
	if(ht->size==0) return ht_new(0);
	ht2 = ht_new(ht->cap);
	for(i=0; i<ht->cap; i++) for(n1=ht->table[i]; n1; n1=n2) {
		n2 = n1->next;
		n = (ht_node *)malloc(sizeof(ht_node));
		n->hash = n1->hash,n->key = strdup(n1->key);
		if(n1->vtype==HT_TYPE_STRING) n->value = strdup(n1->value);
		else n->value = n1->value;
		n->vtype = n1->vtype;
		m = n->hash%ht2->cap,n->next = ht2->table[m],ht2->table[m] = n;
	}
	return ht2;
}

void ht_resize(hashtable_t *ht,size_t c) {
	size_t i,m;
	ht_node *n1,*n2,**t;
//	owl_var_print(h);
	if(c<=ht->cap) {
		if(c<=0) c = ht->cap*2;
		else c = ht->cap+c;
	}
//fprintf(pm_out,"ht_resize(%d=>%d)\n",ht->cap,c);
//fflush(pm_out);
	t = (ht_node **)malloc(sizeof(ht_node *)*c);
	memset(t,0,sizeof(ht_node *)*c);
	if(ht->size>0) for(i=0; i<ht->cap; i++)
		for(n1=ht->table[i]; n1!=NULL; n1=n2) {
			n2 = n1->next,m = n1->hash%c,n1->next = t[m],t[m] = n1;
//fprintf(pm_out,"node(%s,%d,%d[%d],%p)\n",n1->key,i,n1->hash,m,t[m]);
//fflush(pm_out);
		}
	ht->cap = c;
	free(ht->table);
	ht->table = t;
}

uintptr_t ht_hash(const char *k) {
	uintptr_t h = 0;
	for(; *k; k++) h = 31*h+(*k);
	return h;
}

void ht_put_int(hashtable_t *ht,const char *k,int v) { ht_put_type(ht,k,(void *)((intptr_t)v),HT_TYPE_INT); }
void ht_put_string(hashtable_t *ht,const char *k,const char *v) { ht_put_type(ht,k,(void *)v,HT_TYPE_STRING); }
void ht_put_table(hashtable_t *ht,const char *k,void *v) { ht_put_type(ht,k,v,HT_TYPE_TABLE); }
void ht_put_vector(hashtable_t *ht,const char *k,void *v) { ht_put_type(ht,k,v,HT_TYPE_VECTOR); }
void ht_put(hashtable_t *ht,const char *k,void *v) { ht_put_type(ht,k,v,HT_TYPE_POINTER); }

void ht_put_type(hashtable_t *ht,const char *k,void *v,int t) {
	uintptr_t hash = ht_hash(k);
	size_t m = hash%ht->cap;
//fprintf(pm_out,"ht_put(%s,%d[%d])\n",k,hash,m);
//fflush(pm_out);
	ht_node *n;
	for(n=ht->table[m]; n; n=n->next)
		if(strcmp(n->key,k)==0) break;
	if(n && n->vtype==HT_TYPE_STRING) free(n->value);
	else {
		if(ht->size>=ht->cap/2) ht_resize(ht,0);
		n = (ht_node *)malloc(sizeof(ht_node));
		n->hash = hash,n->key = strdup(k);
		n->next = ht->table[m],ht->table[m] = n,ht->size++;
//fprintf(pm_out,"#");
	}
	if(v==NULL) t = HT_TYPE_EMPTY;
	if(t==HT_TYPE_STRING) n->value = strdup((char *)v);
	else n->value = v;
	n->vtype = t;
}

void *ht_get(const hashtable_t *ht,const char *k) {
	size_t m = ht_hash(k)%ht->cap;
	ht_node *n;
	for(n=ht->table[m]; n; n=n->next)
		if(strcmp(n->key,k)==0) break;
	if(n) return n->value;
	return 0;
}

void *ht_get_type(const hashtable_t *ht,const char *k,int *t) {
	size_t m = ht_hash(k)%ht->cap;
	ht_node *n;
	for(n=ht->table[m]; n; n=n->next)
		if(strcmp(n->key,k)==0) break;
	if(n) {
		*t = n->vtype;
		return n->value;
	}
	*t = HT_TYPE_EMPTY;
	return 0;
}

void ht_remove(hashtable_t *ht,const char *k) {
	size_t m = ht_hash(k)%ht->cap;
	ht_node *n1,*n2;
	for(n1=ht->table[m],n2=0; n1; n1=n1->next) {
		if(strcmp(n1->key,k)==0) {
			if(!n2) ht->table[m] = n1->next;
			else n2->next = n1->next;
			free(n1->key);
			if(n1->vtype==HT_TYPE_STRING) free(n1->value);
			free(n1);
			ht->size--;
			break;
		}
		n2 = n1;
	}
}

int ht_size(const hashtable_t *ht) {
	return ht!=NULL? (int)ht->size : 0;
}

int ht_capacity(const hashtable_t *ht) {
	return ht!=NULL? (int)ht->cap : 0;
}

void ht_print(const hashtable_t *ht) {
	printf("Table (size=%lu,capacity=%lu):\n",(unsigned long)ht->size,(unsigned long)ht->cap);
	ht_print_indent(ht,0);
}

void ht_print_indent(const hashtable_t *ht,int ind) {
	size_t i;
	int j;
	ht_node *n;
	for(i=0; i<ht->cap; i++) {
		if(ht->table[i]==NULL) continue;
		for(j=0; j<ind; j++) fputc('\t',stdout);
		printf("[%lu]",(unsigned long)i);
		for(n=ht->table[i]; n; n=n->next) {
			if(n==ht->table[i]) putchar(' ');
			else printf(", ");
			printf("%s => ",n->key);
			if(n->vtype==HT_TYPE_EMPTY) printf("(empty)");
			else if(n->vtype==HT_TYPE_INT) printf("%ld",(long)n->value);
			else if(n->vtype==HT_TYPE_STRING) printf("\"%s\"",(char *)n->value);
			else if(n->vtype==HT_TYPE_TABLE) {
				printf("table[\n");
				ht_print_indent((hashtable_t *)n->value,ind+1);
				for(j=0; j<ind; j++) fputc('\t',stdout);
				fputc(']',stdout);
			} else if(n->vtype==HT_TYPE_VECTOR) {
				printf("vector[\n");
				vec_print_indent((vector_t *)n->value,ind+1);
				for(j=0; j<ind; j++) fputc('\t',stdout);
				fputc(']',stdout);
			} else printf("%p",n->value);
		}
		fputc('\n',stdout);
	}
	fflush(stdout);
}


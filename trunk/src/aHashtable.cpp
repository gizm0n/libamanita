
#include "config.h"
#include <stdio.h>
#include <inttypes.h>
#include <libamanita/aHashtable.h>
#include <libamanita/aVector.h>



#define COMPARE(kt,vt,at) (!kt || at->k_type==kt) && (!vt || at->v_type==vt)


value_t aHashtable::iterator::first(type_t kt,type_t vt) {
	if(ht->sz) for(index=0; index<(long)ht->cap; index++) if((at=ht->table[index])) do {
		if(COMPARE(kt,vt,at)) return at->value;
		at = at->next;
	} while(at);
	index = ITER_INDEX_EMPTY;
	return 0;
}

value_t aHashtable::iterator::next() {
	if(!ht->sz) index = ITER_INDEX_EMPTY;
	else if(index!=ITER_INDEX_AFTER_LAST) {
		if(index<0) index = ITER_INDEX_BEFORE_FIRST;
		if(index>=0 && at && at->next) return (at=at->next)->value;
		while((++index)<(long)ht->cap)
			if((at=ht->table[index])) return at->value;
		index = ITER_INDEX_AFTER_LAST;
	}
	return 0;
}

value_t aHashtable::iterator::next(type_t kt,type_t vt) {
	if(!ht->sz) index = ITER_INDEX_EMPTY;
	else if(index!=ITER_INDEX_AFTER_LAST) {
		if(index<0) index = ITER_INDEX_BEFORE_FIRST;
		if(index>=0 && at && at->next)
			while((at=at->next)) if(COMPARE(kt,vt,at)) return at->value;
		while((++index)<(long)ht->cap)
			if((at=ht->table[index]))
				do if(COMPARE(kt,vt,at)) return at->value;while((at=at->next));
		index = ITER_INDEX_AFTER_LAST;
	}
	return 0;
}

value_t aHashtable::iterator::previous(type_t kt,type_t vt) {
	if(!ht->sz) index = ITER_INDEX_EMPTY;
	else if(index!=ITER_INDEX_BEFORE_FIRST) {
		if(index<0) index = ht->cap-1,at = 0;
		else if(at && at==ht->table[index]) index--,at = 0;
		for(node *n1,*n2; index>=0; index--) if((n1=ht->table[index])) do {
			n2 = n1;
			while(n2->next && n2->next!=at) n2 = n2->next;
			at = n2;
			if(COMPARE(kt,vt,at)) return at->value;
		} while(n2!=n1);
		index = ITER_INDEX_BEFORE_FIRST;
	}
	return 0;
}

value_t aHashtable::iterator::last(type_t kt,type_t vt) {
	if(ht->sz) {
		at = 0,index = ht->cap-1;
		for(node *n1,*n2; index>=0; index--) if((n1=ht->table[index])) do {
			n2 = n1;
			while(n2->next && n2->next!=at) n2 = n2->next;
			at = n2;
			if(COMPARE(kt,vt,at)) return at->value;
		} while(n2!=n1);
	}
	index = ITER_INDEX_EMPTY;
	return 0;
}

value_t aHashtable::iterator::remove() {
	node *n1 = at,*n2;
	if(index>=0 && at && (n2=ht->table[index])) {
		if(n2==at) ht->table[index--] = at->next,at = 0;
		else {
			while(n2->next!=at) n2 = n2->next;
			n2->next = at->next,at = n2;
		}
		value_t v = n1->value;
		delete n1;
		ht->sz--;
		if(!ht->sz) index = ITER_INDEX_EMPTY;
		return v;
	}
	return 0;
}



aObject_Inheritance(aHashtable,aCollection);


aHashtable::aHashtable(size_t c,float l,style_t st) : aCollection(),table(0),full(0),style(st),ld(l) { cap = c; }

aHashtable::iterator aHashtable::iterate() { return iterator(this); }


void aHashtable::setCaseInsensitiveKeys(bool b) {
	style_t st = style;
	st |= HASH_STYLE_CASE_INSENSITIVE;
	if(!b) st ^= HASH_STYLE_CASE_INSENSITIVE;
	if(((style&HASH_STYLE_CASE_INSENSITIVE)? true : false)!=b && sz>0) rehash(st);
	else style = st;
}

void aHashtable::setAllowKeyMultiples(bool b) {
	style |= HASH_STYLE_KEY_MULTIPLES;
	if(!b) style ^= HASH_STYLE_KEY_MULTIPLES;
}

hash_t aHashtable::hash(const char *s) {
	hash_t h = 0;
	if(style&HASH_STYLE_CASE_INSENSITIVE) while(*s) h = 31*h+aString::toLower(*s),s++;
	else while(*s) h = 31*h+(*s),s++;
	return h;
}

void aHashtable::put(value_t key,value_t value,type_t kt,type_t vt) {
	if(sz && !(style&HASH_STYLE_KEY_MULTIPLES)) {
		node *n = table[key%cap];
		while(n) {
			if(n->k_type==kt && n->key==key) break;
			n = n->next;
		}
		if(n) {
			if(n->v_type==TYPE_CHAR_P) free((char *)n->value);
#if _WORDSIZE < 64
			//else if(n->v_type==TYPE_INT64) free((long long *)n->value);
			//else if(n->v_type==TYPE_DOUBLE) free((double *)n->value);
#endif
			n->value = value,n->v_type = vt;
			return;
		}
	}
	if(sz==full) rehash(style);
	hash_t h = key%cap;
	table[h] = new node(key,key,value,kt,vt,table[h]);
	sz++;
}

void aHashtable::put(const char *key,value_t value,type_t vt) {
	if(sz && !(style&HASH_STYLE_KEY_MULTIPLES)) {
		node *n = table[hash(key)%cap];
		if(style&HASH_STYLE_CASE_INSENSITIVE) while(n) {
			if(n->k_type==TYPE_CHAR_P && !aString::stricmp((char *)n->key,key)) break;
			n = n->next;
		} else while(n) {
			if(n->k_type==TYPE_CHAR_P && !strcmp((char *)n->key,key)) break;
			n = n->next;
		}
		if(n) {
			if(n->v_type==TYPE_CHAR_P) free((char *)n->value);
#if _WORDSIZE < 64
			//else if(n->v_type==TYPE_INT64) free((long long *)n->value);
			//else if(n->v_type==TYPE_DOUBLE) free((double *)n->value);
#endif
			n->value = value,n->v_type = vt;
			return;
		}
	}
	if(sz==full) rehash(style);
	char *k = strdup(key);
	hash_t hs = hash(k),h = hs%cap;
	table[h] = new node((value_t)k,hs,value,TYPE_CHAR_P,vt,table[h]);
	sz++;
}

value_t aHashtable::get(value_t key,type_t kt) {
	if(!sz) return 0;
	node *n = table[key%cap];
	while(n) {
		if(n->k_type==kt && n->key==key) return n->value;
		n = n->next;
	}
	return 0;
}

value_t aHashtable::get(const char *key) {
	if(!sz) return 0;
	node *n = table[hash(key)%cap];
	if(style&HASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->k_type==TYPE_CHAR_P && !aString::stricmp((char *)n->key,key)) return n->value;
		n = n->next;
	} else while(n) {
		if(n->k_type==TYPE_CHAR_P && !strcmp((char *)n->key,key)) return n->value;
		n = n->next;
	}
	return 0;
}

value_t aHashtable::get(value_t key,type_t kt,type_t &type) {
	type = TYPE_EMPTY;
	if(!sz) return 0;
	node *n = table[key%cap];
	while(n) {
		if(n->k_type==kt && n->key==key) break;
		n = n->next;
	}
	if(!n) return 0;
	type = n->v_type;
	return n->value;
}

value_t aHashtable::get(const char *key,type_t &type) {
	type = TYPE_EMPTY;
	if(!sz) return 0;
	node *n = table[hash(key)%cap];
	if(style&HASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->k_type==TYPE_CHAR_P && !aString::stricmp((char *)n->key,key)) break;
		n = n->next;
	} else while(n) {
		if(n->k_type==TYPE_CHAR_P && !strcmp((char *)n->key,key)) break;
		n = n->next;
	}
	if(!n) return 0;
	type = n->v_type;
	return n->value;
}

size_t aHashtable::get(value_t key,type_t kt,aVector &v) {
	if(!sz) return 0;
	int i = 0;
	node *n = table[key%cap];
	while(n) {
		if(n->k_type==kt && n->key==key) v.insert(n->value,v.sz,n->v_type),i++;
		n = n->next;
	}
	return i;
}

size_t aHashtable::get(const char *key,aVector &v) {
	if(!sz) return 0;
	size_t i = 0;
	node *n = table[hash(key)%cap];
	if(style&HASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->k_type==TYPE_CHAR_P && !aString::stricmp((char *)n->key,key)) v.insert(n->value,v.sz,n->v_type),i++;
		n = n->next;
	} else while(n) {
		if(n->k_type==TYPE_CHAR_P && !strcmp((char *)n->key,key)) v.insert(n->value,v.sz,n->v_type),i++;
		n = n->next;
	}
	return i;
}

value_t aHashtable::getByIndex(size_t index,type_t &type) {
	type = TYPE_EMPTY;
	if(index>=sz) return 0;
	size_t i = 0;
	node *n = 0;
	for(; 1; index--) {
		if(n) n = n->next;
		if(!n) for(; !n && i<cap; i++) n = table[i];
		if(index==0) break;
	}
	if(!n) return 0;
	type = n->v_type;
	return n->value;
}

value_t aHashtable::remove(value_t key,type_t kt) {
	if(!sz) return 0;
	value_t k = key;
	node *n1 = table[k%cap];
	if(!n1) return 0;
	if(n1->k_type==kt && n1->key==k) table[k%cap] = n1->next;
	else {
		node *n2;
		do { n2 = n1,n1 = n1->next; } while(n1 && n1->k_type!=kt && n1->key!=k);
		if(!n1) return 0;
		n2->next = n1->next;
	}
	value_t v = n1->value;
	delete n1;
	sz--;
	return v;
}

value_t aHashtable::remove(const char *key) {
	if(!sz) return 0;
	hash_t h = hash(key)%cap;
	node *n1 = table[h];
	if(!n1) return 0;
	if(n1->k_type==TYPE_CHAR_P &&
		(((style&HASH_STYLE_CASE_INSENSITIVE) && !aString::stricmp((char *)n1->key,key)) ||
			(!(style&HASH_STYLE_CASE_INSENSITIVE) && !strcmp((char *)n1->key,key)))) table[h] = n1->next;
	else {
		node *n2;
		if(style&HASH_STYLE_CASE_INSENSITIVE)
			do { n2 = n1,n1 = n1->next; }
				while(n1 && n1->k_type!=TYPE_CHAR_P && !aString::stricmp((char *)n1->key,key));
		else do { n2 = n1,n1 = n1->next; } while(n1 && n1->k_type!=TYPE_CHAR_P && !strcmp((char *)n1->key,key));
		if(!n1) return 0;
		n2->next = n1->next;
	}
	value_t v = n1->value;
	delete n1;
	sz--;
	return v;
}

value_t aHashtable::remove(value_t key,type_t kt,type_t &type) {
	type = TYPE_EMPTY;
	if(!sz) return 0;
	value_t k = key;
	node *n1 = table[k%cap];
	if(!n1) return 0;
	if(n1->k_type==kt && n1->key==k) table[k%cap] = n1->next;
	else {
		node *n2;
		do { n2 = n1,n1 = n1->next; } while(n1 && n1->k_type!=kt && n1->key!=k);
		if(!n1) return 0;
		n2->next = n1->next;
	}
	type = n1->v_type;
	value_t v = n1->value;
	delete n1;
	sz--;
	return v;
}

value_t aHashtable::remove(const char *key,type_t &type) {
	type = TYPE_EMPTY;
	if(!sz) return 0;
	int h = hash(key)%cap;
	node *n1 = table[h];
	if(!n1) return 0;
	if(n1->k_type==TYPE_CHAR_P &&
		(((style&HASH_STYLE_CASE_INSENSITIVE) && !aString::stricmp((char *)n1->key,key)) ||
			(!(style&HASH_STYLE_CASE_INSENSITIVE) && !strcmp((char *)n1->key,key)))) table[h] = n1->next;
	else {
		node *n2;
		if(style&HASH_STYLE_CASE_INSENSITIVE)
			do { n2 = n1,n1 = n1->next; }
				while(n1 && n1->k_type!=TYPE_CHAR_P && !aString::stricmp((char *)n1->key,key));
		else do { n2 = n1,n1 = n1->next; } while(n1 && n1->k_type!=TYPE_CHAR_P && !strcmp((char *)n1->key,key));
		if(!n1) return 0;
		n2->next = n1->next;
	}
	type = n1->v_type;
	value_t v = n1->value;
	delete n1;
	sz--;
	return v;
}

void aHashtable::removeAll() {
	if(table) {
		node *n1,*n2;
		for(size_t i=0; i<cap; i++) if((n1=table[i]))
			do { n2 = n1->next;delete n1;n1 = n2; } while(n1);
		free(table);
		table = 0;
	}
	cap = 11,sz = 0,full = 0;
}

void aHashtable::rehash(style_t st) {
	if(!table) {
		sz = 0;
		table = (node **)malloc(cap*sizeof(node *));
		memset(table,0,cap*sizeof(node *));
	} else {
		size_t i,c = (size_t)((float)cap/ld);
		hash_t h;
		if((c&1)==0) c++;
		node **t1 = table;
		table = (node **)malloc(c*sizeof(node *));
		memset(table,0,c*sizeof(node *));

		node *n1,*n2;
		if(st!=style) {
			for(i=0; i<cap; i++) if((n1=t1[i])) while(n1) {
				if(n1->k_type==TYPE_CHAR_P) n1->hash = hash((char *)n1->key);
				n2 = n1->next,h = n1->hash%c,n1->next = table[h],table[h] = n1,n1 = n2;
			}
		} else  for(i=0; i<cap; i++) if((n1=t1[i])) while(n1)
			n2 = n1->next,h = n1->hash%c,n1->next = table[h],table[h] = n1,n1 = n2;

		free(t1);
		cap = c;
	}
	style = st;
	full = (size_t)((float)cap*ld);
//ffprintf(stderr,fp,"aHashtable::rehash(st=%d)\nSize=%d,Full=%d,Capacity=%d\n",st,sz,full,cap);
}

size_t aHashtable::print(const char *fn) {
	FILE *fp;
	if(!fn || !*fn || !(fp=fopen(fn,"w"))) return 0;
	size_t s = print(fp);
	fclose(fp);
	return s;
}

size_t aHashtable::print(FILE *fp) {
	if(!fp) return 0;
	size_t i,s = sz;
	node *n;
	for(i=0; i<cap; i++) {
		fprintf(fp,"Table[%zu]",i);
		if((n=table[i])) while(n) {
			switch(n->k_type) {
				case TYPE_VOID_P:fprintf(fp," %p",(void *)n->key);break;
				case TYPE_INTPTR:fprintf(fp," %" PRIuPTR,(intptr_t)n->key);break;
#if _WORDSIZE == 64
				case TYPE_DOUBLE:fprintf(fp," %f",*((double *)((void *)&n->key)));break;
#else
				//case TYPE_INT64:fprintf(fp," %" PRId64,*(long long *)n->key);break;
				case TYPE_FLOAT:fprintf(fp," %f",*((float *)((void *)&n->key)));break;
				//case TYPE_DOUBLE:fprintf(fp," %f",*((double *)((void *)&n->key)));break;
#endif
				case TYPE_CHAR_P:fprintf(fp," \"%s\"",(char *)n->key);break;
			}
			switch(n->v_type) {
				case TYPE_VOID_P:fprintf(fp,"=%p",(void *)n->value);break;
				case TYPE_INTPTR:fprintf(fp,"=%" PRIuPTR,(intptr_t)n->value);break;
#if _WORDSIZE == 64
				case TYPE_DOUBLE:fprintf(fp,"=%f",*((double *)((void *)&n->value)));break;
#else
				//case TYPE_INT64:fprintf(fp,"=%lu",(unsigned long)n->value);break;
				case TYPE_FLOAT:fprintf(fp,"=%f",*((float *)((void *)&n->value)));break;
				//case TYPE_DOUBLE:fprintf(fp,"=%f",*((double *)((void *)&n->value)));break;
#endif
				case TYPE_CHAR_P:fprintf(fp,"=\"%s\"",(char *)n->value);break;
				case TYPE_OBJECT_P:fprintf(fp,"=%p",(void *)n->value);break;
			}
			fflush(fp);
			if(n->next) s--;
			n = n->next;
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"Size=%zu,Full=%zu,Capacity=%zu,Spread=%f\n",sz,full,cap,100.0f*((float)s/(float)sz));
	fflush(fp);
	return sz;
}

size_t aHashtable::load(const char *fn) {
//fprintf(stderr,"aHashtable::load(%s)\n",fn);
//fflush(stderr);
	FILE *fp;
	if(!fn || !*fn || !(fp=fopen(fn,"rb"))) return 0;
	size_t s = load(fp);
	fclose(fp);
	return s;
}

size_t aHashtable::load(FILE *fp) {
	if(!fp) return 0;
//fprintf(stderr,"aHashtable::load()\n");
//fflush(stderr);
	aString key(64),val(256);
	size_t s = 0;
	while(!feof(fp)) {
//fprintf(stderr,"aHashtable::load(readPair)\n");
//fflush(stderr);
		readPair(fp,key.clear(),val.clear());
		if(key.length()>0 && val.length()>0) {
			put(key,val);
			s++;
//fprintf(stderr,"aHashtable::load(key='%s',val='%s')\n",key.toCharArray(),val.toCharArray());
//fflush(stderr);
		}
	}
	return s;
}

size_t aHashtable::save(const char *fn) {
	FILE *fp;
	if(!fn || !*fn || !(fp=fopen(fn,"wb"))) return 0;
	size_t s = save(fp);
	fclose(fp);
	return s;
}

size_t aHashtable::save(FILE *fp) {
	if(!fp) return 0;
	node *n;
	if(table) for(size_t i=0; i<cap; i++) if((n=table[i])) while(n) {
		switch(n->k_type) {
			case TYPE_VOID_P:fprintf(fp,"%p=",(void *)n->key);break;
			case TYPE_INTPTR:fprintf(fp,"%" PRIuPTR "=",(intptr_t)n->key);break;
#if _WORDSIZE == 64
			case TYPE_DOUBLE:fprintf(fp,"%f=",*((double *)((void *)&n->key)));break;
#else
			//case TYPE_INT64:fprintf(fp,"%" PRId64 "=",*(long long *)n->key);break;
			case TYPE_FLOAT:fprintf(fp,"%f=",*((float *)((void *)&n->key)));break;
			//case TYPE_DOUBLE:fprintf(fp,"%f=",*((double *)((void *)&n->key)));break;
#endif
			case TYPE_CHAR_P:fprintf(fp,"%s=",(char *)n->key);break;
		}
		switch(n->v_type) {
			case TYPE_VOID_P:fprintf(fp,"%p\n",(void *)n->value);break;
			case TYPE_INTPTR:fprintf(fp,"%" PRIuPTR "\n",(intptr_t)n->value);break;
#if _WORDSIZE == 64
			case TYPE_DOUBLE:fprintf(fp,"%f\n",*((double *)((void *)&n->key)));break;
#else
			//case TYPE_UINT32:fprintf(fp,"%lu\n",(unsigned long)n->value);break;
			case TYPE_FLOAT:fprintf(fp,"%f\n",*((float *)((void *)&n->key)));break;
			//case TYPE_DOUBLE:fprintf(fp,"%f\n",*((double *)((void *)&n->key)));break;
#endif
			case TYPE_CHAR_P:fprintf(fp,"%s\n",(char *)n->value);break;
			case TYPE_OBJECT_P:fprintf(fp,"%p\n",(void *)n->value);break;
		}
		n = n->next;
	}
	return sz;
}

void aHashtable::readPair(FILE *fp,aString &key,aString &value) {
	key.appendUntil(fp,"=:",aString::whitespace);
	if(!feof(fp)) value.appendUntil(fp,aString::whitespace+2,aString::whitespace);
}



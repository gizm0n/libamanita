
#include "_config.h"
#include <stdio.h>
#include <inttypes.h>
#include <amanita/aHashtable.h>
#include <amanita/aVector.h>



#define COMPARE(kt,vt,at) (!kt || at->k_type==kt) && (!vt || at->v_type==vt)


value_t aHashtable::iterator::first(type_t kt,type_t vt) {
	if(ht->sz) for(index=0; index<(long)ht->cap; index++) if((at=ht->table[index])) do {
		if(COMPARE(kt,vt,at)) return at->value;
		at = at->next;
	} while(at);
	index = aITER_INDEX_EMPTY;
	return 0;
}

value_t aHashtable::iterator::next() {
	if(!ht->sz) index = aITER_INDEX_EMPTY;
	else if(index!=aITER_INDEX_AFTER_LAST) {
		if(index<0) index = aITER_INDEX_BEFORE_FIRST;
		if(index>=0 && at && at->next) return (at=at->next)->value;
		while((++index)<(long)ht->cap)
			if((at=ht->table[index])) return at->value;
		index = aITER_INDEX_AFTER_LAST;
	}
	return 0;
}

value_t aHashtable::iterator::next(type_t kt,type_t vt) {
	if(!ht->sz) index = aITER_INDEX_EMPTY;
	else if(index!=aITER_INDEX_AFTER_LAST) {
		if(index<0) index = aITER_INDEX_BEFORE_FIRST;
		if(index>=0 && at && at->next)
			while((at=at->next)) if(COMPARE(kt,vt,at)) return at->value;
		while((++index)<(long)ht->cap)
			if((at=ht->table[index]))
				do if(COMPARE(kt,vt,at)) return at->value;while((at=at->next));
		index = aITER_INDEX_AFTER_LAST;
	}
	return 0;
}

value_t aHashtable::iterator::previous(type_t kt,type_t vt) {
	if(!ht->sz) index = aITER_INDEX_EMPTY;
	else if(index!=aITER_INDEX_BEFORE_FIRST) {
		if(index<0) index = ht->cap-1,at = 0;
		else if(at && at==ht->table[index]) index--,at = 0;
		for(node *n1,*n2; index>=0; index--) if((n1=ht->table[index])) do {
			n2 = n1;
			while(n2->next && n2->next!=at) n2 = n2->next;
			at = n2;
			if(COMPARE(kt,vt,at)) return at->value;
		} while(n2!=n1);
		index = aITER_INDEX_BEFORE_FIRST;
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
	index = aITER_INDEX_EMPTY;
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
		if(!ht->sz) index = aITER_INDEX_EMPTY;
		return v;
	}
	return 0;
}



aObject_Inheritance(aHashtable,aCollection)


aHashtable::aHashtable(size_t c,float l,style_t st) : aCollection(),table(0),full(0),style(st),ld(l) { cap = c; }

aHashtable::iterator aHashtable::iterate() { return iterator(this); }


void aHashtable::setCaseInsensitiveKeys(bool b) {
	style_t st = style;
	st |= aHASH_STYLE_CASE_INSENSITIVE;
	if(!b) st ^= aHASH_STYLE_CASE_INSENSITIVE;
	if(((style&aHASH_STYLE_CASE_INSENSITIVE)? true : false)!=b && sz>0) rehash(st);
	else style = st;
}

void aHashtable::setAllowKeyMultiples(bool b) {
	style |= aHASH_STYLE_KEY_MULTIPLES;
	if(!b) style ^= aHASH_STYLE_KEY_MULTIPLES;
}

void aHashtable::put(node &n) {
	if(n.k_type==aCHAR_P) put((const char *)n.key,n.value,n.v_type);
	else put(n.key,n.value,n.k_type,n.v_type);
}

void aHashtable::put(value_t key,value_t value,type_t kt,type_t vt) {
	if(sz && !(style&aHASH_STYLE_KEY_MULTIPLES)) {
		node *n = table[key%cap];
		while(n) {
			if(n->k_type==kt && n->key==key) break;
			n = n->next;
		}
		if(n) {
			if(n->v_type==aCHAR_P) free((char *)n->value);
#if _WORDSIZE < 64
			//else if(n->v_type==aINT64) free((long long *)n->value);
			//else if(n->v_type==aDOUBLE) free((double *)n->value);
#endif
			n->value = value,n->v_type = vt;
			return;
		}
	}
	if(sz==full) rehash(style);
	int c = key%cap;
	table[c] = new node(key,(hash_t)key,value,kt,vt,table[c]);
	sz++;
}

void aHashtable::put(const char *key,value_t value,type_t vt) {
	hash_t h;
	if(sz && !(style&aHASH_STYLE_KEY_MULTIPLES)) {
		h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
		node *n = table[h%cap];
		if(style&aHASH_STYLE_CASE_INSENSITIVE) for(; n; n=n->next)
			if(n->k_type==aCHAR_P && n->hash==h && !aString::stricmp((char *)n->key,key)) break;
		else for(; n; n=n->next)
			if(n->k_type==aCHAR_P && n->hash==h && !strcmp((char *)n->key,key)) break;
		if(n) {
			if(n->v_type==aCHAR_P) free((char *)n->value);
#if _WORDSIZE < 64
			//else if(n->v_type==aINT64) free((long long *)n->value);
			//else if(n->v_type==aDOUBLE) free((double *)n->value);
#endif
			n->value = value,n->v_type = vt;
			return;
		}
	}
	if(sz==full) rehash(style);
	h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	int c = h%cap;
	table[c] = new node((value_t)strdup(key),h,value,aCHAR_P,vt,table[c]);
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
	hash_t h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	node *n = table[h%cap];
	if(style&aHASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->k_type==aCHAR_P && n->hash==h && !aString::stricmp((char *)n->key,key)) return n->value;
		n = n->next;
	} else while(n) {
		if(n->k_type==aCHAR_P && n->hash==h && !strcmp((char *)n->key,key)) return n->value;
		n = n->next;
	}
	return 0;
}

value_t aHashtable::get(value_t key,type_t kt,type_t &type) {
	type = aEMPTY;
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
	type = aEMPTY;
	if(!sz) return 0;
	hash_t h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	node *n = table[h%cap];
	if(style&aHASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->k_type==aCHAR_P && n->hash==h && !aString::stricmp((char *)n->key,key)) break;
		n = n->next;
	} else while(n) {
		if(n->k_type==aCHAR_P && n->hash==h && !strcmp((char *)n->key,key)) break;
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
		if(n->k_type==kt && n->key==key) v.insert(n->value,v.sz,n->v_type),++i;
		n = n->next;
	}
	return i;
}

size_t aHashtable::get(const char *key,aVector &v) {
	if(!sz) return 0;
	size_t i = 0;
	hash_t h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	node *n = table[h%cap];
	if(style&aHASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->k_type==aCHAR_P && n->hash==h && !aString::stricmp((char *)n->key,key)) v.insert(n->value,v.sz,n->v_type),++i;
		n = n->next;
	} else while(n) {
		if(n->k_type==aCHAR_P && n->hash==h && !strcmp((char *)n->key,key)) v.insert(n->value,v.sz,n->v_type),++i;
		n = n->next;
	}
	return i;
}

value_t aHashtable::getByIndex(size_t index,type_t &type) {
	type = aEMPTY;
	if(index>=sz) return 0;
	size_t i = 0;
	node *n = 0;
	for(; 1; index--) {
		if(n) n = n->next;
		if(!n) for(; !n && i<cap; ++i) n = table[i];
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
	hash_t h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	int c = h%cap;
	node *n1 = table[c];
	if(!n1) return 0;
	if(n1->k_type==aCHAR_P && n1->hash==h &&
		(((style&aHASH_STYLE_CASE_INSENSITIVE) && !aString::stricmp((char *)n1->key,key)) ||
			(!(style&aHASH_STYLE_CASE_INSENSITIVE) && !strcmp((char *)n1->key,key)))) table[c] = n1->next;
	else {
		node *n2;
		if(style&aHASH_STYLE_CASE_INSENSITIVE)
			do { n2 = n1,n1 = n1->next; }
				while(n1 && n1->k_type!=aCHAR_P && n1->hash!=h && aString::stricmp((char *)n1->key,key));
		else do { n2 = n1,n1 = n1->next; } while(n1 && n1->k_type!=aCHAR_P && n1->hash!=h && strcmp((char *)n1->key,key));
		if(!n1) return 0;
		n2->next = n1->next;
	}
	value_t v = n1->value;
	delete n1;
	sz--;
	return v;
}

value_t aHashtable::remove(value_t key,type_t kt,type_t &type) {
	type = aEMPTY;
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
	type = aEMPTY;
	if(!sz) return 0;
	hash_t h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	int c = h%cap;
	node *n1 = table[c];
	if(!n1) return 0;
	if(n1->k_type==aCHAR_P && n1->hash==h &&
		(((style&aHASH_STYLE_CASE_INSENSITIVE) && !aString::stricmp((char *)n1->key,key)) ||
			(!(style&aHASH_STYLE_CASE_INSENSITIVE) && !strcmp((char *)n1->key,key)))) table[c] = n1->next;
	else {
		node *n2;
		if(style&aHASH_STYLE_CASE_INSENSITIVE)
			do { n2 = n1,n1 = n1->next; }
				while(n1 && n1->k_type!=aCHAR_P && n1->hash!=h && aString::stricmp((char *)n1->key,key));
		else do { n2 = n1,n1 = n1->next; } while(n1 && n1->k_type!=aCHAR_P && n1->hash!=h && strcmp((char *)n1->key,key));
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
		for(size_t i=0; i<cap; ++i) if((n1=table[i]))
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
			for(i=0; i<cap; ++i) if((n1=t1[i])) while(n1) {
				if(n1->k_type==aCHAR_P)
					n1->hash = aString::crc32((char *)n1->key,style&aHASH_STYLE_CASE_INSENSITIVE);
				n2 = n1->next,h = n1->hash%c,n1->next = table[h],table[h] = n1,n1 = n2;
			}
		} else  for(i=0; i<cap; ++i) if((n1=t1[i])) while(n1)
			n2 = n1->next,h = n1->hash%c,n1->next = table[h],table[h] = n1,n1 = n2;

		free(t1);
		cap = c;
	}
	style = st;
	full = (size_t)((float)cap*ld);
//ffprintf(stderr,fp,"aHashtable::rehash(st=%d)\nSize=%d,Full=%d,Capacity=%d\n",st,sz,full,cap);
}

size_t aHashtable::print(const char *fn) {
	size_t s = 0;
	if(fn && *fn) {
		FILE *fp;
		if((fp=fopen(fn,"wb"))) {
			s = print(fp);
			fclose(fp);
		} else perror(fn);
	}
	return s;
}

size_t aHashtable::print(FILE *fp) {
	if(!fp) return 0;
	size_t i,s = sz;
	node *n;
	for(i=0; i<cap; ++i) {
		fprintf(fp,"Table[%lu]",(unsigned long)i);
		if((n=table[i])) while(n) {
			switch(n->k_type) {
				case aVOID:fprintf(fp," %p",(void *)n->key);break;
				case aINTPTR:fprintf(fp," %" PRIdPTR,(intptr_t)n->key);break;
#if _WORDSIZE == 64
				case aDOUBLE:fprintf(fp," %f",*((double *)((void *)&n->key)));break;
#else
				//case aINT64:fprintf(fp," %" PRId64,*(long long *)n->key);break;
				case aFLOAT:fprintf(fp," %f",*((float *)((void *)&n->key)));break;
				//case aDOUBLE:fprintf(fp," %f",*((double *)((void *)&n->key)));break;
#endif
				case aCHAR_P:fprintf(fp," \"%s\"",(char *)n->key);break;
			}
			switch(n->v_type) {
				case aVOID:fprintf(fp,"=%p",(void *)n->value);break;
				case aINTPTR:fprintf(fp,"=%" PRIdPTR,(intptr_t)n->value);break;
#if _WORDSIZE == 64
				case aDOUBLE:fprintf(fp,"=%f",*((double *)((void *)&n->value)));break;
#else
				//case aINT64:fprintf(fp,"=%lu",(unsigned long)n->value);break;
				case aFLOAT:fprintf(fp,"=%f",*((float *)((void *)&n->value)));break;
				//case aDOUBLE:fprintf(fp,"=%f",*((double *)((void *)&n->value)));break;
#endif
				case aCHAR_P:fprintf(fp,"=\"%s\"",(char *)n->value);break;
				case aOBJECT:fprintf(fp,"=%p",(void *)n->value);break;
			}
			fflush(fp);
			if(n->next) s--;
			n = n->next;
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"Size=%lu,Full=%lu,Capacity=%lu,Spread=%f\n",(unsigned long)sz,(unsigned long)full,(unsigned long)cap,100.0f*((float)s/(float)sz));
	fflush(fp);
	return sz;
}

size_t aHashtable::load(const char *fn,const char *k,const char *l) {
//fprintf(stderr,"aHashtable::load(%s)\n",fn);
//fflush(stderr);
	size_t s = 0;
	if(fn && *fn) {
		FILE *fp;
		if((fp=fopen(fn,"rb"))) {
			s = load(fp,k,l);
			fclose(fp);
		} else perror(fn);
	}
	return s;
}

size_t aHashtable::load(FILE *fp,const char *k,const char *l) {
	if(!fp) return 0;
//fprintf(stderr,"aHashtable::load()\n");
//fflush(stderr);
	aString key(64),val(256);
	size_t n = 0;
	if(!k) k = "=:";
	if(!l) l = aString::endl;
	while(!feof(fp)) {
//fprintf(stderr,"aHashtable::load(readPair)\n");
//fflush(stderr);
		key.clear();
		key.appendUntil(fp,k,aString::whitespace);
		val.clear();
		if(!feof(fp)) val.appendUntil(fp,l,aString::whitespace);
		if(key) {
			put(key,val);
			n++;
//fprintf(stderr,"aHashtable::load(key='%s',val='%s')\n",key.str,val.str);
//fflush(stderr);
		}
	}
	return n;
}

size_t aHashtable::save(const char *fn,const char *k,const char *l) {
	size_t s = 0;
	if(fn && *fn) {
		FILE *fp;
		if((fp=fopen(fn,"wb"))) {
			s = save(fp,k,l);
			fclose(fp);
		} else perror(fn);
	}
	return s;
}

size_t aHashtable::save(FILE *fp,const char *k,const char *l) {
	if(!fp) return 0;
	node *n;
	if(!k) k = "=";
	if(!l) l = aString::endl;
	if(table) for(size_t i=0; i<cap; ++i) if((n=table[i])) while(n) {
		switch(n->k_type) {
			case aVOID:fprintf(fp,"%p%s",(void *)n->key,k);break;
			case aINTPTR:fprintf(fp,"%" PRIdPTR "%s",(intptr_t)n->key,k);break;
#if _WORDSIZE == 64
			case aDOUBLE:fprintf(fp,"%f%s",*((double *)((void *)&n->key)),k);break;
#else
			//case aINT64:fprintf(fp,"%" PRId64 "%s",*(long long *)n->key,k);break;
			case aFLOAT:fprintf(fp,"%f%s",*((float *)((void *)&n->key)),k);break;
			//case aDOUBLE:fprintf(fp,"%f%s",*((double *)((void *)&n->key)),k);break;
#endif
			case aCHAR_P:fprintf(fp,"%s%s",(char *)n->key,k);break;
		}
		switch(n->v_type) {
			case aVOID:fprintf(fp,"%p%s",(void *)n->value,l);break;
			case aINTPTR:fprintf(fp,"%" PRIdPTR "%s",(intptr_t)n->value,l);break;
#if _WORDSIZE == 64
			case aDOUBLE:fprintf(fp,"%f%s",*((double *)((void *)&n->key)),l);break;
#else
			//case aUINT32:fprintf(fp,"%lu%s",(unsigned long)n->value,l);break;
			case aFLOAT:fprintf(fp,"%f%s",*((float *)((void *)&n->key)),l);break;
			//case aDOUBLE:fprintf(fp,"%f%s",*((double *)((void *)&n->key)),l);break;
#endif
			case aCHAR_P:fprintf(fp,"%s%s",(char *)n->value,l);break;
			case aOBJECT:fprintf(fp,"%p%s",(void *)n->value,l);break;
		}
		n = n->next;
	}
	return sz;
}


size_t aHashtable::merge(const char *str) {
	const char *s = str;
	aString key(64),val(256);
	size_t n = 0;
	while(*s!='\0') {
		key.clear();
		key.appendUntil(&s,"=:",aString::whitespace);
		val.clear();
		if(*s!='\0') val.appendUntil(&s,aString::whitespace+2,aString::whitespace);
		if(key) {
			put(key,val);
			n++;
		}
	}
	return n;
}

size_t aHashtable::merge(aHashtable &ht) {
	size_t n = 0;
	if(ht.table) {
		node *n1;
		for(long i=ht.cap-1; i>=0; --i) if((n1=ht.table[i]))
			do { put(*n1);n1 = n1->next,++n; } while(n1);
	}
	return n;
}

size_t aHashtable::merge(aVector &vec) {
	size_t n = 0;
	aVector::node *n1;
	for(long i=vec.sz-1; i>=0; --i) {
		n1 = &vec.list[i];
		put(i,n1->value,_aINTPTR,n1->type);
	}
	return n;
}



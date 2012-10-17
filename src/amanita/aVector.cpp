
#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <amanita/aVector.h>


value_t aVector::iterator::first(type_t type) {
	if(vec->sz) {
		if(!type) return vec->list[index=0].value;
		for(index=0; index<(long)vec->sz; index++)
			if(vec->list[index].type==type) return vec->list[index].value;
	}
	index = aITER_INDEX_EMPTY;
	return 0;
}

value_t aVector::iterator::next(type_t type) {
	if(!vec->sz) index = aITER_INDEX_EMPTY;
	else if(index!=aITER_INDEX_AFTER_LAST) {
		if(index<0) index = aITER_INDEX_BEFORE_FIRST;
		if(!type) {
			if(index<(long)vec->sz-1) return vec->list[++index].value;
		} else for(index++; index<(long)vec->sz; index++)
			if(vec->list[index].type==type) return vec->list[index].value;
		index = aITER_INDEX_AFTER_LAST;
	}
	return 0;
}

value_t aVector::iterator::previous(type_t type) {
	if(!vec->sz) index = aITER_INDEX_EMPTY;
	else if(index!=aITER_INDEX_BEFORE_FIRST) {
		if(index<0) index = vec->sz;
		if(!type) {
			if(index>0) return vec->list[--index].value;
		} else for(index--; index>=0; index--)
			if(vec->list[index].type==type) return vec->list[index].value;
		index = aITER_INDEX_BEFORE_FIRST;
	}
	return 0;
}

value_t aVector::iterator::last(type_t type) {
	if(vec->sz) {
		if(!type) return vec->list[index=vec->sz-1].value;
		for(index=vec->sz-1; index>=0; index--)
			if(vec->list[index].type==type) return vec->list[index].value;
	}
	index = aITER_INDEX_EMPTY;
	return 0;
}

value_t aVector::iterator::remove() {
	if(index>=0 && index<(long)vec->sz) {
		value_t v = vec->list[index].value;
		vec->removeAt(index--);
		return v;
	}
	return 0;
}



aObject_Inheritance(aVector,aCollection)


aVector::aVector() : aCollection(),inc(0),list(0) {}
aVector::aVector(size_t n) : aCollection(),inc(0),list(0) { cap = n; }
aVector::~aVector() { clear(); }


aVector::iterator aVector::iterate() {
	aVector::iterator iter(this);
	return iter;
}


inline long align_to_size(long n,long sz) { return n<0? (sz+1+n<0? -1 : sz+1+n) : (n>sz? sz : n); }


size_t aVector::insert(long n,value_t v,type_t t) {
	if((n=align_to_size(n,sz))<0) return 0;
	if(!list || sz==cap) resize(0);
	if(t==aCHAR_P) v = (value_t)strdup((char *)v);
#if _WORDSIZE < 64
	else if(t==aDOUBLE) {
		double *d = (double *)v;
		v = (value_t)malloc(8),*(double *)v = *d;
	}
#endif
	if((size_t)n<sz) for(size_t i=sz; (long)i>n; --i) list[i] = list[i-1];
	list[n] = (node){ v,t };
	return ++sz;
}

size_t aVector::insert(long n,const char **v) {
	size_t vsz;
	for(vsz=0; v[vsz]; ++vsz);
	if(vsz==0 || (n=align_to_size(n,sz))<0) return 0;
	if(!list || sz+vsz>cap) resize(sz+vsz);
	size_t i;
	node *l;
	if((size_t)n<sz) for(i=sz+vsz-1; i>=(size_t)n+vsz; --i) list[i] = list[i-vsz];
	for(i=0,sz+=vsz; i<vsz; ++i)
		l = &list[n+i],l->type = aCHAR_P,l->value = (value_t)strdup(v[i]);
	return sz;
}

size_t aVector::insert(long n,const aVector &v) {
	if((n=align_to_size(n,sz))<0) return 0;
	if(!list || sz+v.sz>cap) resize(sz+v.sz);
	size_t i;
	node *l;
	if((size_t)n<sz) for(i=sz+v.sz-1; i>=(size_t)n+v.sz; --i) list[i] = list[i-v.sz];
	for(i=0,sz+=v.sz; i<v.sz; ++i) {
		l = &list[n+i],*l = v.list[i];
		if(l->type==aCHAR_P) l->value = (value_t)strdup((char *)l->value);
#if _WORDSIZE < 64
		else if(l->type==aDOUBLE) {
			double *d = (double *)l->value;
			l->value = (value_t)malloc(8),*(double *)l->value = *d;
		}
#endif
	}
	return sz;
}

void aVector::set(long n,value_t v,type_t t) {
	if((n=align_to_size(n,sz))<0) return;
	if(!list || sz==cap) resize(0);
	if(t==aCHAR_P) v = (value_t)strdup((char *)v);
#if _WORDSIZE < 64
	else if(t==aDOUBLE) {
		double *d = (double *)v;
		v = (value_t)malloc(8),*(double *)v = *d;
	}
#endif
	if(n<(long)sz && (list[n].type==aCHAR_P
#if _WORDSIZE < 64
		|| list[n].type==aDOUBLE
#endif
		)) free((void *)list[n].value);
	list[n] = (node){ v,t };
	if(n==(long)sz) ++sz;
}

size_t aVector::remove(value_t v,type_t t) {
	size_t i,n = 0;
	if(v && sz) {
		for(i=0; i+n<sz; ++i) {
			while(list[i+n].value==v && (!t || list[i+n].type==t) && i+n<sz) {
#if _WORDSIZE < 64
				if(list[i+n].type==aDOUBLE) free((void *)list[i+n].value);
#endif
				n++;
			}
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return n;
}

size_t aVector::remove(const char *v) {
	size_t n = 0;
	if(v && sz) {
		for(size_t i=0; i+n<sz; ++i) {
			while(list[i+n].type==aCHAR_P && i+n<sz && !strcmp((char *)list[i+n].value,v)) {
				free((void *)list[i+n].value);
				n++;
			}
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return n;
}

size_t aVector::remove(const char **v) {
	size_t n = 0;
	if(v && *v && sz) {
		size_t i;
		for(i=0; v[i]; ++i) n += remove(v[i]);
		sz -= n;
	}
	return n;
}

void aVector::removeAt(long n) {
	if(sz) {
		if((n=align_to_size(n,sz))<0) return;
		if(list[n].type==aCHAR_P
#if _WORDSIZE < 64
			|| list[n].type==aDOUBLE
#endif
				) free((void *)list[n].value);
		for(size_t i=n+1; i<sz; ++i) list[i-1] = list[i];
		sz--;
	}
}

long aVector::find(value_t v,type_t t) {
	for(size_t i=0; i<sz; ++i) if(list[i].value==v && list[i].type==t) return (long)i;
	return -1;
}

long aVector::find(const char *v) {
	for(size_t i=0; i<sz; ++i)
      if(list[i].type==aCHAR_P && !strcmp((char *)list[i].value,v)) return (long)i;
	return -1;
}

long aVector::find(const char *arr[],const char *s,long o,long l) {
	long i,n;
	for(n=0; arr[n]; ++n);
	if(o<0) o = n+o;
	if(l<=0) l = n-o+l;
	if(o>=0 && o+l<=n)
		for(i=o,n=o+l; i<n; ++i)
   	   if(*arr[i]==*s && !strcmp(arr[i],s)) return i;
	return -1;
}

size_t aVector::split(const char *str,const char *delim,bool trim) {
	if(str && *str) {
		if(delim && *delim) {
			char *buf = strdup(str),*s1 = buf,*s2 = s1;
			int ld = strlen(delim);
			while(s2 && *s2) {
				while(*s2) {
					if(*s2==*delim && (ld==1 || !strncmp(s2,delim,ld))) { *s2 = '\0',s2 += ld;break; }
					else if(*s2=='\\') s2++;
					s2++;
				}
				if(trim) aString::trim(s1);
//debug_output("s1=\"%s\"\n",s1);
				if(*s1) insert(sz,(value_t)s1,aCHAR_P);
				s1 = s2;
			}
			free(buf);
		} else {
			insert(sz,(value_t)str,aCHAR_P);
		}
	}
	return sz;
}


void aVector::resize(size_t s) {
	if(s<sz) s = inc? sz+inc : (sz<<1);
	if(!s) s = cap>0? cap : 16;
	node *l = (node *)malloc(s*sizeof(node));
	if(list && sz>0) {
		memcpy(l,list,sz*sizeof(node));
		free(list);
	}
	cap = s;
	list = l;
}

void aVector::trim() {
	cap = sz;
	if(sz>0) {
		node *l = (node *)malloc(cap*sizeof(node));
		memcpy(l,list,cap*sizeof(node));
		free(list);
		list = l;
	}
}

void aVector::clear() {
	if(list) {
		for(size_t i=0; i<sz; ++i) if(list[i].type==aCHAR_P) free((void *)list[i].value);
		free(list);
	}
	sz = 0,cap = 16,list = 0;
}


size_t aVector::print(const char *fn) {
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

size_t aVector::print(FILE *fp) {
	if(!fp) return 0;
	for(size_t i=0; i<sz; ++i) {
		fprintf(fp,"List[%lu,%u]",(unsigned long)i,list[i].type);
		fflush(fp);
		if(list[i].value) {
			switch(list[i].type) {
				case aVOID:fprintf(fp,"=%p\n",(void *)list[i].value);break;
				case aINTPTR:fprintf(fp,"=%" PRIuPTR "\n",(intptr_t)list[i].value);break;
#if _WORDSIZE == 64
				case aDOUBLE:fprintf(fp,"=%f\n",*((double *)((void *)&list[i].value)));break;
#else
				case aFLOAT:fprintf(fp,"=%f\n",*((float *)((void *)&list[i].value)));break;
#endif
				case aCHAR_P:fprintf(fp,"=\"%s\"\n",(char *)list[i].value);break;
				case aOBJECT:fprintf(fp,"=%p\n",(void *)list[i].value);break;
			}
		}
		fflush(fp);
	}
	fprintf(fp,"Size=%lu,Capacity=%lu\n",(unsigned long)sz,(unsigned long)cap);
	fflush(fp);
	return sz;
}

size_t aVector::load(const char *fn,const char *l) {
	size_t s = 0;
	if(fn && *fn) {
		FILE *fp;
		if((fp=fopen(fn,"rb"))) {
			s = load(fp,l);
			fclose(fp);
		} else perror(fn);
	}
	return s;
}

size_t aVector::load(FILE *fp,const char *l) {
	if(!fp) return 0;
	aString line("",256);
	if(!l) l = aString::endl;
	for(size_t i=0; !feof(fp); ++i) {
//debug_output("aHashtable::load(readPair)\n");
		line.clear();
		line.appendUntil(fp,l);
		if(line) {
			insert(sz,line);
//debug_output("aHashtable::load(key='%s',val='%s')\n",key.array(),val.array());
		}
	}
	return sz;
}

size_t aVector::save(const char *fn,const char *l) {
	size_t s = 0;
	if(fn && *fn) {
		FILE *fp;
		if((fp=fopen(fn,"wb"))) {
			s = save(fp,l);
			fclose(fp);
		} else perror(fn);
	}
	return s;
}

size_t aVector::save(FILE *fp,const char *l) {
	if(!fp) return 0;
	if(!l) l = aString::endl;
	for(size_t i=0; i<sz; ++i) if(list[i].value) {
		switch(list[i].type) {
			case aVOID:fprintf(fp,"%p%s",(void *)list[i].value,l);break;
			case aINTPTR:fprintf(fp,"%" PRIuPTR "%s",(intptr_t)list[i].value,l);break;
#if _WORDSIZE == 64
			case aDOUBLE:fprintf(fp,"%f%s",*((double *)((void *)&list[i].value)),l);break;
#else
			case aFLOAT:fprintf(fp,"%f%s",*((float *)((void *)&list[i].value)),l);break;
#endif
			case aCHAR_P:fprintf(fp,"%s%s",(char *)list[i].value,l);break;
			case aOBJECT:fprintf(fp,"%p%s",(void *)list[i].value,l);break;
		}
	}
	return sz;
}



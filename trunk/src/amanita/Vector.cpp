
#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <amanita/Vector.h>



namespace a {


long search(const char *arr[],const char *s,long o,long l) {
	long i,n;
	for(n=0; arr[n]; ++n);
	if(o<0) o = n+o;
	if(l<=0) l = n-o+l;
	if(o>=0 && o+l<=n)
		for(i=o,n=o+l; i<n; ++i)
   	   if(*arr[i]==*s && !strcmp(arr[i],s)) return i;
	return -1;
}


value_t Vector::iterator::first(type_t type) {
	if(vec->sz) {
		if(!type) return vec->list[index=0].value;
		for(index=0; index<(long)vec->sz; index++)
			if(vec->list[index].type==type) return vec->list[index].value;
	}
	index = ITER_EMPTY;
	return 0;
}

value_t Vector::iterator::next(type_t type) {
	if(!vec->sz) index = ITER_EMPTY;
	else if(index!=ITER_AFTER_LAST) {
		if(index<0) index = ITER_BEFORE_FIRST;
		if(!type) {
			if(index<(long)vec->sz-1) return vec->list[++index].value;
		} else for(index++; index<(long)vec->sz; index++)
			if(vec->list[index].type==type) return vec->list[index].value;
		index = ITER_AFTER_LAST;
	}
	return 0;
}

value_t Vector::iterator::previous(type_t type) {
	if(!vec->sz) index = ITER_EMPTY;
	else if(index!=ITER_BEFORE_FIRST) {
		if(index<0) index = vec->sz;
		if(!type) {
			if(index>0) return vec->list[--index].value;
		} else for(index--; index>=0; index--)
			if(vec->list[index].type==type) return vec->list[index].value;
		index = ITER_BEFORE_FIRST;
	}
	return 0;
}

value_t Vector::iterator::last(type_t type) {
	if(vec->sz) {
		if(!type) return vec->list[index=vec->sz-1].value;
		for(index=vec->sz-1; index>=0; index--)
			if(vec->list[index].type==type) return vec->list[index].value;
	}
	index = ITER_EMPTY;
	return 0;
}

value_t Vector::iterator::remove() {
	if(index>=0 && index<(long)vec->sz) {
		value_t v = vec->list[index].value;
		vec->removeAt(index--);
		return v;
	}
	return 0;
}



Object_Inheritance(Vector,Collection)


Vector::Vector() : Collection(),inc(0),list(0) {}
Vector::Vector(size_t n) : Collection(),inc(0),list(0) { cap = n; }
Vector::~Vector() { clear(); }


Vector::iterator Vector::iterate() {
	Vector::iterator iter(this);
	return iter;
}

size_t Vector::insert(long n,value_t v,type_t t) {
	if((n=alignToSize(n))<0) return 0;
	if(!list || sz==cap) resize(0);
	if((size_t)n<sz) for(size_t i=sz; (long)i>n; --i) list[i] = list[i-1];
	list[n] = (node){ v,t };
	return ++sz;
}

size_t Vector::insert(long n,const char **v) {
	size_t vsz;
	for(vsz=0; v[vsz]; ++vsz);
	if(vsz==0 || (n=alignToSize(n))<0) return 0;
	if(!list || sz+vsz>cap) resize(sz+vsz);
	size_t i;
	node *l;
	if((size_t)n<sz) for(i=sz+vsz-1; i>=(size_t)n+vsz; --i) list[i] = list[i-vsz];
	for(i=0,sz+=vsz; i<vsz; ++i)
		l = &list[n+i],l->type = CHAR_P,l->value = (value_t)strdup(v[i]);
	return sz;
}

size_t Vector::insert(long n,const Vector &v) {
	if((n=alignToSize(n))<0) return 0;
	if(!list || sz+v.sz>cap) resize(sz+v.sz);
	size_t i;
	node *l;
	if((size_t)n<sz) for(i=sz+v.sz-1; i>=(size_t)n+v.sz; --i) list[i] = list[i-v.sz];
	for(i=0,sz+=v.sz; i<v.sz; ++i) {
		l = &list[n+i],*l = v.list[i];
		if(l->type==CHAR_P) l->value = (value_t)strdup((char *)l->value);
#if _WORDSIZE < 64
		else if(l->type==DOUBLE) l->value = (value_t)dbldup(*(double *)l->value);
#endif
	}
	return sz;
}

void Vector::set(long n,value_t v,type_t t) {
	if((n=alignToSize(n))<0) return;
	if(!list || sz==cap) resize(0);
	if(n<(long)sz && (list[n].type==CHAR_P
#if _WORDSIZE < 64
		|| list[n].type==DOUBLE
#endif
			)) free((void *)list[n].value);
	list[n] = (node){ v,t };
	if(n==(long)sz) ++sz;
}

size_t Vector::remove(value_t v,type_t t) {
	size_t i,n = 0;
	if(v && sz) {
		for(i=0; i+n<sz; ++i) {
			while(list[i+n].value==v && (!t || list[i+n].type==t) && i+n<sz) ++n;
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return n;
}

size_t Vector::remove(double v) {
#if _WORDSIZE == 64
	return remove(_VEC_DOUBLE_VALUE(v),DOUBLE);
#else
	size_t n = 0;
	if(v && sz) {
		for(size_t i=0; i+n<sz; ++i) {
			while(list[i+n].type==DOUBLE && i+n<sz && *(double *)list[i+n].value==v) {
				free((void *)list[i+n].value);
				++n;
			}
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return n;
#endif
}

size_t Vector::remove(const char *v) {
	size_t n = 0;
	if(v && sz) {
		for(size_t i=0; i+n<sz; ++i) {
			while(list[i+n].type==CHAR_P && i+n<sz && !strcmp((char *)list[i+n].value,v)) {
				free((void *)list[i+n].value);
				++n;
			}
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return n;
}

size_t Vector::remove(const char **v) {
	size_t n = 0;
	if(v && *v && sz) {
		size_t i;
		for(i=0; v[i]; ++i) n += remove(v[i]);
		sz -= n;
	}
	return n;
}

void Vector::removeAt(long n) {
	if(sz) {
		if((n=alignToSize(n))<0) return;
		if(list[n].type==CHAR_P
#if _WORDSIZE < 64
			|| list[n].type==DOUBLE
#endif
				) free((void *)list[n].value);
		for(size_t i=n+1; i<sz; ++i) list[i-1] = list[i];
		--sz;
	}
}

long Vector::find(value_t v,type_t t) {
	for(size_t i=0; i<sz; ++i) if(list[i].value==v && list[i].type==t) return (long)i;
	return -1;
}

long Vector::find(double v) {
#if _WORDSIZE == 64
	return find(_VEC_DOUBLE_VALUE(v),DOUBLE);
#else
	for(size_t i=0; i<sz; ++i)
      if(list[i].type==DOUBLE && *(double *)list[i].value==v) return (long)i;
	return -1;
#endif
}

long Vector::find(const char *v) {
	for(size_t i=0; i<sz; ++i)
      if(list[i].type==CHAR_P && !strcmp((char *)list[i].value,v)) return (long)i;
	return -1;
}

size_t Vector::split(const char *str,const char *delim,bool trim) {
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
				if(trim) a::trim(s1);
//debug_output("s1=\"%s\"\n",s1);
				if(*s1) insert(sz,(value_t)s1,CHAR_P);
				s1 = s2;
			}
			free(buf);
		} else {
			insert(sz,(value_t)str,CHAR_P);
		}
	}
	return sz;
}


void Vector::resize(size_t s) {
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

void Vector::trim() {
	cap = sz;
	if(sz>0) {
		node *l = (node *)malloc(cap*sizeof(node));
		memcpy(l,list,cap*sizeof(node));
		free(list);
		list = l;
	}
}

void Vector::clear() {
	if(list) {
		for(size_t i=0; i<sz; ++i)
			if(list[i].type==CHAR_P
#if _WORDSIZE < 64
				|| list[i].type==DOUBLE
#endif
					) free((void *)list[i].value);
		free(list);
	}
	sz = 0,cap = 16,list = 0;
}


size_t Vector::print(const char *fn) {
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

size_t Vector::print(FILE *fp) {
	if(!fp) return 0;
	for(size_t i=0; i<sz; ++i) {
		fprintf(fp,"Vector[%lu,%u]",(unsigned long)i,list[i].type);
		fflush(fp);
		if(list[i].value) {
			switch(list[i].type) {
				case VOID_P:fprintf(fp,"=%p\n",(void *)list[i].value);break;
				case INTPTR:fprintf(fp,"=%" PRIuPTR "\n",(intptr_t)list[i].value);break;
#if _WORDSIZE == 64
				case DOUBLE:fprintf(fp,"=%g\n",_VEC_DOUBLE_VALUE(list[i].value));break;
#else
				case FLOAT:fprintf(fp,"=%g\n",*(double *)list[i].value);break;
#endif
				case CHAR_P:fprintf(fp,"=\"%s\"\n",(char *)list[i].value);break;
				case OBJECT:fprintf(fp,"=%p\n",(void *)list[i].value);break;
			}
		}
		fflush(fp);
	}
	fprintf(fp,"Size=%lu,Capacity=%lu\n",(unsigned long)sz,(unsigned long)cap);
	fflush(fp);
	return sz;
}

size_t Vector::load(const char *fn,const char *l) {
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

size_t Vector::load(FILE *fp,const char *l) {
	if(!fp) return 0;
	String line("",256);
	if(!l) l = String::endl;
	for(size_t i=0; !feof(fp); ++i) {
//debug_output("Hashtable::load(readPair)\n");
		line.clear();
		line.appendUntil(fp,l);
		if(line) {
			insert(sz,line);
//debug_output("Hashtable::load(key='%s',val='%s')\n",key.array(),val.array());
		}
	}
	return sz;
}

size_t Vector::save(const char *fn,const char *l) {
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

size_t Vector::save(FILE *fp,const char *l) {
	if(!fp) return 0;
	if(!l) l = String::endl;
	for(size_t i=0; i<sz; ++i) if(list[i].value) {
		switch(list[i].type) {
			case VOID_P:fprintf(fp,"%p%s",(void *)list[i].value,l);break;
			case INTPTR:fprintf(fp,"%" PRIuPTR "%s",(intptr_t)list[i].value,l);break;
#if _WORDSIZE == 64
			case DOUBLE:fprintf(fp,"%g%s",_VEC_DOUBLE_VALUE(list[i].value),l);break;
#else
			case FLOAT:fprintf(fp,"%g%s",*(double *)list[i].value,l);break;
#endif
			case CHAR_P:fprintf(fp,"%s%s",(char *)list[i].value,l);break;
			case OBJECT:fprintf(fp,"%p%s",(void *)list[i].value,l);break;
		}
	}
	return sz;
}

}; /* namespace a */



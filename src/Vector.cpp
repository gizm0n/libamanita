
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libamanita/Vector.h>


value_t Vector::iterator::first(type_t type) {
	if(vec->sz) {
		if(!type) return vec->list[index=0l].value;
		for(index=0l; index<(long)vec->sz; index++)
			if(vec->list[index].type==type) return vec->list[index].value;
	}
	index = ITER_INDEX_EMPTY;
	return 0ul;
}

value_t Vector::iterator::next(type_t type) {
	if(!vec->sz) index = ITER_INDEX_EMPTY;
	else if(index!=ITER_INDEX_AFTER_LAST) {
		if(index<0l) index = ITER_INDEX_BEFORE_FIRST;
		if(!type) {
			if(index<(long)vec->sz-1l) return vec->list[++index].value;
		} else for(index++; index<(long)vec->sz; index++)
			if(vec->list[index].type==type) return vec->list[index].value;
		index = ITER_INDEX_AFTER_LAST;
	}
	return 0ul;
}

value_t Vector::iterator::previous(type_t type) {
	if(!vec->sz) index = ITER_INDEX_EMPTY;
	else if(index!=ITER_INDEX_BEFORE_FIRST) {
		if(index<0l) index = vec->sz;
		if(!type) {
			if(index>0l) return vec->list[--index].value;
		} else for(index--; index>=0l; index--)
			if(vec->list[index].type==type) return vec->list[index].value;
		index = ITER_INDEX_BEFORE_FIRST;
	}
	return 0ul;
}

value_t Vector::iterator::last(type_t type) {
	if(vec->sz) {
		if(!type) return vec->list[index=vec->sz-1l].value;
		for(index=vec->sz-1l; index>=0l; index--)
			if(vec->list[index].type==type) return vec->list[index].value;
	}
	index = ITER_INDEX_EMPTY;
	return 0ul;
}

value_t Vector::iterator::remove() {
	if(index>=0l && index<(long)vec->sz) {
		value_t v = vec->list[index].value;
		vec->removeAt(index--);
		return v;
	}
	return 0ul;
}



RttiObjectInheritance(Vector,Collection);


Vector::Vector() : Collection(),inc(0ul),list(0) {}
Vector::Vector(size_t n) : Collection(),inc(0ul),list(0) { cap = n; }
Vector::~Vector() { clear(); }


Vector::iterator Vector::iterate() {
	Vector::iterator iter(this);
	return iter;
}

Vector &Vector::insert(value_t v,long n,type_t t) {
	if(n<0l || n>(long)sz) n = sz;
	if(!list || sz==cap) resize(0ul);
	if(t==TYPE_CHAR_P) v = (value_t)strdup((char *)v);
	else if(t==TYPE_DOUBLE) {
		double *d = (double *)v;
		v = (value_t)malloc(8),*(double *)v = *d;
	}
	if((size_t)n<sz) for(size_t i=sz; (long)i>n; i--) list[i] = list[i-1];
	list[n] = (node){ v,t };
	sz++;
	return *this;
}

Vector &Vector::insert(Vector &v,long n) {
	if(n<0l || n>(long)sz) n = sz;
	if(!list || sz+v.sz>cap) resize(sz+v.sz);
	size_t i;
	node *l;
	if((size_t)n<sz) for(i=sz+v.sz-1ul; i>=(size_t)n+v.sz; i--) list[i] = list[i-v.sz];
	for(i=0ul; i<v.sz; i++,sz++) {
		l = &list[n+i],*l = v.list[i];
		if(l->type==TYPE_CHAR_P) l->value = (value_t)strdup((char *)l->value);
		else if(l->type==TYPE_DOUBLE) {
			double *d = (double *)l->value;
			l->value = (value_t)malloc(8),*(double *)l->value = *d;
		}
	}
	return *this;
}

Vector &Vector::remove(value_t v,type_t t) {
	if(v && sz) {
		size_t i,n;
		for(i=0ul,n=0ul; i+n<sz; i++) {
			while(list[i+n].value==v && (!t || list[i+n].type==t) && i+n<sz) {
				if(list[i+n].type==TYPE_DOUBLE) free((void *)list[i+n].value);
				n++;
			}
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return *this;
}

Vector &Vector::remove(const char *v) {
	if(v && *v && sz) {
		size_t i,n;
		for(i=0ul,n=0ul; i+n<sz; i++) {
			while(list[i+n].type==TYPE_CHAR_P && i+n<sz && !strcmp((char *)list[i+n].value,v)) {
				free((void *)list[i+n].value);
				n++;
			}
			if(n) list[i] = list[i+n];
		}
		sz -= n;
	}
	return *this;
}

Vector &Vector::removeAt(size_t n) {
	if(sz && n<sz) {
		if(list[n].type==TYPE_CHAR_P || list[n].type==TYPE_DOUBLE) free((void *)list[n].value);
		for(size_t i=n+1ul; i<sz; i++) list[i-1ul] = list[i];
		sz--;
	}
	return *this;
}

long Vector::indexOf(value_t v,type_t t) {
	for(size_t i=0ul; i<sz; i++) if(list[i].value==v && list[i].type==t) return (long)i;
	return -1;
}

long Vector::indexOf(const char *v) {
	for(size_t i=0ul; i<sz; i++)
      if(list[i].type==TYPE_CHAR_P && !strcmp((char *)list[i].value,v)) return (long)i;
	return -1;
}

Vector &Vector::split(const char *str,const char *delim,bool trim) {
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
				if(trim) String::trim(s1);
fprintf(stderr,"s1=\"%s\"\n",s1);
fflush(stderr);
				if(*s1) insert((value_t)s1,sz,TYPE_CHAR_P);
				s1 = s2;
			}
			free(buf);
		} else {
			insert((value_t)str,sz,TYPE_CHAR_P);
		}
	}
	return *this;
}


void Vector::resize(size_t s) {
	if(s<sz) s = inc? sz+inc : (sz<<1);
	if(!s) s = cap>0ul? cap : 16ul;
	node *l = (node *)malloc(s*sizeof(node));
	if(list && sz>0ul) {
		memcpy(l,list,sz*sizeof(node));
		free(list);
	}
	cap = s;
	list = l;
}

void Vector::trim() {
	cap = sz;
	if(sz>0ul) {
		node *l = (node *)malloc(cap*sizeof(node));
		memcpy(l,list,cap*sizeof(node));
		free(list);
		list = l;
	}
}

void Vector::clear() {
	if(list) {
		for(size_t i=0ul; i<sz; i++) if(list[i].type==TYPE_CHAR_P) free((void *)list[i].value);
		free(list);
	}
	sz = 0ul,cap = 16ul,list = 0;
}


size_t Vector::print(const char *fn) {
	FILE *fp;
	if(!fn || !*fn || !(fp=fopen(fn,"w"))) return 0ul;
	size_t s = print(fp);
	fclose(fp);
	return s;
}

size_t Vector::print(FILE *fp) {
	if(!fp) return 0ul;
	for(size_t i=0ul; i<sz; i++) {
		fprintf(fp,"List[%d,%u]",i,list[i].type);
		fflush(fp);
		if(list[i].value) {
			switch(list[i].type) {
				case TYPE_VOID_P:fprintf(fp,"=%p",(void *)list[i].value);break;
				case TYPE_INT32:fprintf(fp,"=%ld",(long)list[i].value);break;
				case TYPE_UINT32:fprintf(fp,"=%lu",(unsigned long)list[i].value);break;
				case TYPE_FLOAT:fprintf(fp,"=%f",*((float *)((void *)&list[i].value)));break;
				case TYPE_CHAR_P:fprintf(fp,"=\"%s\"",(char *)list[i].value);break;
				case TYPE_OBJECT_P:fprintf(fp,"=%p",(void *)list[i].value);break;
			}
		}
		fprintf(fp,"\n");
		fflush(fp);
	}
	fprintf(fp,"Size=%d,Capacity=%d\n",sz,cap);
	fflush(fp);
	return sz;
}

size_t Vector::load(const char *fn) {
	FILE *fp;
	if(!fn || !*fn || !(fp=fopen(fn,"r"))) return 0ul;
	size_t s = load(fp);
	fclose(fp);
	return s;
}

size_t Vector::load(FILE *fp) {
	if(!fp) return 0ul;
	String line("",256);
	for(size_t i=0ul; !feof(fp); i++) {
//fprintf(stderr,"Hashtable::load(readPair)\n");
		line.clear().appendUntil(fp,"\r\n");
		if(line.length()>0) {
			insert(line,sz);
//fprintf(stderr,"Hashtable::load(key='%s',val='%s')\n",key.array(),val.array());
//fflush(stderr);
		}
	}
	return sz;
}

size_t Vector::save(const char *fn) {
	FILE *fp;
	if(!fn || !*fn || !(fp=fopen(fn,"w"))) return 0ul;
	size_t s = save(fp);
	fclose(fp);
	return s;
}

size_t Vector::save(FILE *fp) {
	if(!fp) return 0ul;
	for(size_t i=0ul; i<sz; i++) if(list[i].value) {
		switch(list[i].type) {
			case TYPE_VOID_P:fprintf(fp,"%p\n",(void *)list[i].value);break;
			case TYPE_INT32:fprintf(fp,"%ld\n",(long)list[i].value);break;
			case TYPE_UINT32:fprintf(fp,"%lu\n",(unsigned long)list[i].value);break;
			case TYPE_FLOAT:fprintf(fp,"%f\n",*((float *)((void *)&list[i].value)));break;
			case TYPE_CHAR_P:fprintf(fp,"%s\n",(char *)list[i].value);break;
			case TYPE_OBJECT_P:fprintf(fp,"%p\n",(void *)list[i].value);break;
		}
	}
	return sz;
}



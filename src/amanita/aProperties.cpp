
#include "_config.h"
#include <stdio.h>
#include <inttypes.h>
#include <amanita/aProperties.h>
#include <amanita/aString.h>


static const char *key_escape_chars = ";#=:";


aObject_Inheritance(aProperties,aCollection)

aProperties::aProperties(style_t st) : aCollection(),first(0),last(0),table(0),full(0),style(st) { cap = 11; }

aProperties::~aProperties() {
	node *n1 = first,*n2;
	for(; n1; n1=n2) {
		n2 = n1->list;
		delete n1;
	}
	if(table) free(table);
	first = last = 0,table = 0,cap = sz = 0;
}

aProperties::node::node(const char *k,value_t v,type_t vt,const char *i,const char *d,char q,const char *c) : v_type(vt),list(0),table(0) {
	key = k? strdup(k) : 0;
	hash = 0;
	if(v_type==aCHAR_P) value = (value_t)strdup((char *)v);
	else if(v_type==aDOUBLE) {
		value = (value_t)malloc(sizeof(double));
		*((double *)value) = *((double *)v);
	} else value = v;
	ind = i? strdup(i) : 0;
	delim = d? strdup(d) : 0;
	quot = q;
	comment = c? strdup(c) : 0;
}

aProperties::node::~node() {
	if(key) free(key);
	if(value) {
		if(v_type==aCHAR_P) free((char *)value);
		else if(v_type==aDOUBLE) free((double *)value);
	}
	if(ind) free(ind);
	if(delim) free(delim);
	if(comment) free(comment);
}

void aProperties::getKey(aString &s,const char *section,const char *key) {
	long l = 0;
	if(section && *section) {
		s = section;
		s.escape(0,0,key_escape_chars,aESCAPE_HEX);
		s << '#',l = s.length(),s << key;
	} else s = key;
	s.escape(l,0,key_escape_chars,aESCAPE_HEX);
}

void aProperties::rehash(style_t st) {
	if(!table) {
		sz = 0;
		table = (node **)malloc(cap*sizeof(node *));
		memset(table,0,cap*sizeof(node *));
	} else {
		size_t i,c = (size_t)(cap*2);
		hash_t h;
		if((c&1)==0) c++;
		node **t1 = table;
		table = (node **)malloc(c*sizeof(node *));
		memset(table,0,c*sizeof(node *));

		node *n1,*n2;
		if(st!=style) {
			for(i=0; i<cap; ++i) if((n1=t1[i])) while(n1) {
				n1->hash = aString::crc32((char *)n1->key,style&aHASH_STYLE_CASE_INSENSITIVE);
				n2 = n1->table,h = n1->hash%c,n1->table = table[h],table[h] = n1,n1 = n2;
			}
		} else  for(i=0; i<cap; ++i) if((n1=t1[i])) while(n1)
			n2 = n1->table,h = n1->hash%c,n1->table = table[h],table[h] = n1,n1 = n2;

		free(t1);
		cap = c;
	}
	style = st;
	full = (size_t)(cap*3/4);
//printTable(stderr);
debug_output("rehash: st=%d, sz=%d, full=%d, cap=%d\n",(int)st,(int)sz,(int)full,(int)cap);
}

void aProperties::addNode(node *n) {
	if(!first) first = n;
	else last->list = n;
	last = n;
	if(n->key) putNode(n);
}

void aProperties::putNode(node *n) {
/*	if(sz && !(style&aHASH_STYLE_KEY_MULTIPLES)) {
		node *n1 = table[aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE)%cap];
		if(style&aHASH_STYLE_CASE_INSENSITIVE) while(n1) {
			if(!aString::stricmp(n1->key,n->key)) break;
			n1 = n1->table;
		} else while(n1) {
			if(!strcmp(n1->key,n->key)) break;
			n1 = n1->table;
		}
		if(n1) {
			node n2;
			n2 = *n1,*n1 = *n,*n = n2;
			delete n;
			return;
		}
	}*/
//debug_output("putNode: %s\n",n->key);
	if(sz==full) rehash(style);
	n->hash = aString::crc32(n->key,style&aHASH_STYLE_CASE_INSENSITIVE);
	hash_t h = n->hash%cap;
	n->table = table[h],table[h] = n;
	++sz;
debug_output("put: \"%s\"[%d,%08x] => table[\"%s\"]\n",n->key,(int)h,n->hash,table[h] && table[h]->table? table[h]->table->key : "empty");
printTable(stderr);
}

value_t aProperties::get(const char *key,type_t &type) {
	type = aEMPTY;
	if(!sz) return 0;
	hash_t h = aString::crc32(key,style&aHASH_STYLE_CASE_INSENSITIVE);
	node *n = table[h%cap];
	if(style&aHASH_STYLE_CASE_INSENSITIVE) while(n) {
		if(n->hash==h && !aString::stricmp(n->key,key)) break;
		n = n->table;
	} else while(n) {
		if(n->hash==h && !strcmp((char *)n->key,key)) break;
		n = n->table;
	}
	if(!n) return 0;
	type = n->v_type;
	return n->value;
}

value_t aProperties::get(const char *section,const char *key,type_t &type) {
	aString s;
	getKey(s,section,key);
//debug_output("get: %s\n",(const char *)s);
	return get((const char *)s,type);
}

size_t aProperties::print(const char *fn) {
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

size_t aProperties::print(FILE *fp) {
	if(!fp) return 0;
	size_t i;
	node *n;
	const char *p;
	aString s;
	for(n=first,i=1; n; n=n->list,++i) {
//		fprintf(fp,"[%d]",(int)i);
		if(n->ind) {
//fputs("{ind:",fp);
			fputs(n->ind,fp);
//fputc('}',fp);
		}
		if(/*n->v_type==aHASHTABLE*/n->key && !n->value) {
			s = n->key;
			s.unescape();
			fprintf(fp,"[%s]",(const char *)s);
			if(n->comment) fputs(n->comment,fp);
			fputs(aSTRING_ENDL,fp);
			//((aProperties *)n->value)->print(fp);
		} else {
			if(n->key) {
//				fputs((p=strchr(n->key,'~'))? p+1 : n->key,fp);
//fputs("{key:",fp);
//fprintf(fp,"[%s]",n->key);
				if((p=strchr(n->key,'#'))) s = p+1;
				else s = n->key;
				s.unescape();
				s.print();
				/*if((p=strchr(n->key,'\n'))) ++p;
				else p = n->key;*/
				/*for(p=s; *p; ++p)
					if(*p&0x80) fprintf(fp,"%02X",(unsigned int)(unsigned char)*p);
					else fputc(*p,fp);*/
//fputs("}{delim:",fp);
				fputs(n->delim? n->delim : "=",fp);
//fputs("}{value:",fp);
				if(n->quot) fputc(n->quot,fp);
				switch(n->v_type) {
					case aVOID:fprintf(fp,"%p",(void *)n->value);break;
					case aINTPTR:fprintf(fp,"%" PRIdPTR,(intptr_t)n->value);break;
#if _WORDSIZE == 64
					case aDOUBLE:fprintf(fp,"%f",*((double *)((void *)&n->value)));break;
#else
					//case aINT64:fprintf(fp,"%lu",(unsigned long)n->value);break;
					case aFLOAT:fprintf(fp,"%f",*((float *)((void *)&n->value)));break;
					//case aDOUBLE:fprintf(fp,"%f",*((double *)((void *)&n->value)));break;
#endif
					case aCHAR_P:
						//fprintf(fp,"%s",(char *)n->value);
						s = (char *)n->value;
						s.escape(0,0,";#=:",(n->quot? aESCAPE_QUOTE : 0)|aESCAPE_SL_EOL);
						s.print();
						break;
				}
				if(n->quot) fputc(n->quot,fp);
//fputc('}',fp);
			}
			if(n->comment) {
//fputs("{comment:",fp);
				fputs(n->comment,fp);
//fputc('}',fp);
			}
			if(n->v_type!=aEMPTY) fputs(aSTRING_ENDL,fp);
		}
		fflush(fp);
	}
//	fprintf(fp,"Size=%lu,Full=%lu,Capacity=%lu" aSTRING_ENDL,(unsigned long)sz,(unsigned long)full,(unsigned long)cap);
//	fflush(fp);
	return sz;
}

size_t aProperties::printTable(FILE *fp) {
	if(!fp) return 0;
	size_t i,s = sz;
	node *n;
	for(i=0; i<cap; ++i) {
		fprintf(fp,"Table[%lu]",(unsigned long)i);
		if((n=table[i])) while(n) {
			fprintf(fp," \"%s\"[%d,%08x]",(char *)n->key,(int)(n->hash%cap),n->hash);
			if(n->table) --s;
			n = n->table;
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"Size=%lu, Full=%lu, Capacity=%lu, Spread=%.3f\n",(unsigned long)sz,(unsigned long)full,(unsigned long)cap,100.0f*(sz? (float)s/(float)sz : 1.0));
	fflush(fp);
	return sz;
}

size_t aProperties::load(aString &data) {
	if(!data) return 0;
	long i,j,k,l;
	char c,q;
	aString sec,ind,key,delim,value,comment;
	size_t n = 0;
	aString s;
	for(i=0,l=data.length(); i<l; ++i) {
		for(j=i; j<l && strchr(aSTRING_WHITESPACE ";#",c=data[j]); ) {
			if(c!=';' && c!='#') data.ltrim(j);
			if((c=data[j])!=';' && c!='#') break;
//data.substr(s,j,20);
//debug_output("Comment1: {%s}\n",(const char *)s);
			j = data.skipComments(aLANG_CFG,j,l-j);
		}
		if(j>i) {
			data.luntrim(j,0," \t");
			if(j>i) {
//data.substr(s,i,j-i);
//debug_output("Comment2[%d,%d,%d]: {%s}\n",(int)i,(int)j,(int)(j-i),(const char *)s);
				comment.append(data,i,j-i);
//debug_output("\ncomment: [%s]\n\n",(const char *)comment);
				addNode(new node(0,0,aEMPTY,0,0,0,comment));
				comment.clear();
				i = j;
			}
		}
		if(i<l) {
			j = i;
			data.ltrim(j);
			if(j!=i) {
				ind.append(data,i,j-i);
//debug_output("ind: [%s]\n",(const char *)ind);
				i = j;
			}
			c = data[i];
			if(c=='[') {
				++i,j = data.findChar("]\n",i);
				if(j==-1 || data[j]=='\n') break;
				key.append(data,i,j-i);
				key.escape(0,0,key_escape_chars,aESCAPE_HEX);
				sec = key;
				sec << '#';
				i = j+1,c = data[i];
				if(c!='\n') {
					if(c==';' || c=='#' || aString::isSpace(c)) {
						if(aString::isSpace(c)) {
							j = 1;
							data.runtrim(i,j," \t");
							j = i+j;
						} else j = i;
						c = data[j];
						if(c==';' || c=='#') {
							j = data.find('\n',j);
							if(j==-1) j = l;
						}
						comment.append(data,i,j-i);
						i = j;
					} else break;
				}
				addNode(new node(key,0,aEMPTY,
						(ind? (const char *)ind : 0),0,0,
						(comment? (const char *)comment : 0)));
				++n;
			} else {
				j = data.findChar("=:\n",i);
				if(j==-1 || data[j]=='\n') break;
				k = 1;
				data.untrim(j,k," \t");
				if(j==i) break;
				if(!data.equals("=",j,k)) {
					delim.append(data,j,k);
//debug_output("delim: [%s]\n",(const char *)delim);
				}
				key = sec;
				key.append(data,i,j-i);
				key.escape((long)sec.length(),0,key_escape_chars,aESCAPE_HEX);
				//if(sec) key.insert(0,sec);
//debug_output("key: [%s] i: %ld, j: %ld, k: %ld\n",(const char *)key,i,j,k);
				i = j+k;
				if(i>=l) break;
				j = data.matchValue(aLANG_CFG,i);
				if(j==i) break;
				k = j;
				if(aString::isQuote(data[i]) && data[i]==data[j-1]) q = data[i],++i,--k;
				if(k>i) {
					value.append(data,i,k-i);
					value.unescape();
				}
//debug_output("value: [%s] i: %ld, j: %ld, k: %ld\n",(const char *)value,i,j,k);
				i = j;
				if(data[i]!='\n') {
					j = data.find('\n',i);
					if(j==-1) j = l;
					comment.append(data,i,j-i);
//debug_output("comment: [%s]\n",(const char *)comment);
					i = j;
				}
//debug_output("add[%s%s%s%s%s]\n",(const char *)ind,(const char *)key,(delim? (const char *)delim : 0),(const char *)value,(const char *)comment);
				addNode(new node(key,(value_t)(const char *)value,aCHAR_P,
						(ind? (const char *)ind : 0),
						(delim? (const char *)delim : 0),q,
						(comment? (const char *)comment : 0)));
				++n;
			}
			ind.clear();
			key.clear();
			delim.clear();
			value.clear();
			q = 0;
			comment.clear();
		}
	}
	return n;
}

size_t aProperties::load(const char *fn) {
	if(fn && *fn) {
		aString data;
		data.include(fn);
		if(data) {
//const char *p = data;
//debug_output("aProperties[%s]:\n",fn);
//for(; *p; ++p)
//if(*p&0x80) debug_output("%02X",(unsigned int)(unsigned char)*p);
//else debug_putc(*p);
			return load(data);
		}
	}
	return 0;
}

size_t aProperties::save(const char *fn) {
	size_t s = 0;
	if(fn && *fn) {
		FILE *fp;
		if((fp=fopen(fn,"wb"))) {
			s = save(fp);
			fclose(fp);
		} else perror(fn);
	}
	return s;
}

size_t aProperties::save(FILE *fp) {
	return 0;
/*	if(!fp) return 0;
	node *n;
	char *d;
	size_t i = 0;
	for(n=next,i; n; n=n->next) {
		if(n->key && n->value) {
			d = n->delim? n->delim = "=";
			switch(n->v_type) {
				case aINTPTR:fprintf(fp,"%s%s%" PRIdPTR,n->key,d,(intptr_t)n->value);++i;break;
#if _WORDSIZE == 64
				case aDOUBLE:fprintf(fp,"%s%s%f",n->key,d,*((double *)((void *)&n->key)));++i;break;
#else
				//case aUINT32:fprintf(fp,"%s%s%lu",n->key,d,(unsigned long)n->value);++i;break;
				case aFLOAT:fprintf(fp,"%s%s%f",n->key,d,*((float *)((void *)&n->key)));++i;break;
				//case aDOUBLE:fprintf(fp,"%s%s%f",n->key,d,*((double *)((void *)&n->key)));++i;break;
#endif
				case aCHAR_P:fprintf(fp,"%s%s%s",n->key,d,(char *)n->value);++i;break;
				case aHASHTABLE:
					fprintf(fp,"[%s]" aSTRING_ENDL,n->key);
					i += ((aProperties *)n->value)->save(fp);
					break;
			}
		}
		if(n->comment) fputs(n->comment,fp);
		fputs(aSTRING_ENDL,fp);
	}
	return i;*/
}




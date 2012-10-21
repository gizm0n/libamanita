
#include "_config.h"
#include <stdio.h>
#include <inttypes.h>
#include <amanita/Properties.h>
#include <amanita/String.h>



static const char *comment_chars[] = { ";#","#!" };
static const char *key_escape_chars[] = { ";#=:","#!=: " };
static const char *key_delim_chars[] = { "=:\n","=: \t\n" };


namespace a {


Object_Inheritance(Properties,Collection)

Properties::Properties(int l,style_t st) : Collection(),first(0),last(0),table(0),full(0),lang(l),style(st) {
	cap = 11;
	if(lang!=LANG_CFG && lang!=LANG_INI && lang!=LANG_PROPERTIES) lang = LANG_CFG;
	if(lang==LANG_PROPERTIES)
		style &= ~(PROP_STYLE_SECTIONS|PROP_STYLE_VALUE_COMMENTS|PROP_STYLE_KEY_INDENT);
	cc = comment_chars[lang==LANG_PROPERTIES? 1 : 0];
	kec = key_escape_chars[lang==LANG_PROPERTIES? 1 : 0];
	kdc = key_delim_chars[lang==LANG_PROPERTIES? 1 : 0];
}

Properties::~Properties() {
	node *n1 = first,*n2;
	for(; n1; n1=n2) {
		n2 = n1->list;
		delete n1;
	}
	if(table) free(table);
	first = last = 0,table = 0,cap = sz = 0;
}

Properties::node::node(const char *k,const char *v,const char *i,const char *d,char q,const char *c) : list(0),table(0) {
	key = k && *k? strdup(k) : 0;
	hash = 0;
	setValue(v);
	ind = i && *i? strdup(i) : 0;
	delim = d && *d? strdup(d) : 0;
	quot = q;
	comment = c && *c? strdup(c) : 0;
}

Properties::node::~node() {
	if(key) free(key);
	if(value) {
		if(v_type==CHAR_P) free((char *)value);
#if __WORDSIZE < 64
		else if(v_type==DOUBLE) free((double *)value);
#endif
	}
	if(ind) free(ind);
	if(delim) free(delim);
	if(comment) free(comment);
}

void Properties::node::setValue(const char *v) {
	if(!v) value = 0,v_type = EMPTY;
	else {
		const char *p;
		int n;
		for(p=v,n=0; *p; ++p)
			if((*p<'0' || *p>'9') && *p!='.') break;
			else if(*p=='.') ++n;
		if(*p) value = (value_t)strdup(v),v_type = CHAR_P;
#if __WORDSIZE < 64
		else if(n>0) {
			value = (value_t)malloc(sizeof(double));
			*((double *)value) = atof(v),v_type = DOUBLE;
		}
#else
		else if(n>0) {
			double d = atof(v);
			value = *(value_t *)((void *)&d),v_type = DOUBLE;
//debug_output("double value: %lf[%lf]\n",*(double *)&value,atof(v));
		}
#endif
		else value = (value_t)atol(v),v_type = INTPTR;
	}
}

void Properties::getKey(String &s,const char *section,const char *key) {
	long l = 0;
	if(section && *section) {
		s = section;
		s.escape(0,0,kec,ESCAPE_HEX);
		if(!key || !*key) return;
		s << '#',l = s.length(),s << key;
	} else if(!key || !*key) return;
	else s = key;
	s.escape(l,0,kec,ESCAPE_HEX);
//debug_output("key: %s\n",(const char *)s);
}

void Properties::rehash(style_t st) {
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
				n1->hash = crc32((char *)n1->key,style&PROP_STYLE_CASE_INSENSITIVE);
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
//debug_output("rehash: st=%d, sz=%d, full=%d, cap=%d\n",(int)st,(int)sz,(int)full,(int)cap);
}

void Properties::addNode(node *n) {
	if(!first) first = n;
	else last->list = n;
	last = n;
	if(n->key) putNode(n);
}

void Properties::putNode(node *n) {
/*	if(sz && !(style&HASH_STYLE_KEY_MULTIPLES)) {
		node *n1 = table[crc32(key,style&PROP_STYLE_CASE_INSENSITIVE)%cap];
		if(style&HASH_STYLE_CASE_INSENSITIVE) for(; n1; n1=n1->table)
			if(n->hash==n1->hash && !stricmp(n1->key,n->key)) break;
		else for(; n1; n1=n1->table)
			if(n->hash==n1->hash && !strcmp(n1->key,n->key)) break;
		if(n1) {
			node n2;
			n2 = *n1,*n1 = *n,*n = n2;
			delete n;
			return;
		}
	}*/
//debug_output("putNode: %s\n",n->key);
	if(sz==full) rehash(style);
	n->hash = crc32(n->key,style&PROP_STYLE_CASE_INSENSITIVE);
	hash_t c = n->hash%cap;
	n->table = table[c],table[c] = n;
	++sz;
//debug_output("put: \"%s\"[%d,%08x] => table[\"%s\"]\n",n->key,(int)c,n->hash,table[h] && table[h]->table? table[h]->table->key : "empty");
//printTable(stderr);
}

Properties::node *Properties::removeNode(const char *key) {
	if(!sz) return 0;
	hash_t h = crc32(key,style&PROP_STYLE_CASE_INSENSITIVE);
	int c = h%cap;
	node *n = table[c],*n0 = 0;
	if(style&PROP_STYLE_CASE_INSENSITIVE) for(; n; n0=n,n=n->table) {
		if(n->hash==h && !stricmp(n->key,key)) break;
	} else for(; n; n0=n,n=n->table)
		if(n->hash==h && !strcmp((char *)n->key,key)) break;
	if(n) {
		if(n0) n0->table = n->table;
		else table[c] = n->table;
	}
	return n;
}

Properties::node *Properties::getNode(const char *key) {
	if(!sz) return 0;
	hash_t h = crc32(key,style&PROP_STYLE_CASE_INSENSITIVE);
	node *n = table[h%cap];
	if(style&PROP_STYLE_CASE_INSENSITIVE) for(; n; n=n->table) {
		if(n->hash==h && !stricmp(n->key,key)) break;
	} else for(; n; n=n->table)
		if(n->hash==h && !strcmp((char *)n->key,key)) break;
	return n;
}

void Properties::set(const char *section,const char *key,const char *value) {
	if(key) {
		String s;
		node *n,*n1 = 0,*n0 = 0;
		if(!(style&PROP_STYLE_SECTIONS)) section = 0;
		getKey(s,section,key);
		n = getNode((const char *)s);
		if(n) {
			if(n->v_type==CHAR_P) free((char *)n->value);
#if __WORDSIZE < 64
			else if(n->v_type==DOUBLE) free((double *)n->value);
#endif
			if(value) n->value = (value_t)strdup(value),n->v_type = CHAR_P;
			else n->value = 0,n->v_type = EMPTY;
			return;
		}
		n = new node((const char *)s,value,0,0,0,0);
		if(section) {
			getKey(s,section,0);
			n1 = getNode((const char *)s);
			if(!n1) n1 = last;
			else for(; n1->list; n0=n1,n1=n1->list)
				if(n1->list->key && n1->list->v_type==EMPTY) break; // Find last key-value-pair in section.
		} else if(first && ((first->key && first->v_type!=EMPTY) ||
					(!first->key && first->comment)))
			for(n1=first; n1->list; n0=n1,n1=n1->list)
				if(n1->list->key && n1->list->v_type==EMPTY) break; // Find last key-value-pair before first section.
		if(n1) {
			if(!n1->key && !n1->value && n1->comment && n0) n1 = n0;
			if(n1==last) {
				if(section) addNode(new node((const char *)s,0,0,0,0,0));
				n1 = last,last = n;
			}
			n->list = n1->list,n1->list = n;
		} else n->list = first,first = n;
		putNode(n);
//debug_output("set: \"%s\"[%08x] => list[\"%s\"]\n",n->key,n->hash,n->list? n->list->key : "empty");
//printTable(stderr);
	}
}

void Properties::remove(const char *section,const char *key) {
	if(key) {
		String s;
		node *n,*n1 = 0;
		if(!(style&PROP_STYLE_SECTIONS)) section = 0;
		getKey(s,section,key);
		n = removeNode((const char *)s);
		if(!n) return;
		if(section) {
			getKey(s,section,0);
			n1 = getNode((const char *)s);
			if(n1) for(; n1->list && n1->list!=n; n1=n1->list);
		} else if(first==n) n1 = first;
		else for(n1=first; n1->list && n1->list!=n; n1=n1->list);
		if(n1==first) first = n->list;
		else n1->list = n->list;
		delete n;
	}
}

value_t Properties::get(const char *key,type_t &type) {
//debug_output("get: %s\n",key);
	node *n = getNode(key);
	type = EMPTY;
	if(!n) return 0;
	type = n->v_type;
	return n->value;
}

value_t Properties::get(const char *section,const char *key,type_t &type) {
	String s;
	if(!(style&PROP_STYLE_SECTIONS)) section = 0;
	getKey(s,section,key);
	return get((const char *)s,type);
}

size_t Properties::print(const char *fn) {
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

size_t Properties::print(FILE *fp) {
	if(!fp) return 0;
	size_t i;
	node *n;
	const char *p;
	String s;
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
			fputs(String::endl,fp);
			//((Properties *)n->value)->print(fp);
		} else {
			if(n->key) {
//				fputs((p=strchr(n->key,'~'))? p+1 : n->key,fp);
//fputs("{key:",fp);
//fprintf(fp,"[%s]",n->key);
				if((style&PROP_STYLE_SECTIONS) && (p=strchr(n->key,'#'))) s = p+1;
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
					case VOID_P:fprintf(fp,"%p",(void *)n->value);break;
					case INTPTR:fprintf(fp,"%" PRIdPTR,(intptr_t)n->value);break;
#if __WORDSIZE == 64
					case DOUBLE:fprintf(fp,"%lf",*((double *)((void *)&n->value)));break;
#else
					//case INT64:fprintf(fp,"%lu",(unsigned long)n->value);break;
//					case FLOAT:fprintf(fp,"%f",*((float *)((void *)&n->value)));break;
					case DOUBLE:fprintf(fp,"%lf",*((double *)((void *)n->value)));break;
#endif
					case CHAR_P:
						//fprintf(fp,"%s",(char *)n->value);
						s = (char *)n->value;
						s.escape(0,0,0,(n->quot? ESCAPE_QUOTE : 0)|ESCAPE_SL_EOL);
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
			if(n->v_type!=EMPTY) fputs(String::endl,fp);
		}
		fflush(fp);
	}
//	fprintf(fp,"Size=%lu,Full=%lu,Capacity=%lu" STRING_ENDL,(unsigned long)sz,(unsigned long)full,(unsigned long)cap);
//	fflush(fp);
	return sz;
}

size_t Properties::printTable(FILE *fp) {
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

size_t Properties::load(String &data) {
	if(!data) return 0;
	long i,j,k,l;
	char c,q;
	String sec,ind,key,delim,value,comment;
	size_t n = 0;
	String s;
//debug_output("Properties::load(lang[%d],style[%x],kec[%s],kdc[%s])\n%s\n",lang,style,kec,kdc,(const char *)data);
	for(i=0,l=data.length(); i<l; ++i) {
		for(j=i; j<l; ) {
			if(isspace(data[j])) j = data.right(j);
			if(!strchr(cc,data[j])) break;
//data.substr(s,j,20);
//debug_output("Comment1: {%s}\n",(const char *)s);
			j = data.skipComments(lang,j,l-j);
		}
		if(j>i) {
			j = data.left(j," \t");
			if(j>i && (style&PROP_STYLE_STORE_COMMENTS)) {
//data.substr(s,i,j-i);
//debug_output("Comment2[%d,%d,%d]: {%s}\n",(int)i,(int)j,(int)(j-i),(const char *)s);
				comment.append(data,i,j-i);
//debug_output("\ncomment: [%s]\n\n",(const char *)comment);
				addNode(new node(0,0,0,0,0,comment));
				comment.clear();
			}
			i = j;
		}
		if(i<l) {
			if((j=data.right(i))!=i) {
				if(style&PROP_STYLE_KEY_INDENT) ind.append(data,i,j-i);
//debug_output("ind: [%s]\n",(const char *)ind);
				i = j;
			}
			c = data[i];
			if(c=='[' && (style&PROP_STYLE_SECTIONS)) {
				++i,j = data.matchToken("]\n",i,0,TOKEN_ESCAPE);//findChar("]\n",i);
				if(j==-1 || data[j]=='\n') break;
				key.append(data,i,j-i);
				key.escape(0,0,kec,ESCAPE_HEX);
				sec = key;
				sec << '#';
//debug_output("section: [%s]\n\n",(const char *)key);
				i = j+1,c = data[i];
				if(c!='\n') {
					if(c==';' || c=='#' || isspace(c)) {
						if(isspace(c)) j = data.right(i," \t");
						else j = i;
						c = data[j];
						if(c==';' || c=='#') {
							j = data.find('\n',j);
							if(j==-1) j = l;
						}
						comment.append(data,i,j-i);
						i = j;
					} else break;
				}
				addNode(new node(key,0,
						(ind? (const char *)ind : 0),0,0,
						(comment? (const char *)comment : 0)));
				++n;
			} else {
//				j = data.findChar("=:\n",i);
				j = data.matchToken(kdc,i,0,TOKEN_TRIM|TOKEN_ESCAPE);
				if(j==-1 || data[j]=='\n') break;
				k = 1;
				data.expand(j,k," \t");
				if(j==i) break;
				if((style&PROP_STYLE_STORE_DELIM) && !data.equals("=",j,k)) {
					delim.append(data,j,k);
//debug_output("delim: [%s]\n",(const char *)delim);
				}
				key = sec;
				key.append(data,i,j-i);
				key.escape((long)sec.length(),0,kec,ESCAPE_HEX);
				//if(sec) key.insert(0,sec);
//debug_output("key: [%s] i: %ld, j: %ld, k: %ld\n",(const char *)key,i,j,k);
				i = j+k;
				if(i>=l) break;
				j = data.matchValue(lang,i,0,(style&PROP_STYLE_VALUE_COMMENTS)? 0 : MATCH_COMMENTS);
				if(j==i) break;
				k = j;
				if(isquote(data[i]) && data[i]==data[j-1]) q = data[i],++i,--k;
				if(k>i) {
					value.append(data,i,k-i);
					value.unescape();
				}
//debug_output("value: [%s] i: %ld, j: %ld, k: %ld\n",(const char *)value,i,j,k);
				i = j;
				if(data[i]!='\n') {
					j = data.find('\n',i);
					if(j==-1) j = l;
					if(style&PROP_STYLE_STORE_COMMENTS) comment.append(data,i,j-i);
//debug_output("comment: [%s]\n",(const char *)comment);
					i = j;
				}
//debug_output("add[%s|%s|%s|%s|%s]\n",(const char *)ind,(const char *)key,(const char *)delim,(const char *)value,(const char *)comment);
				addNode(new node(key,(const char *)value,
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

size_t Properties::load(const char *fn) {
	if(fn && *fn) {
		String data;
		data.include(fn);
		if(data) {
//const char *p = data;
//debug_output("Properties[%s]:\n",fn);
//for(; *p; ++p)
//if(*p&0x80) debug_output("%02X",(unsigned int)(unsigned char)*p);
//else debug_putc(*p);
			return load(data);
		}
	}
	return 0;
}

size_t Properties::save(const char *fn) {
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

size_t Properties::save(FILE *fp) {
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
					fprintf(fp,"[%s]" STRING_ENDL,n->key);
					i += ((Properties *)n->value)->save(fp);
					break;
			}
		}
		if(n->comment) fputs(n->comment,fp);
		fputs(String::endl,fp);
	}
	return i;*/
}

}; /* namespace a */



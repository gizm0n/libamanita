
#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <amanita/Dictionary.h>
#include <amanita/String.h>


namespace a {

Object_Inheritance(Dictionary,Collection)


Dictionary::Dictionary(const char **ws,const value_t *vs,size_t l,bool c) : Collection(),words(0),values(0) {
	createIndex(ws,vs,l,c);
}
Dictionary::Dictionary(const word *ws,size_t l,bool c) : Collection(),words(0),values(0) {
	createIndex(ws,l,c);
}

Dictionary::~Dictionary() {
	if(words) clear();
}


void Dictionary::clear() {
	for(size_t i=0; i<sz; i++) free((char *)words[i].key);
	free(words);
	free(values);
	words = 0,values = 0;
}

void Dictionary::createIndex(const char **ws,const value_t *vs,size_t l,bool c) {
	size_t i;
	word *w;
	if(words) clear();
	if(!l) for(i=0; ws[i]; i++);
	else i = l;
	cap = sz = i;
	words = (word *)malloc(cap*sizeof(word));
	for(i=0; i<sz; i++) {
		w = &words[i];
		w->key = strdup(ws[i]);
		w->value = vs[i];
		w->len = strlen(w->key);
		if(c) a::tolower((char *)w->key);
	}
	createIndex();
}

void Dictionary::createIndex(const word *ws,size_t l,bool c) {
	size_t i;
	word *w;
	if(words) clear();
	if(!l) for(i=0; ws[i].key; i++);
	else i = l;
	cap = sz = i;
	words = (word *)malloc(cap*sizeof(word));
	for(i=0; i<sz; i++) {
		w = &words[i];
		w->key = strdup(ws[i].key);
		w->value = ws[i].value;
		w->len = strlen(w->key);
		if(c) a::tolower((char *)w->key);
	}
	createIndex();
}

void Dictionary::createIndex() {
	size_t i;
	node *n;
	qsort(words,sz,sizeof(word),compareKey);
	values = (word **)malloc(sz*sizeof(word *));
	for(i=0; i<sz; i++) values[i] = &words[i];
	qsort(values,sz,sizeof(word *),compareValue);
	memset(k_index,0,sizeof(k_index));
	for(i=0; i<sz; i++) {
		n = &k_index[(int)*words[i].key];
		if(!n->range) n->offset = i;
		n->range++;
	}
const char *fn = "dictionary.txt";
FILE *fp = fopen(fn,"wb");
if(fp) {
for(i=0; i<sz; i++) fprintf(fp,"word(%lu,\"%s\",length=%lu,value=%" PRIuPTR ")\n",(unsigned long)i,words[i].key,(unsigned long)words[i].len,words[i].value);
for(i=0; i<sz; i++) fprintf(fp,"value(%lu,\"%s\",length=%lu,value=%" PRIuPTR ")\n",(unsigned long)i,values[i]->key,(unsigned long)values[i]->len,values[i]->value);
for(i=0; i<256; i++) fprintf(fp,"k_index(%lu=%c,offset=%lu,value=%lu)\n",(unsigned long)i,(char)i,(unsigned long)k_index[i].offset,(unsigned long)k_index[i].range);
fclose(fp);
} else perror(fn);
}

value_t Dictionary::getValue(const char *w,size_t l) {
	node &n = k_index[(int)*w];
	if(!l) l = strlen(w);
	if(n.range) for(size_t i=n.offset,c=i+n.range; i<c; i++)
		if(l==words[i].len && *w==*words[i].key && !strncmp(w,words[i].key,l)) return words[i].value;
	return 0;
}

const char *Dictionary::getKey(value_t v) {
	for(size_t i=0; i<sz; i++) if(values[i]->value==v) return values[i]->key;
	return 0;
}

int Dictionary::compareKey(const void *w1, const void *w2) {
	return strcmp(((word *)w1)->key,((word *)w2)->key);
}

int Dictionary::compareValue(const void *w1, const void *w2) {
	return ((word *)w1)->value-((word *)w2)->value;
}


}; /* namespace a */




#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libamanita/Dictionary.h>
#include <libamanita/String.h>


RttiObjectInheritance(Dictionary,Collection);


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
	for(size_t i=0ul; i<sz; i++) free(words[i].key);
	free(words);
	free(values);
	words = 0,values = 0;
}

void Dictionary::createIndex(const char **ws,const value_t *vs,size_t l,bool c) {
	size_t i;
	word *w;
	if(words) clear();
	if(!l) for(i=0ul; ws[i]; i++);
	else i = l;
	cap = sz = i;
	words = (word *)malloc(cap*sizeof(word));
	for(i=0ul; i<sz; i++) {
		w = &words[i];
		w->key = strdup(ws[i]);
		w->value = vs[i];
		w->len = strlen(w->key);
		if(c) String::toLower(w->key);
	}
	createIndex();
}

void Dictionary::createIndex(const word *ws,size_t l,bool c) {
	size_t i;
	word *w;
	if(words) clear();
	if(!l) for(i=0ul; ws[i].key; i++);
	else i = l;
	cap = sz = i;
	words = (word *)malloc(cap*sizeof(word));
	for(i=0ul; i<sz; i++) {
		w = &words[i];
		w->key = strdup(ws[i].key);
		w->value = ws[i].value;
		w->len = strlen(w->key);
		if(c) String::toLower(w->key);
	}
	createIndex();
}

void Dictionary::createIndex() {
	size_t i;
	node *n;
	qsort(words,sz,sizeof(word),compareKey);
	values = (word **)malloc(sz*sizeof(word *));
	for(i=0ul; i<sz; i++) values[i] = &words[i];
	qsort(values,sz,sizeof(word *),compareValue);
	memset(k_index,0,sizeof(k_index));
	for(i=0ul; i<sz; i++) {
		n = &k_index[(int)*words[i].key];
		if(!n->range) n->offset = i;
		n->range++;
	}
FILE *fp = fopen("dictionary.txt","w");
for(i=0ul; i<sz; i++) fprintf(fp,"word(%d,\"%s\",length=%d,value=%lu)\n",i,words[i].key,words[i].len,words[i].value);
for(i=0ul; i<sz; i++) fprintf(fp,"value(%d,\"%s\",length=%d,value=%lu)\n",i,values[i]->key,values[i]->len,values[i]->value);
for(i=0ul; i<256; i++) fprintf(fp,"k_index(%d=%c,offset=%d,value=%d)\n",i,i,k_index[i].offset,k_index[i].range);
fclose(fp);
}

value_t Dictionary::getValue(const char *w,size_t l) {
	node &n = k_index[(int)*w];
	if(!l) l = strlen(w);
	if(n.range) for(size_t i=n.offset,c=i+n.range; i<c; i++)
		if(l==words[i].len && *w==*words[i].key && !strncmp(w,words[i].key,l)) return words[i].value;
	return 0ul;
}

const char *Dictionary::getKey(value_t v) {
	for(size_t i=0ul; i<sz; i++) if(values[i]->value==v) return values[i]->key;
	return 0;
}

int Dictionary::compareKey(const void *w1, const void *w2) {
	return strcmp(((word *)w1)->key,((word *)w2)->key);
}

int Dictionary::compareValue(const void *w1, const void *w2) {
	return ((word *)w1)->value-((word *)w2)->value;
}



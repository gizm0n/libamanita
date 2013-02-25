
#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <amanita/List.h>


namespace a {


Object_Inheritance(List,Collection)


List::List() : Collection(),first(0),last(0) {}
List::~List() { clear(); }


size_t List::push(value_t v,type_t t) {
	node *n = new node(v,t,last? last : 0,0);
	if(last) last->next = n;
	last = n;
	if(!first) first = n;
	return ++sz;
}

size_t List::unshift(value_t v,type_t t) {
	node *n = new node(v,t,0,first? first : 0);
	if(first) first->prev = n;
	first = n;
	if(!last) last = n;
	return ++sz;
}

value_t List::pop() {
	value_t v = 0;
	if(sz) {
		node *n = last;
		v = n->value;
		last = n->prev;
		if(last) last->next = 0;
		else first = 0;
		n->value = 0;
		delete n;
		--sz;
	}
	return v;
}

value_t List::shift() {
	value_t v = 0;
	if(sz) {
		node *n = first;
		v = n->value;
		first = n->next;
		if(first) first->prev = 0;
		else last = 0;
		n->value = 0;
		delete n;
		--sz;
	}
	return v;
}

size_t List::remove(value_t v,type_t t) {
	size_t n = 0;
	if(v && sz) {
		node *n1 = last,*n2;
		while(n1) {
			if(n1->value==v && (!t || n1->type==t)) {
				n2 = n1;
				if(n1->prev) n1->prev->next = n1->next;
				else first = n1->next;
				if(n1->next) n1->next->prev = n1->prev;
				else last = n1->prev;
				n1 = n1->prev;
				delete n2;
				++n;
			} else n1 = n1->prev;
		}
		sz -= n;
	}
	return n;
}

size_t List::remove(const char *v) {
	size_t n = 0;
	if(v && sz) {
		node *n1 = last,*n2;
		while(n1) {
			if(n1->type==CHAR_P && !strcmp((char *)n1->value,v)) {
				n2 = n1;
				if(n1->prev) n1->prev->next = n1->next;
				else first = n1->next;
				if(n1->next) n1->next->prev = n1->prev;
				else last = n1->prev;
				n1 = n1->prev;
				delete n2;
				++n;
			} else n1 = n1->prev;
		}
		sz -= n;
	}
	return n;
}

void List::removeAt(long n) {
	if(sz) {
		node *n1 = first;
		if((n=alignToSize(n))<0) return;
		for(; n1 && n; --n,n1=n1->next);
		if(!n1) return;
		if(n1->prev) n1->prev->next = n1->next;
		else first = n1->next;
		if(n1->next) n1->next->prev = n1->prev;
		else last = n1->prev;
		delete n1;
		--sz;
	}
}

void List::clear() {
	if(sz) {
		node *n0 = 0,*n1 = first;
		while(n1) {
			n0 = n1,n1 = n1->next;
			delete n0;
		}
	}
	sz = 0,first = 0,last = 0;
}


}; /* namespace a */



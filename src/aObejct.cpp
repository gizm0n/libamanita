
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libamanita/Object.h>

uint32_t Class::id_index = 0;

Class::Class(const char *nm,Class *s) : name(nm),super(s) {
	id = Class::id_index++;
	if(super) {
		if(!super->sub) super->sub = (Class **)malloc(sizeof(Class *));
		else super->sub = (Class **)realloc(super->sub,(super->nsub+1)*sizeof(Class *));
		super->sub[super->nsub++] = this;
		Class *c = super;
		while(c->super) c->d = 0,c = c->super;
		c->setDepth(0);
	}
printf("Class::Class(name=%s,id=%" PRIu32 ",d=%d)\n",name,id,d);
fflush(stdout);
if(super) {
	Class *c = super;
	char ind[64] = "";
	while(c->super) c = c->super;
	c->print(ind);
}
}

Class::~Class() {
	if(sub) free(sub);
	sub = 0,nsub = 0ul;
}

bool Class::instanceOf(Class &c) {
	if(id==c.id) return true;
	if(d<=c.d) return false;
	Class *s = super;
	while(s->d>c.d) s = s->super;
	return s->id==c.id;
}

void Class::setDepth(int n) {
	d = n;
	if(nsub) for(size_t i=0ul; i<nsub; i++) if(sub[i]->d!=d+1) sub[i]->setDepth(d+1);
}

void Class::print(char *ind) {
	if(d) ind[d-1] = '\t';
	ind[d] = '\0';
	printf("%s->%s(id=%" PRIu32 ",d=%d)\n",ind,name,id,d);
	fflush(stdout);
	if(nsub) for(size_t i=0ul; i<nsub; i++) sub[i]->print(ind);
}

Class Object::instance("Object",0);



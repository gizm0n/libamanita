
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libamanita/aObject.h>

uint32_t aClass::id_index = 0;

aClass::aClass(const char *nm,aClass *s) : name(nm),super(s) {
	id = aClass::id_index++;
	if(super) {
		if(!super->sub) super->sub = (aClass **)malloc(sizeof(aClass *));
		else super->sub = (aClass **)realloc(super->sub,(super->nsub+1)*sizeof(aClass *));
		super->sub[super->nsub++] = this;
		aClass *c = super;
		while(c->super) c->d = 0,c = c->super;
		c->setDepth(0);
	}
printf("aClass::aClass(name=%s,id=%" PRIu32 ",d=%d)\n",name,id,d);
fflush(stdout);
if(super) {
	aClass *c = super;
	char ind[64] = "";
	while(c->super) c = c->super;
	c->print(ind);
}
}

aClass::~aClass() {
	if(sub) free(sub);
	sub = 0,nsub = 0ul;
}

bool aClass::instanceOf(aClass &c) {
	if(id==c.id) return true;
	if(d<=c.d) return false;
	aClass *s = super;
	while(s->d>c.d) s = s->super;
	return s->id==c.id;
}

void aClass::setDepth(int n) {
	d = n;
	if(nsub) for(size_t i=0ul; i<nsub; i++) if(sub[i]->d!=d+1) sub[i]->setDepth(d+1);
}

void aClass::print(char *ind) {
	if(d) ind[d-1] = '\t';
	ind[d] = '\0';
	printf("%s->%s(id=%" PRIu32 ",d=%d)\n",ind,name,id,d);
	fflush(stdout);
	if(nsub) for(size_t i=0ul; i<nsub; i++) sub[i]->print(ind);
}

aClass aObject::instance("aObject",0);




#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <amanita/aObject.h>

uint16_t aClass::id_index = 0;

aClass::aClass(const char *nm,aClass *s) : name(nm),super(s) {
	id = aClass::id_index++;
debug_output("aClass::aClass(name: %s, id: %" PRIu32 ", super: %s [%" PRIu32 ",%p,%p,%d])\n",name,id,super? super->name : "-",super? super->id : -1,super,super? super->sub : 0,super? super->nsub : 0);
	if(super) {
		if(!super->sub) super->sub = (aClass **)malloc(sizeof(aClass *));
		else super->sub = (aClass **)realloc(super->sub,(super->nsub+1)*sizeof(aClass *));
		super->sub[super->nsub++] = this;
/*		aClass *c = super;
		while(c->super) c->d = 0,c = c->super;
		c->setDepth(0);*/
	}
/*if(super) {
	aClass *c = super;
	char ind[64] = "";
	while(c->super) c = c->super;
	c->print(ind);
}*/
}

aClass::~aClass() {
	if(sub) free(sub);
	sub = 0,nsub = 0;
}

bool aClass::instanceOf(aClass &c) {
	if(&c==this) return true;
	aClass *s = super;
	while(s && s!=&c) s = s->super;
	return s==&c;
}

/*void aClass::setDepth(int n) {
	d = n;
	if(nsub) for(size_t i=0; i<nsub; i++) if(sub[i]->d!=d+1) sub[i]->setDepth(d+1);
}*/

void aClass::print(FILE *fp,char *ind,int d) {
	if(d) ind[d-1] = '\t';
	ind[d] = '\0';
	fprintf(fp,"%s%s(id=%" PRIu32 ")\n",ind,name,id);
	if(nsub) for(int i=0; i<nsub; i++) sub[i]->print(fp,ind,d+1);
}

aClass aObject::instance("aObject",0);


void aObject::printClasses(FILE *fp) {
	char ind[256] = "";
	if(!fp) fp = stdout;
	fprintf(fp,"\nAll classes inheriting aObject:\n[\n");
	instance.print(fp,ind,0);
	fprintf(fp,"]\n\n");
	fflush(fp);
}



#include "_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <amanita/Object.h>


namespace a {

uint16_t Class::id_index = 0;

Class::Class(const char *nm,Class *s) : name(nm),super(s) {
	id = Class::id_index++;
//debug_output("Class::Class(name: %s, id: %" PRIu32 ", super: %s [%" PRIu32 ",%p,%p,%d])\n",name,id,super? super->name : "-",super? super->id : -1,super,super? super->sub : 0,super? super->nsub : 0);
	if(super) {
		if(!super->sub) super->sub = (Class **)malloc(sizeof(Class *));
		else super->sub = (Class **)realloc(super->sub,(super->nsub+1)*sizeof(Class *));
		super->sub[super->nsub++] = this;
/*		Class *c = super;
		while(c->super) c->d = 0,c = c->super;
		c->setDepth(0);*/
	}
/*if(super) {
	Class *c = super;
	char ind[64] = "";
	while(c->super) c = c->super;
	c->print(ind);
}*/
}

Class::~Class() {
	if(sub) free(sub);
	sub = 0,nsub = 0;
}

bool Class::instanceOf(Class &c) {
	if(&c==this) return true;
	Class *s = super;
	while(s && s!=&c) s = s->super;
	return s==&c;
}

/*void Class::setDepth(int n) {
	d = n;
	if(nsub) for(size_t i=0; i<nsub; i++) if(sub[i]->d!=d+1) sub[i]->setDepth(d+1);
}*/

void Class::print(FILE *fp,char *ind,int d) {
	if(d) ind[d-1] = '\t';
	ind[d] = '\0';
	fprintf(fp,"%s%s(id=%" PRIu32 ")\n",ind,name,id);
	if(nsub) for(int i=0; i<nsub; i++) sub[i]->print(fp,ind,d+1);
}

Class Object::instance("Object",0);


void Object::printClasses(FILE *fp) {
	char ind[256] = "";
	if(!fp) fp = stdout;
	fprintf(fp,"\nAll classes inheriting Object:\n[\n");
	instance.print(fp,ind,0);
	fprintf(fp,"]\n\n");
	fflush(fp);
}

}; /* namespace a */



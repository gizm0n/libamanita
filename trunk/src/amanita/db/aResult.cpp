
#include "../_config.h"
#include <amanita/db/aRecord.h>
#include <amanita/db/aResult.h>



aObject_Inheritance(aResult,aVector)


aResult::~aResult() {
	clear();
}

void aResult::clear() {
	if(cols) delete cols;
	cols = 0;
	if(size()) {
		int i;
		for(i=size()-1; i>=0; i--) delete (aRecord *)(*this)[i];
		aVector::clear();
	}
}

void aResult::addRecord(int n,char **a,char **c) {
	if(!cols) cols = new aRecord(*this,n,c);
	*this << new aRecord(*this,n,a);
}




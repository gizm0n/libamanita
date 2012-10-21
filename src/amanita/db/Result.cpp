
#include "../_config.h"
#include <amanita/db/Record.h>
#include <amanita/db/Result.h>



namespace a {

Object_Inheritance(Result,Vector)


Result::~Result() {
	clear();
}

void Result::clear() {
	if(cols) delete cols;
	cols = 0;
	if(size()) {
		int i;
		for(i=size()-1; i>=0; i--) delete (Record *)(*this)[i];
		Vector::clear();
	}
}

void Result::addRecord(int n,char **a,char **c) {
	if(!cols) cols = new Record(*this,n,c);
	*this << new Record(*this,n,a);
}

}; /* namespace a */




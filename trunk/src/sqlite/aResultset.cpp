

#include <libamanita/sqlite/aResultset.h>


aResultset::~aResultset() {
	if(cols) { delete cols;cols = 0; }
	for(int i=size()-1; i>=0; i--) delete (aRecord *)(*this)[i];
}




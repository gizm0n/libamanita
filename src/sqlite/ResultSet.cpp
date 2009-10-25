

#include <libamanita/sqlite/ResultSet.h>


ResultSet::~ResultSet() {
	if(cols) { delete cols;cols = 0; }
	for(int i=size()-1; i>=0; i--) delete (Record *)(*this)[i];
}




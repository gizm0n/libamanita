
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <libamanita/sqlite/aRecord.h>
#include <libamanita/sqlite/aResultset.h>
#include <libamanita/sqlite/aSQLite.h>


aRecord::aRecord(aResultset &rs,char **d,unsigned int s) {
	if(d && s) {
		data = (char **)malloc(s*sizeof(char *));
		sz = s;
		for(size_t i=0,l; i<sz; i++) {
			if(d[i]) {
				l = (strlen(d[i])+1)*sizeof(char);
				data[i] = (char *)malloc(l);
				memcpy(data[i],d[i],l);
			} else data[i] = 0;
		}
	} else data = 0,sz = 0;
}

aRecord::~aRecord() {
	if(data && sz) {
		for(size_t i=0; i<sz; i++) free(data[i]);
		free(data);
		data = 0,sz = 0;
	}
}


